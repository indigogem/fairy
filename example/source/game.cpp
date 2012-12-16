#include "defs.h"
#include "game.h"
#include "game_states.h"
#include "app.h"
#include "game_state_play.h"
#include "IwDebug.h"
#include "IwResManager.h"

_BEGIN_NAMESPACE_

CGame * CGame::mInstance = NULL;

CGame::CGame()
{
}

CGame::~CGame()
{
}

void CGame::Destroy()
{
	SAFE_DELETE(mInstance);
}

CGame & CGame::GetInst()
{
	if( mInstance == NULL )
	{
		mInstance = new CGame();
	}
	return *mInstance;
}

void CGame::OnUpdate( uint32 delta )
{
	float dft = (float)delta / 1000.f;
	
	if (!mStatesStack.empty())
	{
		mStatesStack.back()->Update( delta );
	}	
}

CGameState * CGame::GetState(EGameState state)
{
	if (mStates.find(state) != mStates.end())
	{
		return mStates[state];
	}

	IwAssertMsg(GAME, false, ("CGame::GetState"));
	return NULL;
}


void CGame::OnInit()
{

	CRenderer::GetInst().SetOnResetParamsCallback( OnResetRenderParams );

	InitResources();
	InitStates();

	PushState(GAME_PLAY);
}

void CGame::OnDeinit()
{
	while ( !mStatesStack.empty() )
	{
		PopState();
	}

	GameStatesMap::iterator it = mStates.begin(), ite = mStates.end();
	for (; it != ite; it++)
	{
		SAFE_DELETE(it->second);
	}
	mStates.clear();
}

void CGame::PushState( EGameState state )
{
	IwTrace(GAME, ("PushState %d", (int)state));
	if (mStatesStack.empty() || mStatesStack.back() != GetState(state))
	{
		mStatesStack.push_back( GetState(state) );
		mStatesStack.back()->OnPush();
	}
}

void CGame::PopState()
{
	if ( !mStatesStack.empty() )
	{
		IwTrace(GAME, ("PopState"));
		mStatesStack.back()->OnPop();
		mStatesStack.pop_back();
	}
}

void CGame::OnRender()
{
	if ( !mStatesStack.empty() )
	{
		mStatesStack.back()->Render();
	}
	
#if IW_DEBUG
	CRenderer::GetInst().Flush();
	DebugRender();
	//OutputBuffer::bufferToScreen();
#endif // IW_DEBUG
}

void CGame::OnResetRenderParams()
{
	// do some stuff with GUI to rescale etc.
}

void CGame::OnPointerUp( int32 x, int32 y )
{
	if ( !mStatesStack.empty() )
	{
		mStatesStack.back()->OnPointerUp( x, y );
	}
}

void CGame::OnPointerDown( int32 x, int32 y )
{
	if ( !mStatesStack.empty() )
	{
		mStatesStack.back()->OnPointerDown( x, y );
	}
}

void CGame::OnPointerMove( int32 x, int32 y )
{
	if ( !mStatesStack.empty() )
	{
		mStatesStack.back()->OnPointerMove( x, y );
	}
}

void CGame::OnZoom( float scale )
{
	if ( !mStatesStack.empty() )
	{
		mStatesStack.back()->OnZoom( scale );
	}
}

void CGame::OnKeyboardChar( s3eWChar ch )
{
	bool handled = false;
	s3eWChar c = towlower(ch);

	if ( !handled && !mStatesStack.empty() )
	{
		mStatesStack.back()->OnKeyboardChar( ch );
	}

}

void CGame::SwapState( EGameState state )
{
	IwTrace(GAME, ("SwapState %d", (int)state));
	PopState();
	PushState( state );
}


#ifdef IW_DEBUG
void CGame::DebugRenderText( int x, int y, const char * txt )
{
	IwGxFontClearFlags(0xffffffff);
	IwGxLightingOn();
	CRenderer::GetInst().DefaultMaterial();
	IwGxFontSetFont( mDebugFont );

	//IwGxFontSetCol(0xFF000000);
	IwGxFontSetCol(0xFFFFFFFF);

	//Set the formatting rect - this controls where the text appears and what it is formatted against
	IwGxFontSetRect(CIwRect(x,y,(int16)200,(int16)50));

	//Draw the text
	IwGxFontDrawText(txt);

	IwGxFlush();

	IwGxLightingOff();
}
void CGame::DebugAddText(int x, int y, const char * txt)
{
	SDebugText info;
	info.x = x;
	info.y = y;
	memset(info.txt, 0, DEBUG_MAX_TEXT);
	strlcpy(info.txt, txt, DEBUG_MAX_TEXT);

	mDebugInfo.push_back( info );
}

void CGame::DebugRender()
{
	std::list<SDebugText>::iterator it = mDebugInfo.begin(), ite = mDebugInfo.end();
	for (; it != ite; it++ )
	{
		DebugRenderText( (*it).x, (*it).y, (*it).txt );
	}
	mDebugInfo.clear();
}

void CGame::DebugAddTextEx( int x, int y, const char * format, ... )
{
	va_list		argptr;
	va_start	( argptr, format );

	char buff[ DEBUG_MAX_TEXT ] = {0};
	vsnprintf( buff, DEBUG_MAX_TEXT, format, argptr );
	va_end( argptr );

	DebugAddText(x, y, buff);
}
#endif // IW_DEBUG


void CGame::InitStates()
{
	mStates[GAME_PLAY] = new CGamePlay();
}

void CGame::InitResources()
{
	CIwResGroup * mainRes = IwGetResManager()->LoadGroup("./res.group");



#ifdef IW_DEBUG 
	mDebugFont = (CIwGxFont*)IwGetResManager()->GetResNamed("debug_font", "CIwGxFont");
#endif

	IwGetResManager()->SetCurrentGroup(mainRes);
}

CGameState * CGame::GetCurrState() const
{
	return mStatesStack.back();
}

void CGame::OnUpdateFrame()
{
	// call some stuff that need be updated each frame, i.e. network
}

_END_NAMESPACE_

