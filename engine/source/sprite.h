#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "defs.h"
#include "IwColour.h"
#include "IwGeomSVec2.h"
#include "renderer.h"
#include "smartptr.h"
#include "tinyxml.h"

_BEGIN_NAMESPACE_

class CSprite;
typedef shared_ptr<CSprite> SpritePtr;

class SSpriteParams
{
public:
	SSpriteParams()
		: x(0), y(0), w(0), h(0), tx(0), ty(0), tw(0), th(0), rot(0)
		, fps(0), frames(0), blendMode((int)CIwMaterial::ALPHA_DEFAULT)
		, isAnimation(0), isLoop(0), isFlipH(0), isFlipV(0), isVisible(1)
		, isPingPong(0)
	{}

	CIwStringL texName;
	float      x;
	float      y;
	float      w;
	float      h;
	float      tx;
	float      ty;
	float      tw;
	float      th;
	float      rot;
	float      fps;
	int        frames;
	int        blendMode;
	int        isAnimation;
	int        isLoop;
	int        isPingPong;
	int        isFlipV;
	int        isFlipH;
	int        isVisible;
	CIwStringS name;
};


class CSprite : public ref_counter
{
public:
	CSprite( const CSprite& s);
	CSprite( CIwTexture * tex, float tx, float ty, float tw, float th );
	virtual ~CSprite();

	virtual void      Update(float dt);
	virtual CSprite * Clone() const;
	void      SetTexture(CIwTexture * tex);
	void      SetSize(float w, float h);
	void      RenderEx( float x, float y, float rot = 0, float hscale = 1, float vscale = 1 );
	void      SetFlip( bool isHFlip, bool isVFlip );
	void      GetFlip( bool & isHFlip, bool & isVFlip ) const;
	void      SetHotSpot( float hx, float hy );
	void      SetHotSpotC();
	void      SetBlend(CIwMaterial::AlphaMode mode);
	float     GetWidth() const;
	float     GetHeight() const;
	float     GetTexelWidth() const;
	float     GetTexelHeight() const;
	void      SetColor( CIwColour c );
	CIwColour GetColor() const;
	bool      IsHitTest( float x, float y, float rot = 0, float hscale = 1, float vscale = 1 );

	static void      LoadParamsXml( TiXmlElement * sprt_elem,  SSpriteParams & params );
	static SpritePtr CreateFromXml( TiXmlElement * sprt_elem );
	static SpritePtr Create( const SSpriteParams & def );

protected:
	CSprite();
	// set in 0..1
	void    SetTexels(float tx, float ty, float tw, float th, bool fixNotPow2Artifacts = true);

protected:
	SQuad  mQuad;
	float  mTx;
	float  mTy;
	float  mTh;
	float  mTw;
	float  mWidth;
	float  mHeight;
	float  mTexWidth;
	float  mTexHeight;
	float  mHotX;
	float  mHotY;
	bool   mHFlip;
	bool   mVFlip;


};


class CAnimatedSprite : public CSprite
{
public:
	enum EAnimFlags
	{
		ANIM_REV        = 0x1,
		ANIM_LOOP       = 0x2,
		ANIM_PINGPONG   = 0x4
	};
	CAnimatedSprite( const CAnimatedSprite& anim );
	CAnimatedSprite(CIwTexture * tex, int frames, float tx, float ty, float tw, float th);
	
	virtual CAnimatedSprite * Clone() const;
	virtual void              Update(float dt);

	void        SetFps(float fps);
	void		Play();
	void		Stop();
	void		Resume();
	bool		IsPlaying() const;
	void        SetFlag(int flag);
	void        ClearFlag(int flag);
	float       GetFps() const;

protected:
	CAnimatedSprite();

	void        SetFrame(int frame);
	
private:
	float       mUpdateTime;
	bool		mIsPlaying;
	int         mFrames;
	int         mCurrFrame;
	float       mAnimSpeed;
	/*float       mTx;
	float       mTy;
	float       mTw;
	float       mTh;*/
	int         mFlags;

};

typedef shared_ptr<CAnimatedSprite> AnimatedSpritePtr;

_END_NAMESPACE_

#endif // _SPRITE_H_