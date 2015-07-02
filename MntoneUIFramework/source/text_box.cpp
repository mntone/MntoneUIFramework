#include "pch.hpp"
#include "text_box.hpp"

#include "window.hpp"

using namespace std;
using namespace mnfx;

HRESULT text_box::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	auto scale_factor = root().scale_factor();
	RECT expected = { 0, 0, scale_factor.scale_x(available.width), scale_factor.scale_y(available.height) };
	HDC hdc = GetDC(hwnd());
	DrawTextW(hdc, text_.c_str(), text_.length(), &expected, DT_LEFT | DT_CALCRECT);
	ReleaseDC(hwnd(), hdc);

	desired.height = scale_factor.scale_inverse_y<dialog_unit>(expected.bottom);
	desired.width = scale_factor.scale_inverse_x<dialog_unit>(expected.right);
	return S_OK;
}

HRESULT text_box::arrange_override(rect& final) noexcept
{
	auto scale_factor = root().scale_factor();
	RECT expected = { 0, 0, scale_factor.scale_x(final.width), scale_factor.scale_y(final.height) };
	HDC hdc = GetDC(hwnd());
	DrawTextW(hdc, text_.c_str(), text_.length(), &expected, DT_LEFT | DT_CALCRECT);
	ReleaseDC(hwnd(), hdc);

	final.height = max(final.height, scale_factor.scale_inverse_y<dialog_unit>(expected.bottom));
	final.width = max(final.width, scale_factor.scale_inverse_x<dialog_unit>(expected.right));
	return S_OK;
}

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