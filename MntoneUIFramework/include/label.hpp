#pragma once
#include "static_panel.hpp"

namespace mnfx {

class label final
	: public static_panel
{
public:
	label() noexcept
		: static_panel()
	{ }

	label(::std::wstring text) noexcept
		: static_panel()
	{
		set_text(text);
	}
};

}