#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "defs.h"
#include "IwGxTypes.h"
#include "IwColour.h"
#include "IwMaterial.h"
#include "IwGxFont.h"
#include "list"

_BEGIN_NAMESPACE_


const uint32 VIRTUAL_WIDTH  = 960;
const uint32 VIRTUAL_HEIGHT = 640;
const uint32 RENDER_MAX_BUFF_SIZE = 480;
const uint32 RENDER_MIN_BUFF_SIZE = 4;

#define IW_SUBPIXEL_FROM_FLOAT(x) ((int16)((x) * (float)(1<<3))) 
#define FLOAT_TO_FIXED(a) (iwsfixed)(a * (float)(1 << 3))

typedef	void (*TResetParamsCallback)();
typedef std::list<CIwMaterial*> MaterialContainer;

struct SDeviceParam
{
	uint32	          screenWidth;
	uint32	          screenHeight;
	CIwColour	      bkgColor;
	bool         	  isClearBackground;				
	float			  screenFactor;
	uint32	          virtualWidth;
	uint32	          virtualHeight;
	IwGxScreenOrient  screenOrient;
	float             scale;
	int32             offsetX;
	int32             offsetY;


};

struct SQuad
{
	CIwSVec2               v[4];
	CIwSVec2               uv[4];
	CIwColour              colour[4];
	CIwTexture *           texture;
	CIwMaterial::AlphaMode blend;
};

class CCamera2D
{
public:
	CCamera2D() : mScale(1.f) { mPos = CIwFVec2(0,0); }
	void     Reset();
	CIwFVec2 GetPos() const;
	void     SetPos( const CIwFVec2 & pos );
	void     SetScale( float scale );
	float    GetScale() const;
	void     Move( CIwFVec2 dr );
	CIwFVec2 Translate( const CIwFVec2 & p ) const;

private:
	CIwFVec2 mPos;
	float    mScale;	
};

inline CIwFVec2 CCamera2D::Translate( const CIwFVec2 & p ) const
{ 
	return mPos + p/mScale; 
}

inline void CCamera2D::Move( CIwFVec2 dr )
{ 
	mPos += dr; 
}

inline float CCamera2D::GetScale() const 
{ 
	return mScale; 
}

inline void CCamera2D::SetScale( float scale )
{ 
	mScale = scale; 
}


inline CIwFVec2 CCamera2D::GetPos() const 
{
	return mPos; 
}


class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
	void Init();

	void Begin();
	void End();
	void DefaultMaterial();
	CIwMaterial * GetDefaultMaterial() const;
	void ResetDeviceParams();
	void UpdateDeviceParams();
	
	void Draw( const SQuad & q );
	void Draw( IwGxPrimType type, const CIwSVec2 * vertex, const CIwSVec2 * uv, const uint16 * indexStream, const CIwColour * colour, int32 vertexCount, CIwMaterial * mat );
	void DrawRect( float x, float y, float w, float h, const CIwColour & color );
	void SetOnResetParamsCallback( TResetParamsCallback c );

	const SDeviceParam & GetDeviceParam() const;
	void  CameraTransform( CIwSVec2 * v, int num );

	CCamera2D & GetCamera();
	void      SetCamera( const CCamera2D& cam );

	// rect pos in virtual coords
	void  RenderText( CIwGxFont * font, const CIwRect & rect, const CIwColour & color, const CIwStringS & txt, IwGxFontAlignHor hAlign, IwGxFontAlignVer vAlign );

	void               Flush();
public:
	static CRenderer &  GetInst();
	static void        Destroy();

	static int32 ScreenSizeChangeCallback(void* systemData, void* userData);


private:
	static CRenderer*   mInstance;


	//void               DrawPrimScreenSubSpace( IwGxPrimType type, CIwSVec2 * vertex, uint16* indexStream, CIwColour * colour, int32 vertexCount );
	CIwMaterial *      GetMtlCached(CIwTexture * tex, CIwMaterial::AlphaMode blend);
	void               ClearMtlCache();
	//void               ResetVertexBuffers( bool isMax );
	void               AllocBuffers();
	void               FreeBuffers();
	void               ResetBuffers();
	void               AddBuffer(IwGxPrimType type, const CIwSVec2 * vertex, const CIwSVec2 * uv, const uint16* indexStream, const CIwColour * colour, int32 vertexCount);
	void               SetBufferType(IwGxPrimType type);
	IwGxPrimType       GetBufferType() const;
	
	uint32             GetVBSize() const;
	uint32             GetUVBSize() const;
	uint32             GetCBSize() const;
	uint32             GetIBSize() const;
	void               DrawBatch();
	void               DrawDirect(IwGxPrimType type, CIwSVec2 * vertex, CIwSVec2 * uv, uint16* indexStream, CIwColour * colour, int32 vertexCount, CIwMaterial * mat);
	
protected:
	SDeviceParam         mDeviceParams;
	CIwMaterial *        mDefaultMaterial;
	bool                 mIsResetDeviceparam;
	TResetParamsCallback mOnResetparams;

	MaterialContainer    mMtlCache;
	CIwSVec2*            mVB;
	CIwSVec2*            mUVB;
	CIwColour*           mCB;
	uint16*              mIB;
	IwGxPrimType         mBuffType;
	CIwMaterial *        mCurrMtl;
	uint32               mVBSize;
	uint32               mCBSize;
	uint32               mUVBSize;
	uint32               mIBSize;
	CCamera2D            mCamera;
	

};


_END_NAMESPACE_
#endif //_RENDERER_H_
