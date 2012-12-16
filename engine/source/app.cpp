#include "app.h"
#include "s3eTypes.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"
#include "s3eTimer.h"
#include "IwGx.h"
#include "IwMemory.h"

#include "renderer.h"

_BEGIN_NAMESPACE_

CApp::CApp()
	: mIsQuit(false)
	, mIsPaused(false)
{
	mMillisecondsPerLogicTick = 0;
}

CApp::~CApp()
{
}

void CApp::Init()
{
	InitFramerate( 30 );

	CRenderer::GetInst().Init();

	ResetTouch();

	OnInit();
}

void CApp::Deinit()
{
	if ( s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) )
	{
		UnRegisterMultitouch();
	}
	else
	{
		UnRegisterSingletouch();
	}

	OnDeinit();
	
	CRenderer::GetInst().Destroy();
}

void CApp::Run()
{
	while ( !mIsQuit )
	{
		s3eDeviceYield(0);
		if ( s3eDeviceCheckQuitRequest() )
		{
			return;
		}

		if (!IsPaused())
		{
			static uint32 d = (int32) s3eTimerGetMs();

			// Force frame rate down to be similar to on-device
			// (by constantly updating timer until we're ready for a new tick)
			do 
			{

				UpdateTimer();
				s3eDeviceYield(0);
			} 
			while ((int32)(s3eTimerGetMs() - d) < mMillisecondsPerLogicTick);

			d = (int32) s3eTimerGetMs();
		}
		else
		{
			s3eDeviceYield(0);
		}

		UpdateTimer();

		s3eDeviceYield(0);
		if (mMillisecondsPassed >= mMillisecondsPerLogicTick)
		{
			// We're ready to execute at least one frame
			while (mMillisecondsPassed >= mMillisecondsPerLogicTick)
			{
				s3eDeviceYield(0);
				mMillisecondsPassed -= mMillisecondsPerLogicTick;

				// Read controls
				UpdateControls();
				
				// Timer update (So we'll iterate this loop again if we need to)
				int32 oldMilliseconds = mMilliseconds;

				UpdateTimer();

				// Call game object update tick
				Update( mMillisecondsPerLogicTick );

				if (mMilliseconds - oldMilliseconds > mMillisecondsPerLogicTick)
				{
					//return; //just give up
				}
			}
		}
		else
		{
			UpdateControls();
		}

		OnUpdateFrame();
		Render();
	}
}

void CApp::Render()
{
	CRenderer::GetInst().Begin();
	OnRender();
	CRenderer::GetInst().End();

}

void CApp::Update( uint32 delta )
{
	
#if APP_ENABLE_KEYBOARD

	s3eWChar ch = s3eKeyboardGetChar();  
	if ( ch != S3E_WEOF )
	{
		OnKeyboardChar( ch );
	}
#endif

	//s3eKeyboardClearState();

	OnUpdate( delta );

}

bool CApp::IsPaused() const
{
	return mIsPaused;
}

void CApp::InitFramerate( int32 fps )
{
	IwAssertMsg(GAME, mMillisecondsPerLogicTick == 0,
		("InitFramerate() should only be called once"));

	ChangeFramerate( fps );
}

void CApp::ChangeFramerate( int32 fps )
{
	if( fps > 0 )
	{
		int32 res = s3eTimerGetInt(S3E_TIMER_RESOLUTION);
		int32 millis = 1000 / fps;
		if( res != 1 ) //if counter resolution is not millisecond-accurate
		{
			int32 mod = millis % res;

			//go to nearest milli count
			if( mod > res/2 )
				millis += mod;
			else
				millis -= mod;

			if( millis == 0 ) //if requested frame rate is above counter resolution
				millis = res; //do at least some physics!
		}

		mMillisecondsPerLogicTick = millis;
	}
	else
	{
		mMillisecondsPerLogicTick = 0;
	}
}

void CApp::UpdateTimer()
{
	// Update timers
	static int32 t = (int32) s3eTimerGetMs();
	int32 t2 = (int32) s3eTimerGetMs();

	// If we're not paused, add on any extra milliseconds since the last call
	// to UpdateTimer()
	if (!IsPaused())
	{
		mMilliseconds		+= t2 - t;
		mMillisecondsPassed	+= t2 - t;
	}

	// Update static variable for future comparisons
	t = t2;

	// This occurs when the clock wraps, and needs to be dealt with
	if (mMillisecondsPassed < 0)
	{
		mMillisecondsPassed = 0;
	}

	// Deal with spikes by bounding milliseconds passed
	if (mMillisecondsPassed > mMillisecondsPerLogicTick * 8)
	{
		mMillisecondsPassed = mMillisecondsPerLogicTick;
	}

}

void CApp::UpdateControls()
{
	s3eDeviceYield(0);
	s3eKeyboardUpdate();
	s3ePointerUpdate();
}

CIwVec2 CApp::ConvertToVirtualPoints( const CIwVec2 & p )
{
	const SDeviceParam & dp = CRenderer::GetInst().GetDeviceParam();
	CIwVec2 ret = p;
		
	/*if ( ret.x >= dp.offsetX && ret.x <= (int32)dp.screenWidth - dp.offsetX
		&& ret.y >= dp.offsetY && ret.y <= (int32)dp.screenHeight - dp.offsetY )*/
	{
		ret.x -= dp.offsetX;
		ret.y -= dp.offsetY;
		ret.x = (int32)(ret.x/dp.scale);
		ret.y = (int32)(ret.y/dp.scale);
		return ret;
	}

	/*ret.x = -1;
	ret.y = -1;
	return ret;*/
}

void CApp::MultiTouchButtonCB( s3ePointerTouchEvent* e, CApp * app )
{
	STouch* touch = app->GetTouch(e->m_TouchID);
	if (touch)
	{
		touch->active = e->m_Pressed != 0;
		touch->x = e->m_x;
		touch->y = e->m_y;
	}
	
	if ( e->m_TouchID == 0 )
	{
		CIwVec2 p( e->m_x, e->m_y);
		p = app->ConvertToVirtualPoints( p );

		if (e->m_Pressed == 0 )
		{
			app->OnPointerUp( p.x, p.y );
		}
		else
		{
			app->OnPointerDown( p.x, p.y );
		}
		
	}

}

void CApp::MultiTouchMotionCB( s3ePointerTouchMotionEvent* e, CApp * app )
{
	STouch* touch = app->GetTouch(e->m_TouchID);
	if (touch)
	{
		touch->x = e->m_x;
		touch->y = e->m_y;
	}

	int touchNum = app->GetActiveTouchesNum();
	if ( touchNum == 2 && !app->mIsZooming )
	{
		 app->StartZooming();
	}
	else if ( touchNum == 2 && app->mIsZooming )
	{
		 app->OnZoom( app->GetZoomingScale() );
	}
	else
	{
		if (app->mIsZooming)
		{
			app->EndZooming();
		}

		if ( e->m_TouchID == 0 )
		{
			CIwVec2 p( e->m_x, e->m_y);
			p = app->ConvertToVirtualPoints( p );
			app->OnPointerMove( p.x, p.y );
		}
	}
}

void CApp::RegisterMultitouch()
{
	s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB, this);
	s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB, this);
}

void CApp::UnRegisterMultitouch()
{
	s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB);
	s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB);
}

void CApp::RegisterSingletouch()
{
	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouchButtonCB, this);
	s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotionCB, this);
}

void CApp::UnRegisterSingletouch()
{
	s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouchButtonCB);
	s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotionCB);
}

void CApp::SingleTouchButtonCB( s3ePointerEvent* e, CApp * app )
{
	app->mTouches[0].active = e->m_Pressed != 0;
	app->mTouches[0].x = e->m_x;
	app->mTouches[0].y = e->m_y;

}

void CApp::SingleTouchMotionCB( s3ePointerMotionEvent* e, CApp * app )
{
	app->mTouches[0].x = e->m_x;
	app->mTouches[0].y = e->m_y;
}

CApp::STouch* CApp::GetTouch( int32 id )
{
	STouch* pInActive = NULL;

	for(uint32 i = 0; i < APP_MAX_TOUCHES; i++)
	{
		if( id == mTouches[i].id )
		{
			return &mTouches[i];
		}
		if( !mTouches[i].active )
		{
			pInActive = &mTouches[i];
		}
	}

	//Return first inactive touch
	if( pInActive )
	{
		pInActive->id = id;
		return pInActive;
	}

	//No more touches, give up.
	return NULL;

}

void CApp::ResetTouch()
{
	for(uint32 i = 0; i < APP_MAX_TOUCHES; i++)
	{
		mTouches[i].active = false;
		mTouches[i].x = 0;
		mTouches[i].y = 0;
		mTouches[i].id = 0;
	}

	mMainPointerId = 0;
	EndZooming();
}

int CApp::GetActiveTouchesNum() const
{
	int num = 0;
	for(uint32 i = 0; i < APP_MAX_TOUCHES; i++)
	{
		if( mTouches[i].active )
		{
			num++;
		}
	}
	return num;
}

void CApp::StartZooming()
{
	mIsZooming = true;
	uint32 k = 0;
	for(uint32 i = 0; i < APP_MAX_TOUCHES; i++)
	{
		if( mTouches[i].active && k < 2 )
		{
			mZoomTouchIds[k] = i;
			k++;
		}
	}

	CIwFVec2 v, v1;
	v.x = (float)mTouches[ mZoomTouchIds[0] ].x;
	v.y = (float)mTouches[ mZoomTouchIds[0] ].y;

	v1.x = (float)mTouches[ mZoomTouchIds[1] ].x;
	v1.y = (float)mTouches[ mZoomTouchIds[1] ].y;

	mZoomInitDelta = (v - v1).GetLength();
}

void CApp::EndZooming()
{
	mIsZooming = false;
	mZoomTouchIds[0] = 0;
	mZoomTouchIds[1] = 0;
	mZoomInitDelta = 0;
}

float CApp::GetZoomingScale() const
{
	CIwFVec2 v, v1;
	float delta = 0;
	v.x = (float)mTouches[ mZoomTouchIds[0] ].x;
	v.y = (float)mTouches[ mZoomTouchIds[0] ].y;

	v1.x = (float)mTouches[ mZoomTouchIds[1] ].x;
	v1.y = (float)mTouches[ mZoomTouchIds[1] ].y;

	delta = (v - v1).GetLength();
	return delta / mZoomInitDelta;
}

_END_NAMESPACE_