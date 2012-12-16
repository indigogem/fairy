#ifndef _GAME_H_
#define _GAME_H_

#include "map"
#include "vector"
#include "IwGxFont.h"
#include "app.h"
#include "list"
#include "tinyxml.h"
#include "hash_map"
#include "smartptr.h"

_BEGIN_NAMESPACE_

#ifdef IW_DEBUG
	#define DEBUG_TEXT(x,y,txt) CGame::GetInst().DebugAddText(x,y,txt)
	#define DEBUG_TEXT_EX(x,y,format, ...) CGame::GetInst().DebugAddTextEx(x, y, format, __VA_ARGS__)
#else
	#define DEBUG_TEXT(x,y,txt) ((void)0)
	#define DEBUG_TEXT_EX(x,y,format, ...) ((void)0)
#endif


class CGameState;

class CGame : public CApp
{
public:
	CGame();
	virtual ~CGame();

	enum EGameState
	{
		GAME_PLAY
	};

	typedef std::map<EGameState, CGameState*> GameStatesMap;
	typedef std::vector<CGameState*> GameStatesStack;

public:
	virtual void            OnInit();
	virtual void            OnDeinit();
	virtual void			OnRender();
	virtual void			OnUpdate( uint32 delta );
	virtual void            OnUpdateFrame();
	virtual void            OnPointerUp( int32 x, int32 y );
	virtual void            OnPointerDown( int32 x, int32 y );
	virtual void            OnPointerMove( int32 x, int32 y );
	virtual void			OnKeyboardChar( s3eWChar ch );
	virtual void            OnZoom( float scale );

public:
	CGameState *       GetState(EGameState state);
	CGameState *       GetCurrState() const;
	void               PushState(EGameState state);
	void               PopState();
	void               SwapState(EGameState state);
		
	void               LoadProfile();
	void               SaveProfile();

	//sound
	void               SoundMute( bool isMute );
	void               SoundPlay(const char * name);
	void               SoundStopAll();
	
#ifdef IW_DEBUG 
	// debug
	void            DebugRenderText(int x, int y, const char * txt);
	void            DebugAddTextEx(int x, int y, const char * format, ...);
	void            DebugAddText(int x, int y, const char * txt);
	void            DebugRender();
#endif // IW_DEBUG

	static CGame &  GetInst();
	static void     Destroy();

	static void     OnResetRenderParams();

	void            SendStatistics();

private:

	void            InitStates();
	void            InitResources();
	void            InitNetwork();
	void            DeinitNetwork();
	void            InitGui();
	void            InitDialog(const char * name);

private:
	static CGame*        mInstance;

	GameStatesMap        mStates;
	GameStatesStack      mStatesStack;


#ifdef IW_DEBUG
	CIwGxFont*      mDebugFont;

	static const int DEBUG_MAX_TEXT = 128;
	struct SDebugText
	{
		int x;
		int y;
		char txt[DEBUG_MAX_TEXT];
	};

	std::list<SDebugText> mDebugInfo;

#endif

};
_END_NAMESPACE_
#endif // _GAME_H_