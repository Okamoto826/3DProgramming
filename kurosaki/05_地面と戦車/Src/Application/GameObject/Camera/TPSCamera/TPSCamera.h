﻿#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera() { Init(); }
	~TPSCamera()			override	{}

	void Init()				override;
	void Update()			override;

private:

	bool m_free = false;
};