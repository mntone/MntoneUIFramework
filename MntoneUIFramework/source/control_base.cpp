#include "pch.hpp"
#include "control_base.hpp"
#include "window.hpp"
#include "win32helper.hpp"

using namespace std;
using namespace mnfx;

bool control_base::invalid_measure_message_queued = false;
bool control_base::invalid_arrange_message_queued = false;
NONCLIENTMETRICSW hwnd_base::non_client_metrics_;
HFONT hwnd_base::gui_font_;

control_base::control_base()
	: initialized_(false)
	, enable_(true)
	, height_(auto_size)
	, width_(auto_size)
	, measure_dirty_(true)
	, arrange_dirty_(true)
{ }

HRESULT control_base::initialize(control_base const& parent) noexcept
{
	parent_ = &parent;
	return on_initialize();
}

HRESULT control_base::measure(mnfx::size available) noexcept
{
	HRESULT hr = S_OK;

	if (isnan(available.height) || isnan(available.width)) return S_FALSE;
	if (!arrange_dirty_)
	{
		hr = invalidate_arrange();
		if (FAILED(hr)) return hr;
	}

	available.height -= margin_.top + margin_.bottom;
	available.width -= margin_.left + margin_.right;

	mnfx::size desired;
	hr = measure_override(available, desired);
	if (FAILED(hr)) return hr;
	if (isinf(desired.height)
		|| isinf(desired.width)
		|| isnan(desired.height)
		|| isnan(desired.width))
	{
		return S_FALSE;
	}

	desired_size = desired;
	measure_dirty_ = false;
	return S_OK;
}

HRESULT control_base::arrange(mnfx::rect final) noexcept
{
	HRESULT hr = S_OK;
	if (measure_dirty_)
	{
		hr = measure(final.size());
		if (FAILED(hr)) return hr;
	}
	if (arrange_dirty_)
	{
		final.y += margin_.top;
		final.x += margin_.left;
		final.height -= margin_.top + margin_.bottom;
		final.width -= margin_.left + margin_.right;

		hr = arrange_override(final);
		if (FAILED(hr)) return hr;
		arrange_dirty_ = false;

		if (final.y != final_rect.y || final.x != final_rect.x)
		{
			if (final.height != final_rect.height || final.width != final_rect.width)
			{
				on_rearrange(final);
			}
			else
			{
				on_reposition(final.point());
			}
		}
		else if (final.height != final_rect.height || final.width != final_rect.width)
		{
			on_resize(final.size());
		}
	}
	return hr;
}

HRESULT control_base::invalidate_measure() noexcept
{
	measure_dirty_ = true;
	if (!initialized()) return S_OK;
	if (invalid_measure_message_queued) return S_OK;

	invalid_measure_message_queued = true;
	return win32::send_message(root().hwnd(), static_cast<window_message>(WM_INVALIDMEASURE), 0, 0);
}

HRESULT control_base::invalidate_arrange() noexcept
{
	arrange_dirty_ = true;
	if (!initialized()) return S_OK;
	if (invalid_arrange_message_queued) return S_OK;
	if (invalid_measure_message_queued) return S_OK;

	invalid_arrange_message_queued = true;
	return win32::send_message(root().hwnd(), static_cast<window_message>(WM_INVALIDARRANGE), 0, 0);
}

void control_base::set_enable(bool value) noexcept
{
	enable_ = move(value);
	on_enable(enable_);
}

void control_base::set_margin(mnfx::margin value) noexcept
{
	margin_ = move(value);
	invalidate_measure();
}

void control_base::set_height(dialog_unit value) noexcept
{
	height_ = move(value);
	invalidate_measure();
}

void control_base::set_width(dialog_unit value) noexcept
{
	width_ = move(value);
	invalidate_measure();
}

void control_base::set_size(mnfx::size value) noexcept
{
	height_ = move(value.height);
	width_ = move(value.width);
	invalidate_measure();
}


hwnd_base::hwnd_base(::std::wstring class_name)
	: hwnd_(nullptr)
	, class_name_(class_name)
	, text_(L"")
	, style_(window_style::child | window_style::visible)
	, exstyle_()
{
	if (!gui_font_)
	{
		non_client_metrics_ = { sizeof(NONCLIENTMETRICSW) };
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &non_client_metrics_, 0);
		gui_font_ = CreateFontIndirectW(&non_client_metrics_.lfMessageFont);
	}
}

hwnd_base::~hwnd_base()
{
	if (hwnd_)
	{
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}
}

HRESULT hwnd_base::initialize(control_base const& parent) noexcept
{
	parent_ = &parent;

	hwnd_ = CreateWindowExW(
		static_cast<DWORD>(exstyle_),
		class_name_.c_str(),
		text_.c_str(),
		static_cast<DWORD>(style_),
		0,
		0,
		static_cast<int>(ceil(root().scale_factor().scale_x(width_))),
		static_cast<int>(ceil(root().scale_factor().scale_y(height_))),
		((hwnd_base*)&parent)->hwnd(), // todo
		nullptr,
		root().hinstance(),
		nullptr);
	if (!hwnd_) return E_FAIL;

	win32::set_font(hwnd_, gui_font_);

	HRESULT hr = on_initialize();
	initialized_ = true;
	return hr;
}

HRESULT hwnd_base::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	desired = available;
	return S_OK;
}

HRESULT hwnd_base::arrange_override(rect final) noexcept
{
	return S_OK;
}

HRESULT hwnd_base::on_initialize() noexcept
{
	return S_OK;
}

HRESULT hwnd_base::on_enable(bool enable) noexcept
{
	return win32::enable_window(hwnd_, enable);
}

HRESULT hwnd_base::on_reposition(::mnfx::point point) noexcept
{
	return win32::set_position(
		hwnd_,
		static_cast<int32_t>(floor(root().scale_factor().scale_y(point.y))),
		static_cast<int32_t>(floor(root().scale_factor().scale_x(point.x))));
}

HRESULT hwnd_base::on_resize(::mnfx::size size) noexcept
{
	return win32::set_size(
		hwnd_,
		static_cast<int32_t>(ceil(root().scale_factor().scale_y(size.height))),
		static_cast<int32_t>(ceil(root().scale_factor().scale_x(size.width))));
}

HRESULT hwnd_base::on_rearrange(::mnfx::rect rect) noexcept
{
	return win32::set_position_and_size(
		hwnd_,
		static_cast<int32_t>(floor(root().scale_factor().scale_y(rect.y))),
		static_cast<int32_t>(floor(root().scale_factor().scale_x(rect.x))),
		static_cast<int32_t>(ceil(root().scale_factor().scale_y(rect.height))),
		static_cast<int32_t>(ceil(root().scale_factor().scale_x(rect.width))));
}

HRESULT hwnd_base::on_command(HWND target, WORD id, WORD notify_code, bool& handled) noexcept
{
	return S_OK;
}

void hwnd_base::set_text(wstring value) noexcept
{
	text_ = move(value);
	if (initialized_) win32::set_text(hwnd_, text_);
}

void hwnd_base::set_style(window_style value) noexcept
{
	style_ = move(value);
	if (initialized_) win32::set_style(hwnd_, style_);
}

void hwnd_base::set_exstyle(extended_window_style value) noexcept
{
	exstyle_ = move(value);
	if (initialized_) win32::set_exstyle(hwnd_, exstyle_);
}