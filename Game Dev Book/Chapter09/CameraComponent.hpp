#pragma once

#include "Component.hpp"
#include "Math.hpp"

class CameraComponent : public Component {
public:
	CameraComponent(class Actor* owner, int updateOrder = 200);
	~CameraComponent();

protected:
	void SetViewMatrix(const Matrix4& view);
};