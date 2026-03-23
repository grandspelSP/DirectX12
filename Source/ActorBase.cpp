//=========================================================
// @file ActorBase.cpp
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#include "ActorBase.h"
#include "ComponentBase.h"
#include "ActorMgr.h"

//--------------------------------------------------------------------------------------
ActorBase::ActorBase()
{
	mBaseProcID = ActorMgr::GetInstance()->generateActorBaseProcID();
}
//--------------------------------------------------------------------------------------
void ActorBase::enter()
{
	for (auto* com : mComponentList) {
		com->enter();
	}
}
//--------------------------------------------------------------------------------------
void ActorBase::update()
{
	for (auto* com : mComponentList) {
		com->update();
	}
}
//--------------------------------------------------------------------------------------
void ActorBase::draw()
{
	for (auto* com : mComponentList) {
#if DEBUG
		com->debugDraw();
#endif
		com->draw();
	}
}
//--------------------------------------------------------------------------------------
void ActorBase::leave()
{
	for (auto* com : mComponentList) {
		com->leave();
		delete com;
		com = nullptr;
	}
}
//--------------------------------------------------------------------------------------