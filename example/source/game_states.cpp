#include "game_states.h"
#include "level.h"
#include "game.h"

_BEGIN_NAMESPACE_


void CGameState::LoadLevel( const char * lvl )
{

	TiXmlDocument doc;

	CLevel::OpenXmlFile(lvl, &doc);

	TiXmlElement * elem_scene = NULL;
	elem_scene = doc.FirstChildElement("scene");

	mLevel = new CLevel();
	mLevel->Load( elem_scene );
	mLevel->Init();
}

LevelPtr CGameState::GetLevel() const
{
	return mLevel;
}

_END_NAMESPACE_
