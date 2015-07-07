#pragma once
#include "hwnd_base.hpp"

namespace mnfx {

class static_panel
	: public hwnd_base
{
public:
	static_panel()
		: hwnd_base(L"STATIC")
	{ }

public:
	::mnfx::horizontal_alignment horizontal_alignment() const noexcept { return horizontal_alignment_; }
	HRESULT set_horizontal_alignment(::mnfx::horizontal_alignment value) noexcept;

private:
	::mnfx::horizontal_alignment horizontal_alignment_;
};

}