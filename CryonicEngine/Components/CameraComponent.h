#pragma once

#include "Component.h"
#include "../RaylibCameraWrapper.h"
#include <filesystem>

class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* obj, int id);
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;

	RaylibCamera raylibCamera;;
	static CameraComponent* main;
	bool setMain = false;
};