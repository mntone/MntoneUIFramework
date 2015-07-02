#pragma once
#include "button.hpp"

namespace mnfx {

class radio_button final
	: public button
{
public:
	radio_button()
		: checked_(false)
	{
		set_style((style() | window_style::button_auto_radio_button) & ~window_style::button_push_button);
	}

	radio_button(::std::wstring text)
		: radio_button()
	{
		set_text(text);
	}

protected:
	virtual HRESULT on_initialize() noexcept
	{
		set_checked();
		return S_OK;
	}

private:
	void set_checked() noexcept;

public:
	bool checked() const noexcept { return checked_; }
	void set_checked(bool value) noexcept;

private:
	bool checked_;
};

}