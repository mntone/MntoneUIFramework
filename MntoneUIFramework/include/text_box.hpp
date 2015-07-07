#pragma once
#include "hwnd_base.hpp"

namespace mnfx {

class text_box final
	: public hwnd_base
{
public:
	text_box()
		: hwnd_base(L"EDIT")
		, multiline_(true)
		, readonly_(false)
	{
		set_width(100);
		set_style(style()| window_style::tab_stop | window_style::edit_left | window_style::edit_multiline | window_style::edit_want_return | window_style::border);
	}

	text_box(::std::wstring text)
		: text_box()
	{
		set_text(text);
	}

	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept;
	virtual HRESULT arrange_override(::mnfx::rect& final) noexcept;

public:
	::std::wstring const& text() const noexcept;

	bool multiline() const noexcept { return multiline_; }
	HRESULT set_multiline(bool value) noexcept;

	bool readonly() const noexcept { return readonly_; }
	HRESULT set_readonly(bool value) noexcept;

private:
	bool multiline_, readonly_;
};

}