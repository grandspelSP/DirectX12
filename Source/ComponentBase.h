//=========================================================
// @file ComponentBase.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"
#include "ActorBase.h"

//--------------------------------------------------------------------------------------
class ComponentBase
{
public:
    ComponentBase() {
        mParentObject = nullptr;
    }
    virtual ~ComponentBase() {}

    // 定義がなければ何もしない
    virtual void enter() {}
    virtual void update() {}
    virtual void draw() {}
    virtual void leave() {}
    virtual void debugDraw() {}

    // コンポーネントを保持しているオブジェクトを取得
    ActorBase* getParent() const;
    const void setParent(ActorBase* parent);

protected:
    ActorBase* mParentObject;
};
//--------------------------------------------------------------------------------------