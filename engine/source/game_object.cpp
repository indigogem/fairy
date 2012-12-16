#include "game_object.h"
#include "IwResManager.h"
#include "level.h"

_BEGIN_NAMESPACE_

ObjectId CGameObject::mIds = 0;

CGameObject::CGameObject(CLevel * level)
	: mIsDead(false)
	, mIsActive(true)
	, mLevel( level )
{
	mId = GetUniqueId();
	mRot = 0;
	mIsPaused = false;
	mIsDisappearing = false;
	mDisappearingTime = 0;
	mDisappearingInitTime = 0;
	mIsVisible = true;

#if APP_SAVE_XML
	mIsSave = false;
#endif
}

CGameObject::CGameObject( const CGameObject& obj )
{
#if APP_SAVE_XML
	mIsSave = obj.mIsSave;
#endif
	mId = GetUniqueId();
	mDisappearingTime = 0;
	mDisappearingInitTime = 0;
	mIsDisappearing = false;
	mRot = 0;

	mIsDead    = obj.mIsDead; // ??
	mIsActive  = obj.mIsActive;
	mIsVisible = obj.mIsVisible;
	mPos       = obj.mPos;
	mZ         = obj.mZ;
	mName      = obj.mName;
	mLevel     = obj.mLevel;
	mRot       = obj.mRot;
	mIsPaused  = obj.mIsPaused;
	mTemplateName = obj.mTemplateName;

	mSprites.clear();
	for (size_t i = 0; i < obj.mSprites.size(); i++ )
	{
		mSprites.push_back( obj.mSprites[i]->Clone() );
	}
	
	UpdateTransform();

}

CGameObject::~CGameObject()
{

}

void CGameObject::Draw()
{
	if ( !mIsVisible )
	{
		return;
	}

	for ( size_t i = 0; i < mSprites.size(); i++ )
	{
		if ( mSprites[i]->isVisible )
		{
			CIwFVec2 pos = mSprites[i]->pos;

			pos = mCacheTransform.TransformVec( pos );

			if ( mSprites[i]->sprt != NULL )
			{
				mSprites[i]->sprt->RenderEx( pos.x, pos.y, mRot + mSprites[i]->rot );
			}
		}	
	}
}

void CGameObject::Update( float dt )
{
	if ( IsDead() || mIsPaused )
	{
		return;
	}

	if ( mIsDisappearing )
	{
		mDisappearingTime -= dt;
		if ( mDisappearingTime < 0 )
		{
			mIsDisappearing = false;
			mDisappearingTime = 0;
			SetAlpha(0);
			Kill();
		}
		else
		{
			SetAlpha( mDisappearingTime / mDisappearingInitTime );
		}
	}


	for ( size_t i = 0; i < mSprites.size(); i++ )
	{
		if ( mSprites[i]->sprt != NULL && mSprites[i]->isVisible )
		{
			mSprites[i]->sprt->Update( dt );
		}
	}
}

bool CGameObject::IsDead() const
{
	return mIsDead;
}

void CGameObject::Init()
{

}

void CGameObject::Deinit()
{

}

void CGameObject::Load( TiXmlElement * objElem )
{
	if ( objElem == NULL )
	{
		return;
	}

	const char * name = objElem->Attribute("name");
	if( name )
	{
		mName = name;
	}

	int isActive = 1;
	objElem->QueryIntAttribute("is_active", &isActive);
	SetActive( isActive != 0 );

	int isVisible = 1;
	objElem->QueryIntAttribute("is_visible", &isVisible);
	SetVisible( isVisible != 0 );

	float x = 0.f, y = 0.f, z = 0.f;
	float rot = 0;

	objElem->QueryFloatAttribute("x", &x);
	objElem->QueryFloatAttribute("y", &y);
	objElem->QueryFloatAttribute("z", &z);

	objElem->QueryFloatAttribute("rot", &rot);

	mPos.x = x;
	mPos.y = y;
	mZ = z;
	mRot = DEGREE_TO_RADIANCE( rot ) ;


	// we ignore other params if we created from template
	const char * tmpl = objElem->Attribute("template");
	if ( tmpl != NULL && tmpl[0] != 0 )
	{
		mTemplateName = tmpl;
		SetRot( mRot );   // we reset pos to phys body
		UpdateTransform(); 
		return;
	}

	mSprites.clear();
	TiXmlElement * sprites = objElem->FirstChildElement("sprites");
	if( sprites )
	{
		TiXmlElement * spriteElem = sprites->FirstChildElement("sprite");

		for(; spriteElem; spriteElem = spriteElem->NextSiblingElement("sprite"))
		{
			//float x = 0.f, y = 0.f, z = 0.f, w = 0.f, h = 0.f;
			//float tx = 0.f, ty = 0.f, tw = 0.f, th = 0.f;
			//float rot = 0.f;
			////float scaleX = 1.f, scaleY = 1.f;
			//int blendMode = (int)CIwMaterial::ALPHA_DEFAULT;
			//int frames = 0;
			//int flip_h = 0;
			//int flip_v = 0;
			//const char * tex_path = 0;
			//float fps = 0.f;
			//int is_loop = 0;
			//int is_pingpong = 0;
			//CIwStringS name;
			//int isVisible = 1;

			//const char * ss = spriteElem->Attribute("name");
			//if ( ss != 0 )
			//{
			//	name = ss;
			//}
	
			//// coords
			//spriteElem->QueryFloatAttribute("x", &x);
			//spriteElem->QueryFloatAttribute("y", &y);
			//spriteElem->QueryFloatAttribute("z", &z);
			//spriteElem->QueryFloatAttribute("w", &w);
			//spriteElem->QueryFloatAttribute("h", &h);

			//spriteElem->QueryFloatAttribute("rot", &rot);

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

			//spriteElem->QueryIntAttribute("is_visible", &isVisible);


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
			//
			//if ( w != 0 && h != 0 )
			//{
			//	sprt->SetSize( w, h );
			//}

			//sprt->SetFlip(flip_h != 0, flip_v != 0);
			////sprt->SetHotSpot( x, y );
			//sprt->SetBlend( (CIwMaterial::AlphaMode)blendMode );

			SSpriteParams params;
			CSprite::LoadParamsXml(spriteElem, params);
			//AddSprite( params );
			SpritePtr sprt = CSprite::Create( params );

			///*SpriteDefPtr def = new SSpriteDef;
			//def->isVisible = (isVisible != 0);
			//def->pos = CIwFVec2(x, y);
			//def->rot = rot;
			//def->sprt = sprt;
			//def->name = name;*/

			SpriteDefPtr def = new SSpriteDef;
			def->isVisible = (params.isVisible != 0);
			def->pos       = CIwFVec2(params.x, params.y);
			def->rot       = params.rot;
			def->name      = params.name;
			def->sprt      = sprt;
			

			mSprites.push_back( def );
			sprt= NULL;


			// save params only for editor!
#if APP_SAVE_XML
	
			/*SSpriteParams params;
			params.texName = tex_path;
			params.x = x;
			params.y = y;
			params.w = w;
			params.h = h;
			params.tx = tx;
			params.ty = ty;
			params.tw = tw;
			params.th = th;
			params.rot = rot;
			params.fps = fps;
			params.frames = frames;
			params.blendMode = blendMode;
			params.isAnimation = isAnimation;
			params.isLoop = is_loop;
			params.isFlipV = flip_v;
			params.isFlipH = flip_h;
			params.name    = name;
			params.isVisible = isVisible;*/

			// (!)
			def->initParams = params;			
#endif

		}
	}


	UpdateTransform();

	TiXmlElement * params = objElem->FirstChildElement("params");
	if( params )
	{
		TiXmlElement * param = params->FirstChildElement("param");
		for (; param; param = param->NextSiblingElement("param") )
		{
			const char * name = param->Attribute("name");
			const char * type = param->Attribute("type");
			const char * val = param->Attribute("val");
			if (name && type && val)
			{
				SetParam(name, type, val);
			}			
		}
	}

}

void CGameObject::SetParam( const char * name, const char * type, const char * val )
{
	mParams[ IwHashString(name) ] = CreateVariant(name, type, val);
}

#if APP_SAVE_XML
void CGameObject::Save( TiXmlElement * obj )
{
	if ( obj == NULL )
	{
		return;
	}


	if ( mName.size() != 0 )
	{
		obj->SetAttribute("name", mName.c_str());
	}
	obj->SetAttribute("type", GetClassName());

	obj->SetAttribute("is_active", (mIsActive? 1: 0) );
	obj->SetAttribute("is_visible", (mIsVisible? 1: 0) );

	obj->SetDoubleAttribute("x", mPos.x );
	obj->SetDoubleAttribute("y", mPos.y );
	obj->SetAttribute("z", (int)mZ );

	double deg = RADIANCE_TO_DEGREE(mRot);
	while( fabs(deg) > 360 )
	{
		deg += (deg < 0? 360: -360 );
	}

	obj->SetDoubleAttribute("rot", deg);

	if ( mTemplateName.size() != 0 )
	{
		obj->SetAttribute("template", mTemplateName.c_str());
		return;
	}

	TiXmlElement * sprites = new TiXmlElement( "sprites" );
	SpritesContainer::iterator it = mSprites.begin(), ite = mSprites.end();
	for(; it != ite; it++ )
	{
		TiXmlElement * sprite = new TiXmlElement( "sprite" );

		const SSpriteParams & params = (*it)->initParams;

		if ( params.name.size() != 0 )
		{
			sprite->SetAttribute("name", params.name.c_str());
		}		

		sprite->SetAttribute("tex", params.texName.c_str());
		sprite->SetDoubleAttribute("x", params.x);
		sprite->SetDoubleAttribute("y", params.y);
		sprite->SetDoubleAttribute("w", params.w);
		sprite->SetDoubleAttribute("h", params.h);

		sprite->SetAttribute("tx", (int)params.tx);
		sprite->SetAttribute("ty", (int)params.ty);
		sprite->SetAttribute("tw", (int)params.tw);
		sprite->SetAttribute("th", (int)params.th);		

		sprite->SetAttribute("blend_mode", params.blendMode);
		sprite->SetAttribute("flip_h", params.isFlipH);
		sprite->SetAttribute("flip_v", params.isFlipV);

		if (params.rot != 0.0)
		{
			float r = RADIANCE_TO_DEGREE(params.rot);
			sprite->SetDoubleAttribute("rot", r);
		}

		if (params.isAnimation != 0)
		{
			sprite->SetAttribute("is_animation", params.isAnimation);
			sprite->SetAttribute("fps", (int)params.fps);
			sprite->SetAttribute("frames", (int)params.frames);
			sprite->SetAttribute("is_loop", (int)params.isLoop);
			if (params.isPingPong)
			{
				sprite->SetAttribute("is_pingpong", (int)params.isPingPong);
			}			
		}

		if ( params.isVisible == 0 )
		{
			sprite->SetAttribute("is_visible", params.isVisible);
		}


		sprites->LinkEndChild(sprite);
	}
	obj->LinkEndChild(sprites);

	if ( !mParams.empty() )
	{
		TiXmlElement * params_elem = new TiXmlElement( "params" );

		VariantMap::iterator it = mParams.begin(), ite = mParams.end();
		char buff[128] = {0};
		for (; it != ite; it++ )
		{
			TiXmlElement * param_elem = new TiXmlElement( "param" );
			buff[0] = 0;
			VariantToStr(it->second, buff, 127);
			param_elem->SetAttribute( "name", it->second.name.c_str() );
			param_elem->SetAttribute( "val", buff );
			param_elem->SetAttribute( "type", GetVarName(it->second.type) );
			
			params_elem->LinkEndChild(param_elem);			
		}

		obj->LinkEndChild(params_elem);
	}

}

#endif // APP_SAVE_XML

void CGameObject::SetActive( bool isActive )
{
	mIsActive = isActive;
}

void CGameObject::SetVisible( bool isVisible )
{
	mIsVisible = isVisible;
}

CIwFVec2 CGameObject::GetPos() const
{
	return mPos;
}

SpritePtr CGameObject::GetSprite( size_t id ) const
{
	if ( id < mSprites.size() )
	{
		return mSprites[id]->sprt;
	}
	IwAssertMsg(GAME, false, ("CGameObject::GetSprite"));
	return SpritePtr();
}

SpritePtr CGameObject::GetSprite( CIwStringS name ) const
{
	SpritesContainer::const_iterator it = mSprites.begin(), ite = mSprites.end();
	for (; it != ite; it++ )
	{
		if ( (*it)->name.size() > 0 && (*it)->name == name )
		{
			return (*it)->sprt;
		}
	}

	return SpritePtr();
}

CIwStringS CGameObject::GetName() const
{
	return mName;
}

void CGameObject::Kill()
{
	mIsDead = true;
}


float CGameObject::GetRot() const
{
	return mRot;
}

CGameObject* CGameObject::Clone() const
{
	return new CGameObject( *this );
}

void CGameObject::SetPos( CIwFVec2 pos )
{
	mPos = pos;
	UpdateTransform();
}

void CGameObject::SetRot( float angle )
{
	mRot = angle;
	UpdateTransform();
}

void CGameObject::UpdateTransform()
{
	mCacheTransform.SetZero();
	mCacheTransform.SetRot( mRot );
	mCacheTransform.SetTrans( mPos );
	mCacheTransform.Normalise();
}

CIwFMat2D CGameObject::GetCachedTransform() const
{
	return mCacheTransform;
}

float CGameObject::GetZ() const
{
	return mZ;
}

bool CGameObject::IsHitTest( int32 x, int32 y, bool isPhysFirst )
{
	CIwFVec2 v( (float)x, (float)y );	

	{
		v = mCacheTransform.TransposeTransformVec( v );

		for ( size_t i = 0; i < mSprites.size(); i++ )
		{
			if ( mSprites[i]->sprt->IsHitTest(v.x - mSprites[i]->pos.x, v.y -  mSprites[i]->pos.y, mSprites[i]->rot) )
			{
				return true;
			}
		}
	}

	return false;
}

bool CGameObject::IsActive() const
{
	return mIsActive;
}

bool CGameObject::IsVisible() const
{
	return mIsVisible;
}

void CGameObject::SetRotateSprite( float rot, size_t id )
{
	if ( id < mSprites.size() && id >= 0 )
	{
		SpriteDefPtr def = mSprites[id];
		def->rot = rot;
	}
}


void CGameObject::SetPosSprite( float x, float y, size_t id )
{
	if ( id < mSprites.size() && id >= 0 )
	{
		SpriteDefPtr def = mSprites[id];
		def->pos.x = x;
		def->pos.y = y;
	}
}


void CGameObject::SetVisibleSprite( bool isVisible, size_t id )
{
	if ( id < mSprites.size() && id >= 0 )
	{
		SpriteDefPtr def = mSprites[id];
		def->isVisible = isVisible;
	}
}

void CGameObject::SetVisibleSprite( bool isVisible, const char * name )
{
	for ( size_t i = 0; i < mSprites.size(); i++ )
	{
		SpriteDefPtr def = mSprites[i];
		if ( def->name == name )
		{
			SetVisibleSprite(isVisible, i);
			return;
		}
	}
}


void CGameObject::SetName( const char * name )
{
	mName = name;
}

void CGameObject::OnClick()
{

}

void CGameObject::OnDblClick()
{

}


void CGameObject::OnDrag( int32 x, int32 y )
{

}

SVariant CGameObject::CreateVariant( const char * name, const char * type, const char * val )
{
	EVariantType vt = GetVarType(type);
	SVariant v;
	v.name = name;
	v.type = vt;

	switch( vt )
	{
	case INT:
		{
			int i = 0;
			sscanf(val, "%d", &i);
			v.i = i;
		}break;
	case FLOAT:
		{
			float f = 0.f;
			sscanf(val, "%f", &f);
			v.f = f;
		}break;
	case STRING:
		{
			v.str = val;
		}break;
	default:
		{}
	}

	return v;
}

bool CGameObject::GetParamFloat( const char * name, float & val )
{
	SVariant v;
	if ( FindParam(name, v) )
	{
		IwAssert(GAME, v.type == FLOAT);
		val = v.f;
		return true;
	}
	return false;
}

bool CGameObject::GetParamInt( const char * name, int & val )
{
	SVariant v;
	if ( FindParam(name, v) )
	{
		IwAssert(GAME, v.type == INT);
		val = v.i;
		return true;
	}
	return false;
}

bool CGameObject::GetParamStr( const char * name, CIwStringS & str )
{
	SVariant v;
	if ( FindParam(name, v) )
	{
		IwAssert(GAME, v.type == STRING);
		str = v.str;
		return true;
	}

	return false;
}

bool CGameObject::FindParam( const char * name, SVariant & v )
{
	uint32 hash = IwHashString(name);

	VariantMap::iterator it = mParams.find(hash);
	if ( it != mParams.end())
	{
		v = it->second;
		return true;
	}
	return false;

	/*if ( !mParams.empty() ) 
	{
		VariantMap::iterator it = mParams.begin(), ite = mParams.end();
		for (; it != ite; it++ )
		{
			if (strcasecmp(it->first.c_str(), name) == 0)
			{
				v = it->second;
				return true;
			}
		}
	}*/
	return false;
}

void CGameObject::VariantToStr( const SVariant & v, char * buff, int size )
{
	switch( v.type )
	{
	case INT:
		{
			snprintf(buff, size, "%d", v.i);
		}break;
	case FLOAT:
		{
			snprintf(buff, size, "%f", v.f);
		}break;
	case STRING:
		{
			snprintf(buff, size, "%s", v.str.c_str());
		}break;
	default:
		{}
	}
}

void CGameObject::SetPause( bool isPaused )
{
	mIsPaused = isPaused;
}

void CGameObject::SetZ( float z )
{
	mZ = z;
	/*if ( mLevel != NULL )
	{
		mLevel->OnObjChangeZ();
	}*/
}


void CGameObject::AddSprite( const SSpriteParams & params )
{
	SpritePtr sprt = CSprite::Create( params );
	SpriteDefPtr def = new SSpriteDef;
	def->isVisible = (params.isVisible != 0);
	def->pos       = CIwFVec2(params.x, params.y);
	def->rot       = params.rot;
	def->name      = params.name;
	def->sprt      = sprt;

	mSprites.push_back( def );
}

void CGameObject::SetAlpha( float a )
{
	SpritesContainer::iterator it = mSprites.begin(), ite = mSprites.end();
	for(; it != ite; it++)
	{
		CIwColour c = (*it)->sprt->GetColor();
		c.a = uint8(255*a);
		(*it)->sprt->SetColor(c);

	}
}

void CGameObject::Disappear( float time )
{
	mIsDisappearing = true;
	mDisappearingTime = time;
	mDisappearingInitTime = time;

}

ObjectId CGameObject::GetUniqueId()
{
	return ++mIds;
}

ObjectId CGameObject::GetId() const
{
	return mId;
}




_END_NAMESPACE_