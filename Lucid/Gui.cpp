#include "pch.h"
#include "Gui.h"
#include "Minecraft.h"

Gui::Gui()
{
	font_class = Minecraft::env->FindClass("avn");
	font_field = Minecraft::env->GetFieldID(Minecraft::mc_class, "k", "Lavn;");

	DrawString = Minecraft::env->GetMethodID(font_class, "a", "(Ljava/lang/String;III)I");

	font_renderer = Minecraft::env->GetObjectField(Minecraft::mc_inst, font_field);
}

void Gui::Test()
{
	jstring jstr = Minecraft::env->NewStringUTF("aaaa");

	Minecraft::env->CallIntMethod(font_renderer, DrawString, jstr, 5, 5, 0xFFFFFF);
}