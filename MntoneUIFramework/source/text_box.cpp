#include "pch.hpp"
#include "text_box.hpp"

using namespace std;
using namespace mnfx;

wstring const& text_box::text() const noexcept
{
	if (initialized())
	{
		auto const& length = static_cast<wstring::size_type>(GetWindowTextLengthW(hwnd()));
		text_.resize(length + 1);
		GetWindowTextW(hwnd(), &text_[0], length + 1);
	}
	return text_;
}