#pragma once
#include "static_panel.hpp"

namespace mnfx {

class label final
	: public static_panel
{
public:
	label(::std::wstring text) noexcept
		: static_panel()
	{
		set_text(text);
	}
};

}