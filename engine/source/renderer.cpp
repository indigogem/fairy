#include "renderer.h"
#include "IwGx.h"
_BEGIN_NAMESPACE_

CRenderer * CRenderer::mInstance = NULL;

CRenderer::CRenderer()
	: mOnResetparams(NULL)
{
	mVB = NULL;
	mUVB = NULL;
	mCB = NULL;
	mIB = NULL;
	mCurrMtl = NULL;
	mVBSize = 0;
	mUVBSize = 0;
	mIBSize = 0;
	mCBSize = 0;
	SetBufferType(IW_GX_QUAD_LIST);

}

CRenderer::~CRenderer()
{
	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);
	SAFE_DELETE(mDefaultMaterial);
}

void CRenderer::Init()
{
	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);
	UpdateDeviceParams();
	mDefaultMaterial = new CIwMaterial;
	mDefaultMaterial->SetAlphaMode( CIwMaterial::ALPHA_BLEND ); 
	mCurrMtl = mDefaultMaterial;

	//mCamera.pos.x = 0;
	//mCamera.pos.y = 0;
	//mCamera.pos = CIwFVec2(0, 0);
	//mCamera.scale = 1.f;
	mCamera.Reset();

	AllocBuffers();
}


CRenderer & CRenderer::GetInst()
{
	if( mInstance == NULL )
	{
		mInstance = new CRenderer();
	}
	return *mInstance;
}

void CRenderer::Destroy()
{
	mInstance->FreeBuffers();
	SAFE_DELETE(mInstance);
}

void CRenderer::Begin()
{
	s3eDeviceYield(0);
	IwGxLightingOff();
	//IwGxLightingAmbient(true);
	IwGxSetScreenSpaceOrg( &CIwSVec2::g_Zero );
	//IwGxSetScreenSpaceSlot( 0 );
	//IwGxLightingDiffuse( false );
	IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F); 

	if (mIsResetDeviceparam)
	{
		mIsResetDeviceparam = false;
		UpdateDeviceParams();
		if ( mOnResetparams != NULL )
		{
			mOnResetparams();
		}		
	}

	ResetBuffers();
}

void CRenderer::End()
{
	DrawBatch();

	IwGxFlush(); 

	IwGxSwapBuffers(); 
	
	ClearMtlCache();
}

void CRenderer::DefaultMaterial()
{
	CIwMaterial * currMat = IwGxGetMaterial();
	if (currMat == NULL || currMat != mDefaultMaterial)
	{
		IwGxSetMaterial(mDefaultMaterial);
	}
}

int32 CRenderer::ScreenSizeChangeCallback( void* systemData, void* userData )
{
	CRenderer::GetInst().ResetDeviceParams();
	return true;
}

void CRenderer::ResetDeviceParams()
{
	mIsResetDeviceparam = true;
}

void CRenderer::UpdateDeviceParams()
{
	mDeviceParams.screenWidth  = IwGxGetScreenWidth();
	mDeviceParams.screenHeight = IwGxGetScreenHeight();
	mDeviceParams.screenOrient = IW_GX_ORIENT_90;
	mDeviceParams.bkgColor.Set(0, 0, 0, 255);
	mDeviceParams.isClearBackground = true;
	mDeviceParams.virtualWidth = VIRTUAL_WIDTH;
	mDeviceParams.virtualHeight = VIRTUAL_HEIGHT;
	mDeviceParams.screenFactor = (float)mDeviceParams.screenWidth / (float)mDeviceParams.screenHeight;
	mDeviceParams.offsetX = 0;
	mDeviceParams.offsetY = 0;

	if (mDeviceParams.screenFactor >= 1.5)
	{
		mDeviceParams.scale = (float)mDeviceParams.screenHeight / mDeviceParams.virtualHeight;

		if (mDeviceParams.screenWidth != mDeviceParams.virtualWidth * mDeviceParams.scale)
		{
			mDeviceParams.offsetX = (uint32)(mDeviceParams.screenWidth - mDeviceParams.virtualWidth * mDeviceParams.scale)/2;
		}
	}

	if (mDeviceParams.screenFactor < 1.5)
	{
		mDeviceParams.scale = (float)mDeviceParams.screenWidth / mDeviceParams.virtualWidth;
		if (mDeviceParams.screenHeight != mDeviceParams.virtualHeight * mDeviceParams.scale)
		{
			mDeviceParams.offsetY = (uint32)(mDeviceParams.screenHeight - mDeviceParams.virtualHeight * mDeviceParams.scale)/2;
		}
	}
	 //IwGxSetScissorScreenSpace(mDeviceParams.offsetX, mDeviceParams.screenHeight, mDeviceParams.screenWidth - mDeviceParams.offsetX, mDeviceParams.screenHeight);

	//IwGxSetScissorScreenSpace(100, 100, 100, 100);
}

void CRenderer::Draw( const SQuad & q )
{
	CIwMaterial* pMat = GetMtlCached( q.texture, q.blend ); 

	Draw(IW_GX_QUAD_LIST, &q.v[0], &q.uv[0], NULL, &q.colour[0], 4, pMat);
}

void CRenderer::Draw( IwGxPrimType type, const CIwSVec2 * vertex, const CIwSVec2 * uv, const uint16* indexStream, const CIwColour * colour, int32 vertexCount, CIwMaterial * mat )
{
	if ( mat == NULL )
	{
		mat = mDefaultMaterial;
	}
	if (( GetVBSize() + vertexCount >= RENDER_MAX_BUFF_SIZE 
		|| (mat != mCurrMtl) 
		|| (GetBufferType() != type)
		|| (uv == NULL && GetUVBSize() != 0) 
		|| (indexStream == NULL && GetIBSize() != 0)
		|| (colour == NULL && GetCBSize() != 0)
		))
	{
		DrawBatch();
	}

	if ( (uint32)vertexCount >= RENDER_MAX_BUFF_SIZE )
	{
		CIwSVec2 * _v = IW_GX_ALLOC(CIwSVec2, vertexCount);
		CIwSVec2 * _uv = IW_GX_ALLOC(CIwSVec2, vertexCount);
		CIwColour * _c = IW_GX_ALLOC(CIwColour, vertexCount);

		uint16 * _is = NULL;
		if (indexStream != NULL )
		{
			_is = IW_GX_ALLOC(uint16, vertexCount);
			memcpy(_is, indexStream, vertexCount*sizeof(uint16));
		}
		memcpy(&_v[0], &vertex[0], vertexCount*sizeof(CIwSVec2));
		memcpy(&_uv[0], &uv[0], vertexCount*sizeof(CIwSVec2));
		memcpy(&_c[0], &colour[0], vertexCount*sizeof(CIwColour));

		CameraTransform( &_v[0], vertexCount);
		DrawDirect(type, _v, _uv, _is, _c, vertexCount, mat);
	}
	else
	{
		mCurrMtl  = mat;
		SetBufferType(type);
		AddBuffer(type, vertex, uv, indexStream, colour, vertexCount);
		CameraTransform( &mVB[GetVBSize() - vertexCount], vertexCount);
	}	
}

void CRenderer::SetOnResetParamsCallback( TResetParamsCallback c )
{
	mOnResetparams = c;
}

const SDeviceParam & CRenderer::GetDeviceParam() const
{
	return mDeviceParams;
}

CIwMaterial * CRenderer::GetMtlCached( CIwTexture * tex, CIwMaterial::AlphaMode blend )
{
	MaterialContainer::iterator it = mMtlCache.begin(), ite = mMtlCache.end();
	for (; it != ite; it++ )
	{
		if ( (*it)->GetTexture() == tex && (*it)->GetAlphaMode() == blend )
		{
			return *it;
		}
	}

	CIwMaterial * pMat = new CIwMaterial();
	pMat->SetTexture( tex ); 
	pMat->SetAlphaMode( blend ); 
	//pMat->SetBlendMode( CIwMaterial::BLEND_MODULATE_4X );
	pMat->SetFiltering( true );
	pMat->SetClamping(true);

	mMtlCache.push_back( pMat );

	return pMat;
}

void CRenderer::DrawBatch()
{
	if ( GetVBSize() > 0 )
	{
		IwGxLightingOff();
		IwGxSetMaterial( mCurrMtl ); 
		IwGxSetUVStream( mUVB );
		IwGxSetColStream(mCB, mCBSize); 
		IwGxSetVertStreamScreenSpaceSubPixel( mVB, mVBSize );
		if ( mIBSize == 0 )
		{
			IwGxDrawPrims(mBuffType, NULL, mVBSize);
		}
		else
		{
			IwGxDrawPrims(mBuffType, mIB, mIBSize);
		}
		
		
		mCurrMtl = NULL;
		//DefaultMaterial();
	
		IwGxFlush(); 
		IwGxSetColStream(NULL); 
		//IwGxSetUVStream( NULL );
		ResetBuffers();
	}
}

void CRenderer::ClearMtlCache()
{
	MaterialContainer::iterator it = mMtlCache.begin(), ite = mMtlCache.end();
	for (; it != ite; it++ )
	{
		SAFE_DELETE( *it );
	}
	mMtlCache.clear();
}

CCamera2D & CRenderer::GetCamera()
{
	return mCamera;
}

void CRenderer::SetCamera( const CCamera2D& cam )
{
	mCamera = cam;
}

void CCamera2D::SetPos( const CIwFVec2 & pos )
{ 
	mPos = pos; 
}

void CCamera2D::Reset()
{ 
	mPos = CIwFVec2(0, 0); 
	mScale = 1.f; 
}

void CRenderer::Flush()
{
	DrawBatch();
}

void CRenderer::DrawRect( float x, float y, float w, float h, const CIwColour & color )
{
	int32 nVerts = 5;

	CIwSVec2 *  vertex           = IW_GX_ALLOC(CIwSVec2, nVerts); 
	uint16*     indexStream      = IW_GX_ALLOC(uint16,   nVerts);
	CIwColour*  colour           = IW_GX_ALLOC(CIwColour,nVerts);
	int count = 0;

	for (int i = 0; i < nVerts; i++)
	{
		colour[i]   = color;
		indexStream[count] = count; 
		count++;
	}

	vertex[0].x = FLOAT_TO_FIXED(x);
	vertex[0].y = FLOAT_TO_FIXED(y);

	vertex[1].x = FLOAT_TO_FIXED((x + w));
	vertex[1].y = FLOAT_TO_FIXED(y);

	vertex[2].x = FLOAT_TO_FIXED((x + w));
	vertex[2].y = FLOAT_TO_FIXED((y + h));

	vertex[3].x = FLOAT_TO_FIXED(x);
	vertex[3].y = FLOAT_TO_FIXED((y + h));

	vertex[4] = vertex[0];


	Draw(IW_GX_LINE_STRIP, vertex, NULL, indexStream, colour, nVerts, NULL );
}



void CRenderer::RenderText( CIwGxFont * font, const CIwRect & rect, const CIwColour & color, const CIwStringS & txt, IwGxFontAlignHor hAlign, IwGxFontAlignVer vAlign )
{
	iwsfixed scale = IW_SFIXED(mDeviceParams.scale);
	iwsfixed offX  = mDeviceParams.offsetX;
	iwsfixed offY  = mDeviceParams.offsetY;

	CIwRect r;
	r.x = IW_FIXED_MUL(rect.x, scale) + offX;
	r.y = IW_FIXED_MUL(rect.y, scale) + offY;
	r.w = IW_FIXED_MUL(rect.w, scale);
	r.h = IW_FIXED_MUL(rect.h, scale);

	Flush();
	
	DefaultMaterial();

	IwGxFontClearFlags(0xffffffff);
	IwGxLightingOn();	
	IwGxFontSetFont( font );
	IwGxFontSetCol( color );
	//IwGxFontSetScale( scale );

	//Set the formatting rect - this controls where the text appears and what it is formatted against
	IwGxFontSetRect(r);
	IwGxFontSetAlignmentVer(vAlign);
	IwGxFontSetAlignmentHor(hAlign);

	//Draw the text
	IwGxFontDrawText(txt.c_str());
	IwGxLightingOff();
	IwGxFontFreeBuffers();
	Flush();

}

void CRenderer::CameraTransform( CIwSVec2 * vertex, int vertexCount )
{
	iwsfixed scale = IW_SFIXED(mDeviceParams.scale);

	CIwSVec2 t( FLOAT_TO_FIXED(mCamera.GetPos().x), FLOAT_TO_FIXED(mCamera.GetPos().y) );
	iwsfixed csacale = IW_SFIXED( mCamera.GetScale() );

	for (int i = 0; i < vertexCount; i++)
	{
		vertex[i] -= t;
		vertex[i].x = IW_FIXED_MUL(vertex[i].x, csacale);
		vertex[i].y = IW_FIXED_MUL(vertex[i].y, csacale);

		vertex[i].x = IW_FIXED_MUL(vertex[i].x, scale) + ((mDeviceParams.offsetX) << 3);
		vertex[i].y = IW_FIXED_MUL(vertex[i].y, scale) + ((mDeviceParams.offsetY) << 3);
	}
}

void CRenderer::AllocBuffers()
{
	IwAssert(RENDER, (mVB == NULL && mUVB == NULL && mCB == NULL) );

	mVB  = (CIwSVec2*)calloc( RENDER_MAX_BUFF_SIZE, sizeof(CIwSVec2) );
	mUVB = (CIwSVec2*)calloc( RENDER_MAX_BUFF_SIZE, sizeof(CIwSVec2) );
	mCB  = (CIwColour*)calloc( RENDER_MAX_BUFF_SIZE, sizeof(CIwColour) );
	mIB  = (uint16*)calloc( RENDER_MAX_BUFF_SIZE, sizeof(uint16) );
	ResetBuffers();

}

void CRenderer::FreeBuffers()
{
	free(mVB);
	mVB = NULL;

	free(mUVB);
	mUVB = NULL;

	free(mCB);
	mCB = NULL;

	free(mIB);
	mIB = NULL;

	mVBSize = 0;
	mCBSize = 0;
	mUVBSize = 0;
	mIBSize = 0;
}

void CRenderer::ResetBuffers()
{
	mVBSize = 0;
	mCBSize = 0;
	mUVBSize = 0;
	mIBSize = 0;
}


void CRenderer::AddBuffer( IwGxPrimType type, const CIwSVec2 * vertex, const CIwSVec2 * uv, const uint16 * indexStream, const CIwColour * colour, int32 vertexCount )
{
	IwAssert(RENDER, mBuffType == type);
	IwAssert(RENDER, vertex != NULL);
	//IwAssert(RENDER, (mUVBSize ==0 && uv == NULL));
	//IwAssert(RENDER, (mIBSize ==0 && indexStream == NULL));
	//IwAssert(RENDER, (mCBSize ==0 && colour == NULL));

	memcpy( &mVB[mVBSize], vertex, vertexCount*sizeof(CIwSVec2) );
	mVBSize += vertexCount;

	if ( uv != NULL )
	{
		memcpy( &mUVB[mUVBSize], uv, vertexCount*sizeof(CIwSVec2) );
		mUVBSize += vertexCount;
	}

	if ( indexStream != NULL )
	{
		memcpy( &mIB[mIBSize], indexStream, vertexCount*sizeof(uint16) );
		mIBSize += vertexCount;
	}
	
	if ( colour != NULL )
	{
		memcpy( &mCB[mCBSize], colour, vertexCount*sizeof(CIwColour) );
		mCBSize += vertexCount;
	}	

}

void CRenderer::SetBufferType( IwGxPrimType type )
{
	mBuffType = type;
}

IwGxPrimType CRenderer::GetBufferType() const
{
	return mBuffType;
}

uint32 CRenderer::GetVBSize() const
{
	return mVBSize;
}

uint32 CRenderer::GetUVBSize() const
{
	return mUVBSize;
}

uint32 CRenderer::GetCBSize() const
{
	return mCBSize;
}

uint32 CRenderer::GetIBSize() const
{
	return mIBSize;
}
    
void CRenderer::DrawDirect( IwGxPrimType type, CIwSVec2 * vertex, CIwSVec2 * uv, uint16* indexStream, CIwColour * colour, int32 vertexCount, CIwMaterial * mat )
{
	IwGxLightingOff();
	IwGxSetMaterial( mat ); 
	IwGxSetUVStream( uv );
	IwGxSetColStream(colour, vertexCount); 
	IwGxSetVertStreamScreenSpaceSubPixel( vertex, vertexCount );
	IwGxDrawPrims(type, indexStream, vertexCount);

	IwGxFlush(); 
	IwGxSetColStream(NULL); 
}

CIwMaterial * CRenderer::GetDefaultMaterial() const
{
	return mDefaultMaterial;
}

_END_NAMESPACE_

