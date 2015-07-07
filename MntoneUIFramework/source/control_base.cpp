#include "pch.hpp"
#include "control_base.hpp"

#include "win32helper.hpp"
#include "window.hpp"

using namespace std;
using namespace mnfx;

bool control_base::invalid_measure_message_queued = false;
bool control_base::invalid_arrange_message_queued = false;

control_base::control_base() noexcept
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

	desired_size_ = desired;
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

		if (final.y != final_rect_.y || final.x != final_rect_.x)
		{
			if (final.height != final_rect_.height || final.width != final_rect_.width)
			{
				on_rearrange(final);
			}
			else
			{
				on_reposition(final.point());
			}
		}
		else if (final.height != final_rect_.height || final.width != final_rect_.width)
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

HRESULT control_base::set_enable(bool value) noexcept
{
	if (enable_ == value) return S_OK;
	enable_ = move(value);
	return on_enable(enable_);
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

HRESULT control_base::set_font(mnfx::font* value) noexcept
{
	if (value != nullptr && !value->initialized())
	{
		value->register_scale_factor(root().scale_factor_);
	}

	auto old_value = font_;
	font_ = make_shared<mnfx::font>();
	font_.reset(value);
	return set_font_internal(old_value, font_);
}

HRESULT control_base::set_font_internal(shared_ptr<mnfx::font> old_value, shared_ptr<mnfx::font> new_value) noexcept
{
	if (font_ != old_value) return S_OK;
	return on_font_change(old_value.get(), new_value.get());
}