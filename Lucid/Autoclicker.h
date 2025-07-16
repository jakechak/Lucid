#pragma once

#include "Module.h"
#include <jni.h>

class Autoclicker : public Module
{
public:
	Autoclicker();
	void OnUpdate() override;

private:
	jmethodID ClickMouse;
};