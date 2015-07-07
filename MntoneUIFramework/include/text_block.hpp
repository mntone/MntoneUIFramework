#pragma once
#include "static_panel.hpp"

namespace mnfx {

class text_block final
	: public static_panel
{
public:
	text_block()
		: static_panel()
	{
		set_style(style() | window_style::static_no_prefix);
	}
};

}