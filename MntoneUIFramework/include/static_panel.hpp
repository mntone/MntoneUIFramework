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
};

}