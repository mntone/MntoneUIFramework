#include "pch.hpp"
#include "radio_button.hpp"

using namespace std;
using namespace mnfx;

void radio_button::set_checked() noexcept
{
	SendMessageW(hwnd(), BM_SETCHECK, checked_ ? TRUE : FALSE, 0);
}

void radio_button::set_checked(bool value) noexcept
{
	checked_ = std::move(value);
	if (initialized()) set_checked();
}