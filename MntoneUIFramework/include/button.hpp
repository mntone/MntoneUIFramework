#pragma once
#include "hwnd_base.hpp"

namespace mnfx {

class button
	: public hwnd_base
{
public:
	button()
		: hwnd_base(L"BUTTON")
	{
		set_style(style() | window_style::tab_stop | window_style::button_push_button);
	}

	button(::std::wstring text)
		: button()
	{
		set_text(text);
	}

public:
	event_handler<event_args> const& click() { return click_; }

private:
	event_handler<event_args> click_;
};

}