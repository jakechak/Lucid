#include "pch.h"
#include "Autoclicker.h"

#include <cstdlib>
#include <ctime>

#include "Client.h"

Autoclicker::Autoclicker() : Module(Client::env->NewStringUTF("Autoclicker"), 0x58)
{
	ClickMouse = Client::env->GetMethodID(Client::mc_class, "aw", "()V");
	std::srand(time(0));
}

void Autoclicker::OnUpdate()
{
	if (!this->IsToggled())
		return;

	//Roughly 10 CPS
	if (std::rand() % 10 > 6)
		return;

	Client::env->CallVoidMethod(Client::mc_inst, ClickMouse);
}