#include "pch.h"
#include "Gui.h"
#include "Client.h"

Gui::Gui()
{
	font_class = Client::env->FindClass("avn");
	font_field = Client::env->GetFieldID(Client::mc_class, "k", "Lavn;");

	DrawString = Client::env->GetMethodID(font_class, "a", "(Ljava/lang/String;III)I");

	font_renderer = Client::env->GetObjectField(Client::mc_inst, font_field);
}

void Gui::Render()
{
	Client::env->CallIntMethod(font_renderer, DrawString, Client::client_name, 5, 5, 0xF3C2F3);

	int y_offset = 15;

	for (std::unique_ptr<Module>& m : Client::modules)
	{
		if (!m->IsToggled())
			continue;

		Client::env->CallIntMethod(font_renderer, DrawString, m->GetName(), 5, y_offset, 0xF3C2F3);
		y_offset += 10;
	}
}