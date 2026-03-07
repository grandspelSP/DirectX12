//=========================================================
// @file ActorBase.h
// @auth Kota Tamaoki
// @date 2022/12/03
//=========================================================

#pragma once
#include <d3d12.h>
#include <list>
#include <string>
#include "DirectXIncluder.hpp"
#include "Component/ComponentBase.h"

using namespace DirectX;

class ActorBase
{
public:
	ActorBase() {}
	virtual ~ActorBase() {
		this->leave();
	}

	virtual void init() {}
	virtual void enter();
	virtual void update();
	virtual void draw();
	virtual void leave();

	template <class T>
	T* getComponent() {
		T* component_searcher;
		for (auto* com : mComponentList) {
			component_searcher = dynamic_cast<T*>(com);
			if (component_searcher) {
				return component_searcher;
			}
		}

		WCHAR error_message[256];
		swprintf(error_message, 256,
			L"コンポーネントの取得に失敗。\nObject:%d\nid    :%d",
			this->mObjectInfo.name, this->mObjectInfo.id);
		MessageBox(nullptr, error_message, nullptr, MB_OK | MB_ICONERROR);
		return nullptr;
	}

	template <class T>
	T* addComponent()
	{
		T* new_component = new T();
		if (!new_component) {
			WCHAR error_message[256];
			swprintf(error_message, 256,
				L"コンポーネントの追加に失敗。\nObject:%d\nid    :%d",
				this->mObjectInfo.name, this->mObjectInfo.id);
			MessageBox(nullptr, error_message, nullptr, MB_OK | MB_ICONERROR);
			return nullptr;
		}
		new_component->setParent(this);
		mComponentList.push_back(new_component);
		new_component->enter();
		return new_component;
	}

private:
	enum class ObjectName : char
	{
		NONE,
		Player,
		Enemy,
		UI,
	};

	struct ObjectInfo
	{
		ObjectName name = ObjectName::NONE;
		unsigned int id = 0;
	};

protected:
	std::list<ComponentBase*> mComponentList;
	ObjectInfo mObjectInfo;
};
