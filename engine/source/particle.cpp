#include "particle.h"
#include "math.h"

_BEGIN_NAMESPACE_

CParticleSystem::CParticleSystem( CLevel * level )
	: CGameObject( level )
	, mAge( -1.f )
	, mParticalesNum( 0 )
	, mEmissionResidue( 0.f )
	, mIsParticlesTrail( true )
{
	
	mPsiDef.emission = 0;
	mPsiDef.lifetime = 0;

	mPsiDef.particleLifeMin = 0;
	mPsiDef.particleLifeMax = 0;

	mPsiDef.direction = 0;
	mPsiDef.spread = 0;
	mPsiDef.relative = false;	

	mPsiDef.speedMin = 0;
	mPsiDef.speedMax = 0;

	mPsiDef.gravityMin = 0;
	mPsiDef.gravityMax = 0;

	mPsiDef.radialAccelMin = 0;
	mPsiDef.radialAccelMax = 0;

	mPsiDef.tangentialAccelMin = 0;
	mPsiDef.tangentialAccelMax = 0;

	mPsiDef.sizeStart = 0;
	mPsiDef.sizeEnd = 0;
	mPsiDef.sizeVar = 0;

	mPsiDef.spinStart = 0;
	mPsiDef.spinEnd = 0;
	mPsiDef.spinVar = 0;

	uint32 c = 0xFFFFFFFF;
	mPsiDef.colColorStart.Set(c); 
	mPsiDef.colColorEnd.Set(c);
	mPsiDef.colorVar = 0;
	mPsiDef.alphaVar = 0;	

}

CParticleSystem::CParticleSystem( const CParticleSystem & ps )
	//: CGameObject( *dynamic_cast<const CGameObject*>(&ps) )
	: CGameObject( ps )
{
	mAge = ps.mAge;
	mParticalesNum = ps.mParticalesNum;
	mEmissionResidue = ps.mEmissionResidue;
	mIsParticlesTrail = ps.mIsParticlesTrail;

	mPsiDef = ps.mPsiDef;

	Init();
	UpdateTransform();
}

CParticleSystem::~CParticleSystem()
{

}

void CParticleSystem::Init()
{
	if ( mSprites.empty() )
	{
		IwAssertMsg(GAME, false, ("CParticleSystem::Init no sprite"));
	}

	SpriteDefPtr df = mSprites[0];	

	df->isVisible = false;
	//mSprites.clear();

	mParticles.clear();
	mParticles.resize( MAX_PARTICLES );

	for ( int i = 0; i < MAX_PARTICLES; i++ )
	{
		ParticleDefPtr pd = new SParticleDef();
		pd->sprt = df->sprt->Clone();
		pd->sprt->SetHotSpotC();
		pd->isVisible = false;
#if APP_SAVE_XML
		pd->initParams = df->initParams;
#endif // APP_SAVE_XML
		mParticles[i] = pd;
	}

}

void CParticleSystem::Load( TiXmlElement * objElem )
{
	CGameObject::Load( objElem );

	TiXmlElement * psiElem = objElem->FirstChildElement("psi");

	if ( psiElem != NULL )
	{
		psiElem->QueryIntAttribute("emission", &mPsiDef.emission);
		psiElem->QueryFloatAttribute("lifetime", &mPsiDef.lifetime);

		psiElem->QueryFloatAttribute("particleLifeMin", &mPsiDef.particleLifeMin);
		psiElem->QueryFloatAttribute("particleLifeMax", &mPsiDef.particleLifeMax);

		psiElem->QueryFloatAttribute("direction", &mPsiDef.direction);
		psiElem->QueryFloatAttribute("spread", &mPsiDef.spread);

		int relative = 0;
		psiElem->QueryIntAttribute("relative", &relative);
		mPsiDef.relative = (relative != 0);

		psiElem->QueryFloatAttribute("speedMin", &mPsiDef.speedMin);
		psiElem->QueryFloatAttribute("speedMax", &mPsiDef.speedMax);

		psiElem->QueryFloatAttribute("gravityMin", &mPsiDef.gravityMin);
		psiElem->QueryFloatAttribute("gravityMax", &mPsiDef.gravityMax);

		psiElem->QueryFloatAttribute("radialAccelMin", &mPsiDef.radialAccelMin);
		psiElem->QueryFloatAttribute("radialAccelMax", &mPsiDef.radialAccelMax);

		psiElem->QueryFloatAttribute("tangentialAccelMin", &mPsiDef.tangentialAccelMin);
		psiElem->QueryFloatAttribute("tangentialAccelMin", &mPsiDef.tangentialAccelMin);

		psiElem->QueryFloatAttribute("sizeStart", &mPsiDef.sizeStart);
		psiElem->QueryFloatAttribute("sizeEnd", &mPsiDef.sizeEnd);
		psiElem->QueryFloatAttribute("sizeVar", &mPsiDef.sizeVar);

		psiElem->QueryFloatAttribute("spinStart", &mPsiDef.spinStart);
		psiElem->QueryFloatAttribute("spinEnd", &mPsiDef.spinEnd);
		psiElem->QueryFloatAttribute("spinVar", &mPsiDef.spinVar);


		const char * color = psiElem->Attribute("colColorStart");
		if ( color )
		{
			int cc = 0;
			sscanf(color, "%X", &cc);

			uint8 b = cc & 0xFF;
			uint8 g = (cc >> 8) & 0xFF;
			uint8 r = (cc >> 16) & 0xFF;
			uint8 a = (cc >> 24) & 0xFF;

			mPsiDef.colColorStart.Set(r, g, b, a);
		}

		color = psiElem->Attribute("colColorEnd");
		if ( color )
		{
			int cc = 0;
			sscanf(color, "%X", &cc);

			uint8 b = cc & 0xFF;
			uint8 g = (cc >> 8) & 0xFF;
			uint8 r = (cc >> 16) & 0xFF;
			uint8 a = (cc >> 24) & 0xFF;

			mPsiDef.colColorEnd.Set(r, g, b, a);
		}	

		psiElem->QueryFloatAttribute("colorVar", &mPsiDef.colorVar);
		psiElem->QueryFloatAttribute("alphaVar", &mPsiDef.alphaVar);
	}
}

#if APP_SAVE_XML
void CParticleSystem::Save( TiXmlElement * obj )
{
	CGameObject::Save( obj );

	TiXmlElement * psiElem = new TiXmlElement( "psi" );

	psiElem->SetAttribute("emission", mPsiDef.emission);
	psiElem->SetDoubleAttribute("lifetime", mPsiDef.lifetime);

	psiElem->SetDoubleAttribute("particleLifeMin", mPsiDef.particleLifeMin);
	psiElem->SetDoubleAttribute("particleLifeMax", mPsiDef.particleLifeMax);

	psiElem->SetDoubleAttribute("direction", mPsiDef.direction);
	psiElem->SetDoubleAttribute("spread", mPsiDef.spread);
	
	psiElem->SetAttribute("relative", (mPsiDef.relative?1:0) );	

	psiElem->SetDoubleAttribute("speedMin", mPsiDef.speedMin);
	psiElem->SetDoubleAttribute("speedMax", mPsiDef.speedMax);

	psiElem->SetDoubleAttribute("gravityMin", mPsiDef.gravityMin);
	psiElem->SetDoubleAttribute("gravityMax", mPsiDef.gravityMax);

	psiElem->SetDoubleAttribute("radialAccelMin", mPsiDef.radialAccelMin);
	psiElem->SetDoubleAttribute("radialAccelMax", mPsiDef.radialAccelMax);

	psiElem->SetDoubleAttribute("tangentialAccelMin", mPsiDef.tangentialAccelMin);
	psiElem->SetDoubleAttribute("tangentialAccelMin", mPsiDef.tangentialAccelMin);

	psiElem->SetDoubleAttribute("sizeStart", mPsiDef.sizeStart);
	psiElem->SetDoubleAttribute("sizeEnd", mPsiDef.sizeEnd);
	psiElem->SetDoubleAttribute("sizeVar", mPsiDef.sizeVar);

	psiElem->SetDoubleAttribute("spinStart", mPsiDef.spinStart);
	psiElem->SetDoubleAttribute("spinEnd", mPsiDef.spinEnd);
	psiElem->SetDoubleAttribute("spinVar", mPsiDef.spinVar);

	char buff[128] = {0};

	sprintf(buff, "0x%02X%02X%02X%02X", mPsiDef.colColorStart.a, mPsiDef.colColorStart.b, mPsiDef.colColorStart.g, mPsiDef.colColorStart.r);
	psiElem->SetAttribute("colColorStart", buff);

	sprintf(buff, "0x%02X%02X%02X%02X", mPsiDef.colColorEnd.a, mPsiDef.colColorEnd.b, mPsiDef.colColorEnd.g, mPsiDef.colColorEnd.r);
	psiElem->SetAttribute("colColorEnd", buff);

	psiElem->SetDoubleAttribute("colorVar", mPsiDef.colorVar);
	psiElem->SetDoubleAttribute("alphaVar", mPsiDef.alphaVar);

	obj->LinkEndChild(psiElem);

}
#endif // APP_SAVE_XML

void CParticleSystem::Update(float dt)
{
	if ( mAge == -2.f && AllParticlesDead() )
	{
		Kill();
	}

	if ( IsDead() || mIsPaused )
	{
		return;
	}

	CGameObject::Update( dt );

	

	for ( size_t i = 0; i < mParticles.size(); i++ )
	{
		if ( mParticles[i]->sprt != NULL && mParticles[i]->isVisible )
		{
			mParticles[i]->sprt->Update( dt );
		}
	}

	if ( mAge >= 0 )
	{
		mAge += dt;
		if ( mAge >= mPsiDef.lifetime )
		{
			mAge = -2.f;
		}
	}

	// update curr particles
	
	ParticleDefPtr prt;
	CIwFVec2 vecAccel, vecAccel2;
	float ang;

	for ( int i = 0; i < MAX_PARTICLES; i++ )
	{		
		if ( mParticles[i]->isVisible )
		{
			prt = shared_ptr_scast<SParticleDef, SSpriteDef>( mParticles[i] );

			prt->age += dt;
		}
		else
		{
			continue;
		}

		if(prt->age >= prt->terminalAge)
		{
			mParticalesNum--;
			prt->isVisible = false;			
			continue;
		}

		vecAccel = prt->pos - mPos;
		vecAccel.Normalise();
		vecAccel2 = vecAccel;
		vecAccel *= prt->radialAccel;

		ang = vecAccel2.x;
		vecAccel2.x = -vecAccel2.y;
		vecAccel2.y = ang;

		vecAccel2 *= prt->tangentialAccel;
		prt->vel += ( vecAccel + vecAccel2 ) * dt;
		prt->vel.y += prt->gravity * dt;

		prt->pos += prt->vel * dt;

		prt->spin += prt->spinDelta * dt;
		prt->size += prt->sizeDelta * dt;
		prt->colColor += prt->colColorDelta * IW_SFIXED_FROM_FLOAT(dt);

		//prt->colColor += prt->colColorDelta;
	}


	// generate some particles

	if ( mAge != -2.f )
	{
		float particlesNeeded = mPsiDef.emission * dt + mEmissionResidue;
		uint16 particlesCreated = (uint16)particlesNeeded;
		mEmissionResidue = particlesNeeded - particlesCreated;

		for ( int i = 0; i < MAX_PARTICLES && particlesCreated > 0; i++ )
		{
			if ( mParticalesNum >= MAX_PARTICLES )
			{
				break;
			}

			if ( mParticles[i]->isVisible )
			{
				continue;
			}
			prt = shared_ptr_scast<SParticleDef, SSpriteDef>( mParticles[i] );

			prt->age = 0.0f;
			prt->terminalAge = RandFloatMinMax(mPsiDef.particleLifeMin, mPsiDef.particleLifeMax);

			prt->pos = mPrevPos + ( mPos - mPrevPos ) * RandFloatMinMax( 0, 1 );
			prt->pos.x += RandFloatMinMax(-10.0f, 10.0f);
			prt->pos.y += RandFloatMinMax(-10.0f, 10.0f);


			ang =   mPsiDef.direction - (float)M_PI_2 
				  + RandFloatMinMax( 0, mPsiDef.spread ) 
				  - mPsiDef.spread / 2.0f;

			if ( mPsiDef.relative ) 
			{
				CIwFVec2 v = mPrevPos - mPos;
				float a = atan2f( v.y, v.x );
				ang += a + (float)M_PI_2;

			}

			prt->vel.x = cosf(ang);
			prt->vel.y = sinf(ang);
			prt->vel *= RandFloatMinMax(mPsiDef.speedMin, mPsiDef.speedMax);

			prt->gravity = RandFloatMinMax(mPsiDef.gravityMin, mPsiDef.gravityMax);
			prt->radialAccel = RandFloatMinMax(mPsiDef.radialAccelMin, mPsiDef.radialAccelMax);
			prt->tangentialAccel = RandFloatMinMax(mPsiDef.tangentialAccelMin, mPsiDef.tangentialAccelMax);

			prt->size = RandFloatMinMax( mPsiDef.sizeStart, mPsiDef.sizeStart + ( mPsiDef.sizeEnd - mPsiDef.sizeStart ) * mPsiDef.sizeVar );
			prt->sizeDelta = ( mPsiDef.sizeEnd - prt->size ) / prt->terminalAge;

			prt->spin = RandFloatMinMax( mPsiDef.spinStart, mPsiDef.spinStart + ( mPsiDef.spinEnd - mPsiDef.spinStart ) * mPsiDef.spinVar);
			prt->spinDelta = (mPsiDef.spinEnd - prt->spin) / prt->terminalAge;


			prt->colColor.r = (uint8)RandFloatMinMax(mPsiDef.colColorStart.r, mPsiDef.colColorStart.r + (mPsiDef.colColorEnd.r - mPsiDef.colColorStart.r) * mPsiDef.colorVar);
			prt->colColor.g = (uint8)RandFloatMinMax(mPsiDef.colColorStart.g, mPsiDef.colColorStart.g + (mPsiDef.colColorEnd.g - mPsiDef.colColorStart.g) * mPsiDef.colorVar);
			prt->colColor.b = (uint8)RandFloatMinMax(mPsiDef.colColorStart.b, mPsiDef.colColorStart.b + (mPsiDef.colColorEnd.b - mPsiDef.colColorStart.b) * mPsiDef.colorVar);
			prt->colColor.a = (uint8)RandFloatMinMax(mPsiDef.colColorStart.a, mPsiDef.colColorStart.a + (mPsiDef.colColorEnd.a - mPsiDef.colColorStart.a) * mPsiDef.alphaVar);

			prt->colColorDelta.r = (uint8)((mPsiDef.colColorEnd.r - prt->colColor.r) / prt->terminalAge);
			prt->colColorDelta.g = (uint8)((mPsiDef.colColorEnd.g - prt->colColor.g) / prt->terminalAge);
			prt->colColorDelta.b = (uint8)((mPsiDef.colColorEnd.b - prt->colColor.b) / prt->terminalAge);
			prt->colColorDelta.a = (uint8)((mPsiDef.colColorEnd.a - prt->colColor.a) / prt->terminalAge);

			prt->isVisible = true;
			particlesCreated--;
			mParticalesNum++;
		}
	}

	mPrevPos = mPos;
}

void CParticleSystem::Draw()
{
	if ( !mIsVisible )
	{
		return;
	}

	for ( size_t i = 0; i < mParticles.size(); i++ )
	{
		if ( mParticles[i]->isVisible )
		{
			CIwFVec2 pos = mParticles[i]->pos;

			//pos = mCacheTransform.TransformVec( pos );

			if ( mParticles[i]->sprt != NULL )
			{
				ParticleDefPtr prt;
				prt = shared_ptr_scast<SParticleDef, SSpriteDef>( mParticles[i] );

				mParticles[i]->sprt->SetColor( prt->colColor );
				mParticles[i]->sprt->RenderEx( pos.x, pos.y, prt->spin*prt->age, prt->size, prt->size );
			}
		}	
	}
}

float CParticleSystem::RandFloatMinMax(float min, float max)
{
	return IwRandMinMax(int(min*1000), int(max*1000)) / 1000.f;
}

CParticleSystem * CParticleSystem::Clone() const
{
	//IwAssertMsg(GANE, false, ("CParticleSystem::Clone not impl"));
	return new CParticleSystem( *this );
}

void CParticleSystem::SetPos( CIwFVec2 pos )
{
	if ( !mIsParticlesTrail )
	{
		CIwFVec2 dl;
		dl = pos - mPos;

		for ( int i = 0; i < MAX_PARTICLES; i++ )
		{
			if ( mParticles[i]->isVisible )
			{
				mParticles[i]->pos += dl;
			}
		}

		mPrevPos += dl;
	}
	else
	{
		if ( mAge==-2.0 )
		{ 
			mPrevPos = pos;
		}
		else 
		{ 
			mPrevPos = mPos;
		}
	}

	CGameObject::SetPos( pos );
}

void CParticleSystem::SetParticlesTrail(bool isTrail)
{
	mIsParticlesTrail = isTrail;
}

bool CParticleSystem::IsHitTest( int32 x, int32 y )
{
	// particles didnt collide
	return false;
}

void CParticleSystem::SetEmission( int ems )
{
	mPsiDef.emission = ems;
}

int CParticleSystem::GetEmission() const
{
	return mPsiDef.emission;
}

void CParticleSystem::SetAge( float age )
{
	mAge = age;
}

bool CParticleSystem::AllParticlesDead() const
{

	for ( int i = 0; i < MAX_PARTICLES; i++ )
	{		
		if ( mParticles[i]->isVisible )
		{
			return false;
		}
	}

	return true;
}

void CParticleSystem::SetVisible( bool isVisible )
{
	CGameObject::SetVisible( isVisible );

	if ( mParticles.size() > 0 )
	{
		for ( int i = 0; i < MAX_PARTICLES; i++ )
		{	
			mParticles[i]->isVisible = false;
		}
	}
}


_END_NAMESPACE_