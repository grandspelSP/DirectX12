//=========================================================
// @file ObjectBase.cpp
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#include "ObjectBase.h"
#include "DIrectXIncluder.hpp"
#include "Component/ComponentBase.h"

using namespace DirectX;

void ObjectBase::enter()
{
	for (auto* com : mComponentList) {
		com->enter();
	}
}

void ObjectBase::update()
{
	for (auto* com : mComponentList) {
		com->update();
	}
}

void ObjectBase::draw()
{
	for (auto* com : mComponentList) {
		com->draw();
	}
}

void ObjectBase::leave()
{
	for (auto* com : mComponentList) {
		com->leave();
		delete com;
		com = nullptr;
	}
}
