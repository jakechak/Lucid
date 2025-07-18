#pragma once

#include <jni.h>

class Gui
{
private:
	jclass font_class;

	jfieldID font_field;

	jmethodID DrawString;

	jobject font_renderer;

public:
	Gui();
	void Render();
};