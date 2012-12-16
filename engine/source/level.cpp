#include "IwGx.h"
#include "level.h"
#include "sprite.h"
#include "particle.h"

_BEGIN_NAMESPACE_

bool CLevel::IsObjDeadPred(const GameObjectPtr& obj)
{
	if( obj->IsDead() )
	{
		obj->Deinit();
	}
	return obj->IsDead();
}

bool CLevel::OrderZPred( const GameObjectPtr & obj1, const GameObjectPtr & obj2 )
{
	if ( obj1 != NULL && obj2 != NULL )
	{
		return obj1->GetZ() < obj2->GetZ();
	}
	return false;
}

CLevel::CLevel()
	: mSceneWidth(VIRTUAL_WIDTH)
	, mSceneHeight(VIRTUAL_HEIGHT)
{
	mInitCam.x = 0;
	mInitCam.y = 0;
}

CLevel::~CLevel()
{
	Deinit();
}


void CLevel::Update( int32 dt )
{
	float dft = (float)dt / 1000.f;

	DoAddObjs();

	GameObjectsContainer::iterator it = mObjs.begin(), ite = mObjs.end();

	for(; it != ite; it++ )
	{
		(*it)->Update( dft );

		CIwFVec2 pos = (*it)->GetPos();
	}	

	mObjs.remove_if( IsObjDeadPred );

	SortObjs();
}

void CLevel::Render()
{
	GameObjectsContainer::iterator it = mObjs.begin(), ite = mObjs.end();

	for(; it != ite; it++ )
	{
		(*it)->Draw();
	}

	CRenderer::GetInst().Flush();
}

void CLevel::Init()
{
	CRenderer::GetInst().GetCamera().Reset();
	CRenderer::GetInst().GetCamera().SetPos( mInitCam );

}

void CLevel::AddObject( GameObjectPtr obj )
{
	if ( std::find(mObjsToAdd.begin(), mObjsToAdd.end(), obj ) == mObjsToAdd.end())
	{
		mObjsToAdd.push_back(obj);
	}	
}

void CLevel::Load( TiXmlElement * root )
{
	if ( root == NULL )
	{
		return;
	}
	int sw = VIRTUAL_WIDTH;
	int sh = VIRTUAL_HEIGHT;
	root->QueryIntAttribute("w", &sw);
	root->QueryIntAttribute("h", &sh);

	mSceneWidth = (uint32)sw;
	mSceneHeight = (uint32)sh;

	int initCamX = 0;
	int initCamY = 0;

	root->QueryIntAttribute("init_cam_x", &initCamX);
	root->QueryIntAttribute("init_cam_y", &initCamY);

	mInitCam.x = (float)initCamX;
	mInitCam.y = (float)initCamY;

		
	// includes
	TiXmlElement * elem_inc = root->FirstChildElement("include");
	for(; elem_inc; elem_inc = elem_inc->NextSiblingElement("include"))
	{
		const char * path = elem_inc->Attribute("path");
		if ( path != 0 )
		{
			TiXmlDocument doc;
			if ( OpenXmlFile(path, &doc) )
			{
				TiXmlElement * elem_templ = doc.FirstChildElement("template");
				for(; elem_templ; elem_templ = elem_templ->NextSiblingElement("template"))
				{
					AddTemplate( (TiXmlElement *)elem_templ->Clone(), true );
				}
			}

		}

		mIncludes.push_back((TiXmlElement *)elem_inc->Clone());
	}


	// templates

	TiXmlElement * elem_templ = root->FirstChildElement("template");
	for(; elem_templ; elem_templ = elem_templ->NextSiblingElement("template"))
	{
		AddTemplate( (TiXmlElement *)elem_templ->Clone(), false );
	}


	TiXmlElement * elem_obj = root->FirstChildElement("object");

	for(; elem_obj; elem_obj = elem_obj->NextSiblingElement("object"))
	{
		GameObjectPtr obj;
		const char * type = elem_obj->Attribute("type");
		const char * tmpl = elem_obj->Attribute("template");
		obj = CreateObject( type, tmpl );
		IwAssert(LEVEL, obj != NULL);

		obj->Load( elem_obj );
		obj->Init();
		AddObject( obj );
#if APP_SAVE_XML
		obj->SetSave(true);
#endif

	}

	DoAddObjs();
}
#if APP_SAVE_XML
void CLevel::Save(TiXmlElement * root)
{
	root->SetAttribute("w", (int)mSceneWidth);
	root->SetAttribute("h", (int)mSceneHeight);

	root->SetAttribute("init_cam_x", (int)mInitCam.x);
	root->SetAttribute("init_cam_y", (int)mInitCam.y);

	{	
		TiXmlElementContainer::iterator it = mIncludes.begin(), ite = mIncludes.end();
		for(; it != ite; it++ )
		{
			root->LinkEndChild( (*it)->Clone() );
		}
	}
	{	
		ObjectTemplates::iterator it = mTemplates.begin(), ite = mTemplates.end();
		for(; it != ite; it++ )
		{
			if ( !(it->second.included) )
			{
				root->LinkEndChild( (it->second.elem)->Clone() );
			}			
		}
	}

	GameObjectsContainer::iterator it = mObjs.begin(), ite = mObjs.end();
	for(; it != ite; it++ )
	{
		if ( (*it)->IsSaveable() )
		{
			TiXmlElement * obj = new TiXmlElement( "object" );
			(*it)->Save( obj );
			root->LinkEndChild( obj );
		}
	}
}
#endif // APP_SAVE_XML

void CLevel::Deinit()
{
	CRenderer::GetInst().GetCamera().Reset();
	
	{
		TiXmlElementContainer::iterator it = mIncludes.begin(), ite = mIncludes.end();
		for(; it != ite; it++ )
		{
			SAFE_DELETE( *it );
		}
		mIncludes.clear();
	}

	{
		ObjectTemplates::iterator it = mTemplates.begin(), ite = mTemplates.end();
		for(; it != ite; it++ )
		{
			SAFE_DELETE( it->second.elem );
		}
		mTemplates.clear();
	}

	GameObjectsContainer::iterator it = mObjs.begin(), ite = mObjs.end();
	for(; it != ite; it++ )
	{
		(*it)->Deinit();
	}
	mObjs.clear();

	
}

GameObjectPtr CLevel::GetObjByName( CIwStringS name ) const
{
	GameObjectsContainer::const_iterator it = mObjs.begin(), ite = mObjs.end();
	for(; it != ite; it++ )
	{
		if ( (*it)->GetName() == name )
		{
			return *it;
		}
	}

	return GameObjectPtr();
}

void CLevel::SortObjs()
{
	mObjs.sort( OrderZPred );
}

GameObjectPtr CLevel::HitTest( int32 x, int32 y, bool isIgnoreInactive )
{
	GameObjectsContainer::const_reverse_iterator it = mObjs.rbegin(), ite = mObjs.rend();

	for (; it != ite; it++ )
	{
		if ( ((*it)->IsActive() || !isIgnoreInactive) && (*it)->IsHitTest(x, y) )
		{
			return *it;
		}
	}

	return GameObjectPtr();
}

void CLevel::RemoveObject( GameObjectPtr obj )
{
	if ( obj != NULL )
	{
		//obj->Kill();
		//obj->Deinit();
		mObjs.remove( obj );
	}

}

uint32 CLevel::GetSceneWidth() const
{
	return mSceneWidth;
}

uint32 CLevel::GetSceneHeight() const
{
	return mSceneHeight;
}

const GameObjectsContainer & CLevel::GetObjs() const
{
	return mObjs;
}


void CLevel::AddTemplate( TiXmlElement * elem, bool isIncluded  )
{
	const char * name = elem->Attribute("name");
	SObjTemplate t;
	t.elem = elem;
	t.included = isIncluded;

	// if we find template with same name we replace it
	ObjectTemplates::iterator it = mTemplates.find( name );
	if ( it != mTemplates.end() )
	{
		SAFE_DELETE( it->second.elem );
		mTemplates.erase( it );
	}

	mTemplates[ name ] = t;
}

TiXmlElement * CLevel::GetTemplate( const char * name )
{
	ObjectTemplates::const_iterator it = mTemplates.find( name );
	if ( it != mTemplates.end() )
	{
		return it->second.elem;
	}

	IwAssertMsg(GAME, false, ("InstanceTemplate can't find template='%s'", name));
	return NULL;
}

GameObjectPtr CLevel::CreateObject( const char * type, const char * templ )
{
	GameObjectPtr obj;

	if ( type != NULL && type[0] != 0 )
	{
		if ( strcasecmp(type, "cgameobject") == 0 )
		{
			obj = new CGameObject( this );
		}
		else if ( strcasecmp(type, "cparticlesystem") == 0 )
		{
			obj = new CParticleSystem( this );
		}
		else
		{
			IwAssertMsg(GAME, false, ("Cant load unknown type '%s'", type));
		}
	}
	else
	{
		IwAssertMsg(GAME, false, ("CLevel::CreateObject invalid type"));
	}

	if ( templ != NULL )
	{
		TiXmlElement * elem_tmpl = GetTemplate( templ );
		obj->Load( elem_tmpl );
	}

	return obj;
}

void CLevel::SetInitCam( CIwFVec2 pos )
{
	mInitCam = pos;
}


void CLevel::DoAddObjs()
{
	if ( mObjsToAdd.size() > 0 )
	{
		mObjs.insert( mObjs.end(), mObjsToAdd.begin(), mObjsToAdd.end() );
		SortObjs();
		mObjsToAdd.clear();
	}
}

void CLevel::ProccedAddObjs()
{
	DoAddObjs();
}

bool CLevel::OpenXmlFile( const char * path, TiXmlDocument * doc )
{
	if ( doc == NULL )
	{
		return false;
	}
	s3eFile * file = s3eFileOpen(path, "r");
	if ( file != NULL )
	{
		s3eFileSeek(file, 0, S3E_FILESEEK_END);
		long len = s3eFileTell( file );
		s3eFileSeek(file, 0, S3E_FILESEEK_SET);

		char * buff = new char[len];
		s3eFileRead(buff, sizeof(char), len, file);

		doc->Parse(buff);

		SAFE_DELETE_ARRAY( buff );
	}
	else
	{
		IwAssertMsg(GAME, false, ("can't open file '%s'", path));
		return false;
	}
	s3eFileClose(file);

	return true;
}

_END_NAMESPACE_
