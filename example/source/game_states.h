#ifndef _GAME_STATES_H_
#define _GAME_STATES_H_

#include "defs.h"
#include "level.h"

_BEGIN_NAMESPACE_

class CLevel;

class CGameState
{
public:
	CGameState(){ mIsPaused = false; };
	virtual ~CGameState(){};

	virtual void Update( uint32 delta ){};
	virtual void Render(){};
	virtual void OnPointerUp( int32 x, int32 y ){};
	virtual void OnPointerDown( int32 x, int32 y ){};
	virtual void OnPointerMove( int32 x, int32 y ){};
	virtual void OnKeyboardChar( s3eWChar ch ){};
	virtual void OnZoom( float scale ) {};

	virtual void OnPush(){};
	virtual void OnPop(){};

	void         LoadLevel( const char * lvl );
	void         LoadCurrLevel();
	LevelPtr     GetLevel() const;

protected:
	LevelPtr      mLevel;
	bool          mIsPaused;
	CIwFVec2      mLastPointer;
};


_END_NAMESPACE_

#endif // _GAME_STATES_H_