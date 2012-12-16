#ifndef _APP_H_
#define _APP_H_

#include "defs.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"
#include "list"
#include "vector"
#include "IwGeomVec2.h"
#include "tinyxml.h"

_BEGIN_NAMESPACE_

const uint32 APP_MAX_TOUCHES = 10;

class CApp
{
	
public:
	CApp();
	virtual ~CApp();

	virtual void  OnInit() = 0;
	virtual void  OnDeinit() = 0;
	virtual void  OnUpdate( uint32 delta ) = 0;
	virtual void  OnUpdateFrame() = 0;
	virtual void  OnRender() = 0;
	virtual void  OnPointerUp( int32 x, int32 y ) = 0;
	virtual void  OnPointerDown( int32 x, int32 y ) = 0;
	virtual void  OnPointerMove( int32 x, int32 y ) = 0;
	virtual void  OnKeyboardChar( s3eWChar ch ) = 0;
	virtual void  OnZoom( float scale ) = 0;
	//virtual void  OnKeyboardUp( s3eKey key ) = 0;
	//virtual void  OnKeyboardDown( s3eKey key ) = 0;

	void  Run();
	void  Render();
	void  Init();
	void  Deinit();
	void  Update( uint32 delta );	

	bool  IsPaused() const;
	void  InitFramerate(int32 fps);
	void  ChangeFramerate(int32 fps);
	void  UpdateTimer();
	void  UpdateControls();

	void RegisterSingletouch();
	void UnRegisterSingletouch();

	void RegisterMultitouch();
	void UnRegisterMultitouch();

	CIwVec2 ConvertToVirtualPoints( const CIwVec2 & p );

	static const unsigned int MAX_GESTURE_POINT = 20; 
	static void MultiTouchButtonCB(s3ePointerTouchEvent* event, CApp * app);
	static void MultiTouchMotionCB(s3ePointerTouchMotionEvent* event, CApp * app);

	static void SingleTouchButtonCB(s3ePointerEvent* event, CApp * app);
	static void SingleTouchMotionCB(s3ePointerMotionEvent* event, CApp * app);

	int          GetActiveTouchesNum() const;

protected:

	struct STouch
	{
		int32 x;
		int32 y;
		bool  active; 
		int32 id;
	};

	STouch*      GetTouch(int32 id);
	void         ResetTouch();
	
	void         StartZooming();
	void         EndZooming();
	float        GetZoomingScale() const;

protected:
	bool          mIsQuit;
	bool          mIsPaused;
	int32         mMillisecondsPerLogicTick;
	int32		  mMilliseconds;			//!< total number of milliseconds elapsed since app start
	int32		  mMillisecondsPassed;	//!< number of milliseconds since last update

	STouch        mTouches[APP_MAX_TOUCHES];
	bool          mIsZooming;
	uint32        mZoomTouchIds[2];
	float         mZoomInitDelta;
	uint32        mMainPointerId;
};


_END_NAMESPACE_
#endif // _APP_H_