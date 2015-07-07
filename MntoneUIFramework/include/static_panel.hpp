#pragma once
#include "hwnd_base.hpp"

namespace mnfx {

class static_panel
	: public hwnd_base
{
public:
	static_panel()
		: hwnd_base(L"STATIC")
		, horizontal_content_alignment_(::mnfx::horizontal_alignment::left)
	{
		set_style(style() | window_style::static_left);
	}

public:
	::mnfx::horizontal_alignment horizontal_content_alignment() const noexcept { return horizontal_content_alignment_; }
	HRESULT set_horizontal_content_alignment(::mnfx::horizontal_alignment value) noexcept;

private:
	::mnfx::horizontal_alignment horizontal_content_alignment_;
};

}