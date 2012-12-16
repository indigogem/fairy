#ifndef _GAME_STATE_PLAY_
#define _GAME_STATE_PLAY_

#include "defs.h"
#include "game_states.h"

_BEGIN_NAMESPACE_

class CGamePlay : public CGameState
{
public:

	CGamePlay();
	virtual ~CGamePlay();

	virtual void Update( uint32 delta );
	virtual void Render();
	virtual void OnPointerUp( int32 x, int32 y );
	virtual void OnPointerDown( int32 x, int32 y );
	virtual void OnPointerMove( int32 x, int32 y );
	virtual void OnKeyboardChar( s3eWChar ch );
	virtual void OnZoom( float scale );

	virtual void OnPush();
	virtual void OnPop();


protected:
	GameObjectPtr mFairy;
	GameObjectPtr mDust;
	GameObjectPtr mBrick;
	float mVel;
};




_END_NAMESPACE_
#endif // _GAME_STATE_PLAY_