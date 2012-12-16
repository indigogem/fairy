#include "IwGx.h"
#include "sprite.h"
#include "math.h"
#include "IwMaterial.h"

_BEGIN_NAMESPACE_

CSprite::CSprite()
{
	mQuad.texture = NULL;

	for(int nrVertex = 0; nrVertex < 4; nrVertex++) 
	{
		mQuad.colour[nrVertex].Set(255, 255, 255, 255);
	}

	SetTexels(0, 0, 1, 1);

	mQuad.blend  = CIwMaterial::ALPHA_BLEND;

	mHotX = 0;
	mHotY = 0;
	mHFlip = false;
	mVFlip = false;

	mTexWidth = 0;
	mTexHeight = 0;
	mWidth = 0;
	mHeight = 0;

	mTx = 0;
	mTw = 0;
	mTy = 0;
	mTh = 0;

	//mId = ++sCount;
}

CSprite::CSprite( const CSprite& s )
{
	mQuad      = s.mQuad;
	mWidth     = s.mWidth;
	mHeight    = s.mHeight;
	mTexWidth  = s.mTexWidth;
	mTexHeight = s.mTexHeight;
	mHotX	   = s.mHotX;
	mHotY	   = s.mHotY;
	mHFlip	   = s.mHFlip;
	mVFlip	   = s.mVFlip;
	mTx		   = s.mTx;
	mTw        = s.mTw;
	mTy        = s.mTy;
	mTh        = s.mTh;

	//mId = ++sCount;
}

CSprite::CSprite( CIwTexture * tex, float tx, float ty, float tw, float th )
{
	mQuad.texture = NULL;

	for(int nrVertex = 0; nrVertex < 4; nrVertex++) 
	{
		mQuad.colour[nrVertex].Set(255, 255, 255, 255);
	}

	SetTexels(0, 0, 1, 1);

	mQuad.blend  = CIwMaterial::ALPHA_BLEND;

	mHotX = 0;
	mHotY = 0;
	mHFlip = false;
	mVFlip = false;

	mTexWidth = 0;
	mTexHeight = 0;
	mWidth = 0;
	mHeight = 0;

	SetTexture( tex );
	mTx = tx;
	mTy = ty;
	mTw = tw;
	mTh = th;
	SetTexels(mTx/mTexWidth, mTy/mTexHeight, mTw/mTexWidth, mTh/mTexHeight);
}

CSprite::~CSprite()
{

}

void CSprite::SetTexture( CIwTexture * tex )
{
	mQuad.texture = tex;

    mTexWidth  = (float)tex->GetWidth();
    mTexHeight = (float)tex->GetHeight();

	mWidth  = mTexWidth;
	mHeight = mTexHeight;

	mTx = 0;
	mTw = mTexWidth;
	mTy = 0;
	mTh = mTexHeight;

	
	//mHotX = mWidth / 2;
	//mHotY = mHeight / 2;

	//mHotX = 0;
	//mHotY = 0;
}

void CSprite::SetColor( CIwColour c )
{
	for(int nrVertex = 0; nrVertex < 4; nrVertex++) 
	{
		mQuad.colour[nrVertex] = c;
	}
}


//#define IW_SUBPIXEL_FROM_FLOAT(x) ((x << 3))
void CSprite::RenderEx( float x, float y, float rot, float hscale, float vscale )
{
	float tx1, ty1, tx2, ty2; 

	tx1 = -mHotX * hscale; 
	ty1 = -mHotY * vscale; 
	tx2 = (mWidth - mHotX) * hscale ; 
	ty2 = (mHeight - mHotY) * vscale; 


	mQuad.v[0].x = IW_SUBPIXEL_FROM_FLOAT(tx1);
	mQuad.v[0].y = IW_SUBPIXEL_FROM_FLOAT(ty1);

	mQuad.v[1].x = IW_SUBPIXEL_FROM_FLOAT(tx1);
	mQuad.v[1].y = IW_SUBPIXEL_FROM_FLOAT(ty2);

	mQuad.v[2].x = IW_SUBPIXEL_FROM_FLOAT(tx2);
	mQuad.v[2].y = IW_SUBPIXEL_FROM_FLOAT(ty2);

	mQuad.v[3].x = IW_SUBPIXEL_FROM_FLOAT(tx2);
	mQuad.v[3].y = IW_SUBPIXEL_FROM_FLOAT(ty1);

	/*mQuad.v[0].x = int16(tx1);
	mQuad.v[0].y = int16(ty1);

	mQuad.v[1].x = int16(tx1);
	mQuad.v[1].y = int16(ty2);

	mQuad.v[2].x = int16(tx2);
	mQuad.v[2].y = int16(ty2);

	mQuad.v[3].x = int16(tx2);
	mQuad.v[3].y = int16(ty1);*/


	CIwMat2D m;
	m.SetIdentity();
	m.SetRot( IW_ANGLE_FROM_RADIANS(rot));//, CIwVec2(IW_SUBPIXEL_FROM_FLOAT(mHotX), IW_SUBPIXEL_FROM_FLOAT(mHotY)));
	m.SetTrans(CIwVec2(IW_SUBPIXEL_FROM_FLOAT(x), IW_SUBPIXEL_FROM_FLOAT(y)));
	m.Normalise();
	for (int i = 0; i < 4; i++)
	{
		//mQuad.v[i] = m.TransformVec(mQuad.v[i] << 3);
		mQuad.v[i] = m.TransformVec(mQuad.v[i]);
	}		 

	CRenderer::GetInst().Draw( mQuad );
}

void CSprite::SetFlip( bool isHFlip, bool isVFlip )
{
	int16 tx = 0, ty = 0;

	if (mHFlip != isHFlip)
	{
		tx = mQuad.uv[0].x; mQuad.uv[0].x = mQuad.uv[1].x; mQuad.uv[1].x = tx;
		ty = mQuad.uv[0].y; mQuad.uv[0].y = mQuad.uv[1].y; mQuad.uv[1].y = ty;
		tx = mQuad.uv[3].x; mQuad.uv[3].x = mQuad.uv[2].x; mQuad.uv[2].x = tx;
		ty = mQuad.uv[3].y; mQuad.uv[3].y = mQuad.uv[2].y; mQuad.uv[2].y = ty;

		mHFlip = !mHFlip;
	}

	if(mVFlip != isVFlip)
	{
		tx = mQuad.uv[0].x; mQuad.uv[0].x = mQuad.uv[3].x; mQuad.uv[3].x = tx;
		ty = mQuad.uv[0].y; mQuad.uv[0].y = mQuad.uv[3].y; mQuad.uv[3].y = ty;
		tx = mQuad.uv[1].x; mQuad.uv[1].x = mQuad.uv[2].x; mQuad.uv[2].x = tx;
		ty = mQuad.uv[1].y; mQuad.uv[1].y = mQuad.uv[2].y; mQuad.uv[2].y = ty;

		mVFlip = !mVFlip;
	}


}

void CSprite::SetHotSpot( float hx, float hy )
{
	mHotX = hx;
	mHotY = hy;
}

void CSprite::SetTexels( float tx, float ty, float tw, float th, bool fixNotPow2Artifacts )
{
	/*if (mQuad.texture == NULL )
	{
		return;
	}*/

	// make offset from boundaries (1,1)
	/*if ( fixNotPow2Artifacts && mQuad.texture != NULL )
	{
		tx += 1.f/mTexWidth;
		ty += 1.f/mTexHeight;
		tw -= 1.f/mTexWidth;
		th -= 1.f/mTexHeight;
	}*/
	mQuad.uv[0] = CIwSVec2(IW_SFIXED_FROM_FLOAT(tx) , IW_SFIXED_FROM_FLOAT(ty));
	mQuad.uv[1] = CIwSVec2(IW_SFIXED_FROM_FLOAT(tx) , IW_SFIXED_FROM_FLOAT(ty + th));
	mQuad.uv[2] = CIwSVec2(IW_SFIXED_FROM_FLOAT(tx + tw) , IW_SFIXED_FROM_FLOAT(ty + th));
	mQuad.uv[3] = CIwSVec2(IW_SFIXED_FROM_FLOAT(tx + tw) , IW_SFIXED_FROM_FLOAT(ty));

}

void CSprite::SetSize( float w, float h )
{
	mHotX = 0;
	mHotY = 0;

	mWidth = w;
	mHeight = h;
}

void CSprite::SetHotSpotC()
{
	mHotX = mWidth / 2;
	mHotY = mHeight / 2;
}

void CSprite::SetBlend( CIwMaterial::AlphaMode mode )
{
	mQuad.blend = mode;
}

void CSprite::Update( float dt )
{

}

float CSprite::GetWidth() const
{
	return mWidth;
}

float CSprite::GetHeight() const
{
	return mHeight;
}

CSprite * CSprite::Clone() const
{
	return new CSprite( *this );
}

bool CSprite::IsHitTest( float x, float y, float rot, float hscale, float vscale )
{
	float tx1, ty1, tx2, ty2; 

	CIwFMat2D m;
	m.SetIdentity();
	m.SetRot( rot );
	m.Normalise();

	m = m.GetInverse();

	tx1 = -mHotX * hscale; 
	ty1 = -mHotY * vscale; 
	tx2 = (mWidth - mHotX) * hscale ; 
	ty2 = (mHeight - mHotY) * vscale; 


	CIwFVec2 p1(tx1, ty1);
	CIwFVec2 p2(tx2, ty2);

	//p1 = m.TransformVec( p1 );
	//p2 = m.TransformVec( p2 );

	CIwFVec2 dv	= CIwFVec2(x, y);
	float	lx = 0, ly = 0;
	lx = m.RotateVecX(dv);
	if ((lx >= p1.x) && (lx <= p2.x))
	{
		ly = m.RotateVecY(dv);
		if ((ly >= p1.y) && (ly <= p2.y))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	/*if (  x < p1.x || x > p2.x || y < p1.y || y > p2.y )
	{
		return false;
	}

	b2AABB*/


	/*if ( x < tx1 || x > tx2 || y < ty1 || y > ty2 )
	{
		return false;
	}*/

	// hit on aabb
	if( !mQuad.texture->GetModifiable() )
	{
		return true;
	}
	
	uint8 * tex = 0;

	tex = mQuad.texture->GetTexels();
	if( tex != NULL )
	{
		x *= mTw / mWidth;
		y *= mTh / mHeight;

		CIwImage& imageData = mQuad.texture->GetImage();

		uint32 w = imageData.GetWidth();
		uint32 h = imageData.GetHeight();
		uint32 texelDepth = imageData.GetByteDepth();
		uint32 pitch = imageData.GetPitch();

		uint32 ox = IW_SFIXED_MUL(mQuad.uv[0].x, w);
		uint32 oy = IW_SFIXED_MUL(mQuad.uv[0].y, h);

		ox += (uint32)x;
		oy += (uint32)y;

		const uint8* pTexel = tex + pitch * oy + texelDepth*ox;
		//uint8 * cc = new uint8[texelDepth];
		//memcpy( cc, pTexel, texelDepth*sizeof(uint8));

		//delete [] cc;

		if (texelDepth == 4)
		{
			uint8 alpha = *(pTexel + 3);
			return alpha > 0x80;
		}
		return true;
	}

	return false;
}

float CSprite::GetTexelWidth() const
{
	return mTw;
}

float CSprite::GetTexelHeight() const
{
	return mTh;
}

void CSprite::GetFlip( bool & isHFlip, bool & isVFlip ) const
{
	isHFlip = mHFlip;
	isVFlip = mVFlip;
}

CIwColour CSprite::GetColor() const
{
	return mQuad.colour[0];
}

// <sprite tex="texname" w="512" h="126" tx="0" ty="386" tw="512" th="126" blend_mode="4" flip_h="0" flip_v="0" />
SpritePtr CSprite::CreateFromXml( TiXmlElement * spriteElem )
{
	//float w = 0.f, h = 0.f;
	//float tx = 0.f, ty = 0.f, tw = 0.f, th = 0.f;
	//
	////float scaleX = 1.f, scaleY = 1.f;
	//int blendMode = (int)CIwMaterial::ALPHA_DEFAULT;
	//int frames = 0;
	//int flip_h = 0;
	//int flip_v = 0;
	//const char * tex_path = 0;
	//float fps = 0.f;
	//int is_loop = 0;
	//int is_pingpong = 0;
	//
	//spriteElem->QueryFloatAttribute("w", &w);
	//spriteElem->QueryFloatAttribute("h", &h);

	//// get source tex coords
	//spriteElem->QueryFloatAttribute("tx", &tx);
	//spriteElem->QueryFloatAttribute("ty", &ty);
	//spriteElem->QueryFloatAttribute("tw", &tw);
	//spriteElem->QueryFloatAttribute("th", &th);

	//// flip
	//spriteElem->QueryIntAttribute("flip_h", &flip_h);
	//spriteElem->QueryIntAttribute("flip_v", &flip_v);

	//// blend
	//spriteElem->QueryIntAttribute("blend_mode", &blendMode);

	//tex_path = spriteElem->Attribute("tex");
	//CIwTexture * tex = NULL;
	//if( tex_path && tex_path[0]) 
	//{
	//	tex = (CIwTexture *)IwGetResManager()->GetResNamed(tex_path, IW_GX_RESTYPE_TEXTURE, IW_RES_SEARCH_ALL_F);
	//}

	//SpritePtr sprt;
	//int isAnimation = 0;
	//spriteElem->QueryIntAttribute("is_animation", &isAnimation);
	//if ( isAnimation == 0 )
	//{
	//	if( tw == 0 || th == 0 )
	//	{
	//		tw = (float)tex->GetWidth();
	//		th = (float)tex->GetHeight();
	//	}

	//	sprt = new CSprite(tex, tx, ty, tw, th);
	//}
	//else
	//{
	//	spriteElem->QueryIntAttribute("frames", &frames);
	//	spriteElem->QueryFloatAttribute("fps", &fps);

	//	CAnimatedSprite * anim = new CAnimatedSprite(tex, frames, tx, ty, tw, th);
	//	anim->SetFps(fps);

	//	spriteElem->QueryIntAttribute("is_loop", &is_loop);
	//	if ( is_loop != 0 )
	//	{
	//		anim->SetFlag(CAnimatedSprite::ANIM_LOOP);
	//	}
	//	spriteElem->QueryIntAttribute("is_pingpong", &is_pingpong);
	//	if ( is_pingpong != 0 )
	//	{
	//		anim->SetFlag(CAnimatedSprite::ANIM_PINGPONG);
	//	}

	//	sprt = anim;
	//}

	//if ( w != 0 && h != 0 )
	//{
	//	sprt->SetSize( w, h );
	//}

	//sprt->SetFlip(flip_h != 0, flip_v != 0);
	//sprt->SetBlend( (CIwMaterial::AlphaMode)blendMode );

	SSpriteParams params;
	LoadParamsXml( spriteElem, params );

	return Create( params );
}

SpritePtr CSprite::Create( const SSpriteParams & def )
{
	if (def.texName.length() ==0)
	{
		return SpritePtr();
	}
	CIwTexture * tex = NULL;
	tex = (CIwTexture *)IwGetResManager()->GetResNamed(def.texName.c_str(), IW_GX_RESTYPE_TEXTURE, IW_RES_SEARCH_ALL_F);
	
	SpritePtr sprt;
	if ( def.isAnimation )
	{
		CAnimatedSprite * anim = new CAnimatedSprite(tex, def.frames, def.tx, def.ty, def.tw, def.th);
		anim->SetFps(def.fps);
		if ( def.isLoop )
		{
			anim->SetFlag(CAnimatedSprite::ANIM_LOOP);
		}
		else if( def.isPingPong )
		{
			anim->SetFlag(CAnimatedSprite::ANIM_PINGPONG);
		}


		sprt = anim;
	}
	else
	{
		float tw = def.tw, th = def.th;
		if(  tw == 0 || th == 0 )
		{
			tw = (float)tex->GetWidth();
			th = (float)tex->GetHeight();
		}
		sprt = new CSprite(tex, def.tx, def.ty, tw, th);
	}

	if ( def.w != 0 && def.h != 0 )
	{
		sprt->SetSize( def.w, def.h );
	}
	sprt->SetFlip(def.isFlipH != 0, def.isFlipV != 0);
	sprt->SetBlend( (CIwMaterial::AlphaMode)def.blendMode );

	return sprt;
}

void CSprite::LoadParamsXml( TiXmlElement * spriteElem, SSpriteParams & params )
{
	spriteElem->QueryFloatAttribute("x", &params.x);
	spriteElem->QueryFloatAttribute("y", &params.y);
	//spriteElem->QueryFloatAttribute("z", &params.z);
	spriteElem->QueryFloatAttribute("w", &params.w);
	spriteElem->QueryFloatAttribute("h", &params.h);

	spriteElem->QueryFloatAttribute("rot", &params.rot);
	params.rot = DEGREE_TO_RADIANCE(params.rot);

	// get source tex coords
	spriteElem->QueryFloatAttribute("tx", &params.tx);
	spriteElem->QueryFloatAttribute("ty", &params.ty);
	spriteElem->QueryFloatAttribute("tw", &params.tw);
	spriteElem->QueryFloatAttribute("th", &params.th);

	// flip
	spriteElem->QueryIntAttribute("flip_h", &params.isFlipH);
	spriteElem->QueryIntAttribute("flip_v", &params.isFlipV);

	// blend
	spriteElem->QueryIntAttribute("blend_mode", &params.blendMode);

	spriteElem->QueryIntAttribute("is_visible", &params.isVisible);

	const char * tex_path = spriteElem->Attribute("tex");
	if( tex_path && tex_path[0]) 
	{
		params.texName = tex_path;
	}

	const char * name = spriteElem->Attribute("name");
	if ( name != 0 )
	{
		params.name = name;
	}
	spriteElem->QueryIntAttribute("is_animation", &params.isAnimation);
	if ( params.isAnimation )
	{
		spriteElem->QueryIntAttribute("frames", &params.frames);
		spriteElem->QueryFloatAttribute("fps", &params.fps);
		spriteElem->QueryIntAttribute("is_loop", &params.isLoop);
		spriteElem->QueryIntAttribute("is_pingpong", &params.isPingPong);
	}

}



/////////////////////////////////////////////////////////////////////

void CAnimatedSprite::Play()
{
	mIsPlaying = true;
	mUpdateTime = 0;
}

void CAnimatedSprite::Stop()
{ 
	mIsPlaying = false; 
}
void CAnimatedSprite::Resume()
{ 
	mIsPlaying = true;
	
}

bool CAnimatedSprite::IsPlaying() const 
{ 
	return mIsPlaying;
}

CAnimatedSprite::CAnimatedSprite()
	: mFlags(0)
{
	mFrames = 0;
	mCurrFrame = 0;
	mTx = 0;
	mTy = 0;
	mTw = 0;
	mTh = 0;
	mAnimSpeed = 1 / 30.f;
	mUpdateTime = 0;
	mIsPlaying = false;
}

CAnimatedSprite::CAnimatedSprite( CIwTexture * tex, int frames, float x, float y, float w, float h )
	: CSprite()
	, mFlags(0)
{
	SetTexture( tex );
	mFrames = frames;
	mCurrFrame = 0;
	mTx = x;
	mTy = y;
	mTw = w;
	mTh = h;
	mAnimSpeed = 1 / 30.f;
	mUpdateTime = 0;
	
	SetSize(w, h);
	SetFrame(mCurrFrame);
	mIsPlaying = false;
}

CAnimatedSprite::CAnimatedSprite( const CAnimatedSprite& anim )
	: CSprite( anim )
{
	mUpdateTime = anim.mUpdateTime;
	mIsPlaying  = anim.mIsPlaying;
	mFrames     = anim.mFrames;
	mCurrFrame  = anim.mCurrFrame;
	mAnimSpeed  = anim.mAnimSpeed;
	/*mTx         = anim.mTx;
	mTy         = anim.mTy;
	mTw         = anim.mTw;
	mTh         = anim.mTh;*/
	mFlags      = anim.mFlags;
}

void CAnimatedSprite::SetFrame( int frame )
{
	IwAssert(GAME, (frame >= 0));
	IwAssert(GAME, (frame < mFrames));

	float tx, ty, tw, th;
	tx = (mTx + mTw*frame) / mTexWidth;
	ty = mTy / mTexHeight;
	tw = mTw / mTexWidth;
	th = mTh / mTexHeight;
	SetTexels(tx, ty, tw, th);

	bool hf = mHFlip, vf = mVFlip;
	mHFlip = false;
	mVFlip = false;
	SetFlip( hf, vf );

	mCurrFrame = frame;
}

void CAnimatedSprite::SetFps( float fps )
{
	mAnimSpeed = 1.f/ fps;
	mUpdateTime = 0;
}

void CAnimatedSprite::Update( float dt )
{
	if (!mIsPlaying)
	{
		return;
	}
	mUpdateTime += dt;
	while (mUpdateTime >= mAnimSpeed)
	{
		mUpdateTime -= mAnimSpeed;

		int delta = 1;
		if (mFlags & ANIM_REV)
		{
			delta = -delta;
		}

		mCurrFrame += delta;

		if (mCurrFrame >= mFrames)
		{
			mCurrFrame = mFrames - 1;
			if (mFlags & ANIM_PINGPONG)
			{
				SetFlag( ANIM_REV );
			}
			else
			if (mFlags & ANIM_LOOP)
			{
				mCurrFrame = 0;
			}
			else
			{
				Stop();
			}
		}
		else
		if (mCurrFrame < 0)
		{
			mCurrFrame = 0;
			if (mFlags & ANIM_PINGPONG)
			{
				ClearFlag( ANIM_REV );
			}
			else
			if (mFlags & ANIM_LOOP)
			{
				mCurrFrame = mFrames - 1;
			}
			else
			{
				Stop();
			}
		}

		SetFrame(mCurrFrame);
	}
}

void CAnimatedSprite::SetFlag( int flag )
{
	if (flag & ANIM_PINGPONG)
	{
		ClearFlag( ANIM_LOOP );
	}
	else
	if (flag & ANIM_LOOP)
	{
		ClearFlag( ANIM_PINGPONG );
	}

	if (flag & ANIM_REV)
	{
		SetFrame(mFrames - 1);
	}

	mFlags |= flag;

}

void CAnimatedSprite::ClearFlag( int flag )
{
	mFlags &= ~flag;
}

CAnimatedSprite * CAnimatedSprite::Clone() const
{
	return new CAnimatedSprite( *this );
}

float CAnimatedSprite::GetFps() const
{
	return 1.f/mAnimSpeed;
}





_END_NAMESPACE_