#include "game_state_play.h"
#include "game.h"

_BEGIN_NAMESPACE_

CGamePlay::CGamePlay()
{
	
}

CGamePlay::~CGamePlay()
{
	mLevel = NULL;
}

void CGamePlay::Update( uint32 delta )
{
	float dft = (float)delta / 1000.f;

	if (mLevel != NULL)
	{
		mLevel->Update(delta);
	}	
	
	CIwFVec2 pos = mFairy->GetPos();
	pos.x = pos.x + mVel*delta;
	if (pos.x > 960 || pos.x < 0)
	{
		mVel = -mVel;
	}
	mFairy->SetPos(pos);
	mDust->SetPos(mFairy->GetPos());

	float angle = mBrick->GetRot();
	mBrick->SetRot(angle + 0.001f * delta);
}

void CGamePlay::Render()
{
	if (mLevel != NULL)
	{
		mLevel->Render();
	}
	
	CRenderer::GetInst().Flush();
}

void CGamePlay::OnPointerUp( int32 x, int32 y )
{
	
}

void CGamePlay::OnPointerMove( int32 x, int32 y )
{

}

void CGamePlay::OnPointerDown( int32 x, int32 y )
{
		
}

void CGamePlay::OnZoom( float scale )
{	
}

void CGamePlay::OnKeyboardChar( s3eWChar ch )
{
		
}

void CGamePlay::OnPush()
{
	// when we enter state - do some init stuff, i.e. show/hide some gui
	LoadLevel("./levels/level.xml");

	// get animated sprite from object and start play animation
	GameObjectPtr animObj = mLevel->GetObjByName("anim_obj");
	if (animObj)
	{
		animObj->GetSprite<CAnimatedSprite>("anim")->Play();
	}

	mFairy = animObj;

	mDust = mLevel->GetObjByName("particles_obj");

	mBrick = mLevel->GetObjByName("comp_obj");

	mVel = 0.1f;
}

void CGamePlay::OnPop()
{
	// leave this state
	CRenderer::GetInst().GetCamera().Reset();

	mLevel = NULL;
}

_END_NAMESPACE_



