//=========================================================
// @file ComponentBase.h
// @auth Kota Tamaoki
// @date 2022/12/04
//=========================================================

#pragma once

class ObjectBase;

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

    // コンポーネントを保持しているオブジェクトを取得
    ObjectBase* getParent() const;
    const void setParent(ObjectBase* parent);

protected:
    ObjectBase* mParentObject;
};

