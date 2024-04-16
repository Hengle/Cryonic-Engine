#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif

enum Shape
{
    Square,
    Circle
};

class Collider2D : public Component {
public:
    Collider2D(GameObject* obj, int id);
    void Start() override;
    void Destroy() override;

    Shape GetShape();
    void SetTrigger(bool value);
    bool IsTrigger();

private:
    Shape shape;
    bool trigger;

#if !defined(EDITOR)
    b2FixtureDef fixtureDef;
    b2Body* body;
    b2Fixture* fixture;
#endif
};
