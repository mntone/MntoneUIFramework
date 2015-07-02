#pragma once
#include "hwnd_base.hpp"

namespace mnfx {

class text_box final
	: public hwnd_base
{
public:
	text_box()
		: hwnd_base(L"EDIT")
	{
		set_width(100);
		set_style(style() | window_style::tab_stop | window_style::editcontrol_left | window_style::editcontrol_auto_horizontal_scroll | window_style::border);
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
};

}