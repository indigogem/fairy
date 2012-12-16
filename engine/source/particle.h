#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "defs.h"
#include "game_object.h"

_BEGIN_NAMESPACE_

class CParticleSystem : public CGameObject
{
public:
	GAME_CLASS_TYPENAME(CParticleSystem)

	CParticleSystem(CLevel * level);
	CParticleSystem( const CParticleSystem & ps);
	virtual ~CParticleSystem();

	virtual void Init();
	virtual void Load(TiXmlElement * objElem);
#if APP_SAVE_XML	
	virtual void Save(TiXmlElement * root);
#endif // APP_SAVE_XML
	virtual void Update(float dt);
	virtual void Draw();
	virtual void SetVisible( bool isVisible );

	virtual CParticleSystem * Clone() const;
	virtual void SetPos( CIwFVec2 pos );
	virtual bool IsHitTest( int32 x, int32 y );

	void         SetParticlesTrail(bool isTrail);
	void         SetEmission( int ems );
	int          GetEmission() const;
	void         SetAge(float age);
	bool         AllParticlesDead() const;


	class SParticleDef : public CGameObject::SSpriteDef
	{
	public:
		// SSpriteDef

		//SpritePtr sprt;
		//CIwFVec2  pos;
		//float     rot;
		//bool      isVisible;

		CIwFVec2  vel;
		float	  radialAccel;
		float	  tangentialAccel;
		float     gravity;

		float	  spin;
		float	  spinDelta;

		float	  size;
		float	  sizeDelta;

		CIwColour colColor;		// + alpha
		CIwColour colColorDelta;

		float	  age;
		float	  terminalAge;
	};

	typedef shared_ptr<SParticleDef> ParticleDefPtr;

protected:

	static const int  MAX_PARTICLES = 100;

	struct SParticleSystemeDef
	{
		int			emission; // particles per sec
		float		lifetime;

		float		particleLifeMin;
		float		particleLifeMax;

		float		direction;
		float		spread;
		bool		relative;	

		float		speedMin;
		float		speedMax;

		float		gravityMin;
		float		gravityMax;

		float		radialAccelMin;
		float		radialAccelMax;

		float		tangentialAccelMin;
		float		tangentialAccelMax;

		float		sizeStart;
		float		sizeEnd;
		float		sizeVar;

		float		spinStart;
		float		spinEnd;
		float		spinVar;

		CIwColour	colColorStart; // + alpha
		CIwColour	colColorEnd;
		float		colorVar;
		float		alphaVar;
	};
protected:
	
	float           RandFloatMinMax(float min, float max);

protected:
	SParticleSystemeDef   mPsiDef;
	float                 mAge;
	float				  mEmissionResidue;
	int                   mParticalesNum;
	CIwFVec2              mPrevPos;
	bool                  mIsParticlesTrail;
	SpritesContainer      mParticles;

};

typedef shared_ptr<CParticleSystem> ParticleSystemPtr;


_END_NAMESPACE_
#endif // _PARTICLE_H_