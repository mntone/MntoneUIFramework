#include "pch.hpp"
#include "static_panel.hpp"

using namespace mnfx;

HRESULT static_panel::set_horizontal_alignment(mnfx::horizontal_alignment value) noexcept
{
	auto current_style = style() & ~(window_style::static_left | window_style::static_center | window_style::static_right);
	switch (value)
	{
	case horizontal_alignment::left:
		set_style(current_style | window_style::static_left);
		break;

	case horizontal_alignment::center:
		set_style(current_style | window_style::static_center);
		break;

	case horizontal_alignment::right:
		set_style(current_style | window_style::static_right);
		break;

	default: return S_FALSE;
	}
	return S_OK;
}