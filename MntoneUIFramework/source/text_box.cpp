#include "pch.hpp"
#include "text_box.hpp"

#include "window.hpp"

using namespace std;
using namespace mnfx;

HRESULT text_box::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	auto& scale_factor = root().scale_factor();
	RECT expected = { 0, 0, static_cast<LONG>(ceil(scale_factor.scale_x(available.width))), static_cast<LONG>(ceil(scale_factor.scale_y(available.height))) };
	UINT format = DT_LEFT | DT_CALCRECT;
	if (multiline_) format |= DT_WORDBREAK;
	HDC hdc = GetDC(hwnd());
	DrawTextW(hdc, text_.c_str(), text_.length(), &expected, format);
	ReleaseDC(hwnd(), hdc);

	desired.height = scale_factor.scale_inverse_y<dialog_unit>(static_cast<dialog_unit>(expected.bottom));
	desired.width = scale_factor.scale_inverse_x<dialog_unit>(static_cast<dialog_unit>(expected.right));
	return S_OK;
}

HRESULT text_box::arrange_override(rect& final) noexcept
{
	auto& scale_factor = root().scale_factor();
	RECT expected = { 0, 0, static_cast<LONG>(ceil(scale_factor.scale_x(final.width))), static_cast<LONG>(ceil(scale_factor.scale_y(final.height))) };
	UINT format = DT_LEFT | DT_CALCRECT;
	if (multiline_) format |= DT_WORDBREAK;
	HDC hdc = GetDC(hwnd());
	DrawTextW(hdc, text_.c_str(), text_.length(), &expected, format);
	ReleaseDC(hwnd(), hdc);

	final.height = max(final.height, scale_factor.scale_inverse_y<dialog_unit>(static_cast<dialog_unit>(expected.bottom)));
	final.width = max(final.width, scale_factor.scale_inverse_x<dialog_unit>(static_cast<dialog_unit>(expected.right)));
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

void text_box::set_multiline(bool value) noexcept
{
	multiline_ = move(value);

	auto current_style = style();
	if (multiline_)
	{
		current_style |= window_style::edit_multiline | window_style::edit_want_return;
		current_style &= ~window_style::edit_auto_horizontal_scroll;
	}
	else
	{
		current_style &= ~(window_style::edit_multiline | window_style::edit_want_return);
		current_style |= window_style::edit_auto_horizontal_scroll;
	}
	set_style(current_style);
}

void text_box::set_readonly(bool value) noexcept
{
	readonly_ = move(value);

	auto current_style = style();
	if (readonly_)current_style |= window_style::edit_readonly;
	else current_style &= ~window_style::edit_readonly;
	set_style(current_style);
}