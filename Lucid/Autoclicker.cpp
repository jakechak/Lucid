#include "pch.h"
#include "Autoclicker.h"

#include <cstdlib>
#include <ctime>

#include "Minecraft.h"

Autoclicker::Autoclicker() : Module("Autoclicker", 0x58)
{
	ClickMouse = Minecraft::env->GetMethodID(Minecraft::mc_class, "aw", "()V");
	std::srand(time(0));
}

void Autoclicker::OnUpdate()
{
	if (!this->IsToggled())
		return;

	//Roughly 10 CPS
	if (std::rand() % 10 > 6)
		return;

	Minecraft::env->CallVoidMethod(Minecraft::mc_inst, ClickMouse);
}