//=========================================================
// @file ActorMgr.h
// @auth Kota Tamaoki
// @date 2026/03/23
//=========================================================

#pragma once
#include <cstdint>

//--------------------------------------------------------------------------------------
class ActorMgr
{
public:
	ActorMgr() : mActorBaseProcID(0) {}

	void initialize() {}

	static ActorMgr* GetInstance()
	{
		if (!mInstance) {
			mInstance = new ActorMgr();
		}
		return mInstance;
	}

	uint32_t generateActorBaseProcID();

private:
	static ActorMgr* mInstance;
	uint32_t mActorBaseProcID;
};
//--------------------------------------------------------------------------------------