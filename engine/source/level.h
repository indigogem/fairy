#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "defs.h"
#include "smartptr.h"
#include "list"
#include "map"
#include "game_object.h"
#include "tinyxml.h"

_BEGIN_NAMESPACE_

struct SObjTemplate
{
	TiXmlElement * elem;
	bool           included;
};
typedef std::list<TiXmlElement *>          TiXmlElementContainer;
typedef std::map<CIwStringS, SObjTemplate> ObjectTemplates;
typedef std::list<GameObjectPtr>           GameObjectsContainer;

class CLevel : public ref_counter
{
public:
	CLevel();
	~CLevel();

public:
	void           Init();
	void           Update(int32 dt);
	void           Render();
	void           AddObject(GameObjectPtr obj);
	void           RemoveObject(GameObjectPtr obj);
	void           Load(TiXmlElement * root);
	void           Deinit();
	GameObjectPtr  CreateObject(const char * type, const char * templ);

	void           AddTemplate(TiXmlElement * elem, bool isIncluded );
	TiXmlElement * GetTemplate( const char * name );

#if APP_SAVE_XML
	void           Save(TiXmlElement * root);
#endif // APP_SAVE_XML

	GameObjectPtr  GetObjByName(CIwStringS name) const;
	GameObjectPtr  GetObjById(ObjectId id) const;
	GameObjectPtr  HitTest( int32 x, int32 y, bool isIgnoreInactive = true );

	uint32         GetSceneWidth() const;
	uint32         GetSceneHeight() const;

	void           SetInitCam( CIwFVec2 pos );

	const  GameObjectsContainer & GetObjs() const;

	void           ProccedAddObjs();



public:
	static bool   IsObjDeadPred(const GameObjectPtr& obj);
	static bool   OrderZPred( const GameObjectPtr & obj1, const GameObjectPtr & obj2 );

	static bool     OpenXmlFile(const char * path, TiXmlDocument * doc);

protected:
	void          SortObjs();
	void          RegisterScriptFunctions();
	void          DoAddObjs();

private:
	float                 mDt;
	GameObjectsContainer  mObjs;
	GameObjectsContainer  mObjsToAdd;
	uint32                mSceneWidth;
	uint32                mSceneHeight;

	ObjectTemplates       mTemplates;
	TiXmlElementContainer mIncludes;

	CIwFVec2              mInitCam;

};

typedef shared_ptr<CLevel> LevelPtr;

_END_NAMESPACE_

#endif // _LEVEL_H_
