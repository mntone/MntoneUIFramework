#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>
#include "win32define.hpp"

#define WM_INVALIDMEASURE (WM_APP + 1)
#define WM_INVALIDARRANGE (WM_APP + 2)

namespace mnfx {

using dialog_unit = float;

dialog_unit const default_top = 11;
dialog_unit const default_left = 11;
dialog_unit const default_height = 26;
dialog_unit const default_one_line_height = 16;
dialog_unit const default_width = 90;
dialog_unit const auto_size = ::std::numeric_limits<dialog_unit>::quiet_NaN();

struct margin final
{
	dialog_unit left, top, right, bottom;

	margin() : left(0), top(0), right(0), bottom(0) { }
	margin(dialog_unit value) : left(value), top(value), right(value), bottom(value) { }

	margin(dialog_unit left_right, dialog_unit top_bottom)
		: left(left_right), top(top_bottom), right(left_right), bottom(top_bottom)
	{ }

	margin(dialog_unit left, dialog_unit top, dialog_unit right, dialog_unit bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{ }
};

struct point final
{
	dialog_unit y, x;

	point() : y(0), x(0) { }

	point(dialog_unit y, dialog_unit x)
		: y(y), x(x)
	{ }
};

struct size final
{
	dialog_unit height, width;

	size() : height(0), width(0) { }

	size(dialog_unit height, dialog_unit width)
		: height(height), width(width)
	{ }
};

struct rect final
{
	dialog_unit y, x, height, width;

	rect() : y(0), x(0), height(0), width(0) { }

	rect(dialog_unit y, dialog_unit x, dialog_unit height, dialog_unit width)
		: y(y), x(x), height(height), width(width)
	{ }

	::mnfx::point point() const { return ::mnfx::point(y, x); }
	::mnfx::size size() const { return ::mnfx::size(height, width); }
};

class control_base;
template<typename args>
class event_handler final
{
public:
	event_handler()
		: unique_id_(1)
	{ }

	HRESULT add(::std::function<HRESULT(control_base const&, args)> callback, size_t& id) const noexcept
	{
		auto tmp = callbacks_.size();
		id = 0;
		callbacks_.emplace(unique_id_++, callback);
		id = tmp;
		return S_OK;
	}
	HRESULT remove(size_t id) const noexcept
	{
		auto itr = callbacks_.erase(find(callbacks_.cbegin(), callbacks_.cend(), id));
		return itr != callbacks_.cend() ? S_OK : S_FALSE;
	}

	HRESULT invoke(control_base const& sender, args e) noexcept
	{
		if (callbacks_.size() == 0) return S_FALSE;

		HRESULT error_hr = S_OK;
		for (auto&& callback : callbacks_)
		{
			HRESULT hr = callback.second(sender, e);
			if (FAILED(hr)) error_hr = hr;
		}
		return error_hr;
	}

private:
	mutable size_t unique_id_;
	mutable ::std::unordered_map<size_t, ::std::function<HRESULT(control_base const&, args)>> callbacks_;
};

struct event_args
{ };

}