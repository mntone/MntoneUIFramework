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

	virtual ~button() { }

protected:
	virtual HRESULT on_command(WORD id, WORD notify_code, bool& handled) noexcept;

public:
	typed_event_handler<button, event_args> const& click() const noexcept { return click_; }

private:
	typed_event_handler<button, event_args> click_;
};

}