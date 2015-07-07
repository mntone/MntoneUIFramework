#include "pch.hpp"
#include "hwnd_base.hpp"

#include "win32helper.hpp"
#include "window.hpp"

using namespace std;
using namespace mnfx;

hwnd_base::hwnd_base(::std::wstring class_name) noexcept
	: hwnd_(nullptr)
	, class_name_(class_name)
	, text_(L"")
	, style_(window_style::child | window_style::visible)
	, exstyle_()
{ }

hwnd_base::~hwnd_base() noexcept
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
		parent.hwnd(),
		nullptr,
		root().hinstance(),
		nullptr);
	if (!hwnd_) return E_FAIL;

	if (font_ != nullptr && font_->handle() != nullptr)
	{
		win32::set_font(hwnd(), font_->handle());
	}

	HRESULT hr = on_initialize();
	initialized_ = true;
	return hr;
}

HRESULT hwnd_base::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	desired = available;
	return S_OK;
}

HRESULT hwnd_base::arrange_override(rect& /*final*/) noexcept
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

HRESULT hwnd_base::on_command_internal(HWND target, WORD id, WORD notify_code, bool& handled, bool& traversed) noexcept
{
	if (is_self(target))
	{
		HRESULT hr = on_command(id, notify_code, handled);
		traversed = true;
		return hr;
	}
	return S_OK;
}

HRESULT hwnd_base::on_font_change(::mnfx::font* old_value, ::mnfx::font* new_value) noexcept
{
	HRESULT hr = S_OK;
	if (old_value != nullptr)
	{
		hr = old_value->font_change().remove(font_id_);
	}
	if (new_value != nullptr)
	{
		size_t id = 0;
		hr = new_value->font_change().add(bind(&hwnd_base::font_change_callback, this, placeholders::_1, placeholders::_2), id);
		font_id_ = id;

		font_change_callback(*new_value, event_args());
	}
	return hr;
}

HRESULT hwnd_base::font_change_callback(::mnfx::font const& sender, event_args) noexcept
{
	win32::set_font(hwnd(), sender.handle());
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