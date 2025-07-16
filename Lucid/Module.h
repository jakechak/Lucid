#pragma once

#include <string>
#include <Windows.h>

class Module
{
private:
	bool key_was_down;

protected:
	bool toggled;
	DWORD keybind;
	std::string name;

public:
	Module(const std::string& name, DWORD keybind) : name(name), keybind(keybind), key_was_down(false), toggled(false) {}

	bool IsToggled() const { return toggled; }
	void Toggle() { toggled = !toggled; }

	DWORD GetKeybind() const { return keybind; }

	virtual void OnUpdate() { }

	void ListenForKeyPress()
	{
		bool key_is_down = GetAsyncKeyState(keybind) & 0x8000;

		if (!key_is_down && key_was_down)
			Toggle();

		key_was_down = key_is_down;
	}
};