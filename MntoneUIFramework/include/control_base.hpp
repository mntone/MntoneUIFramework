#pragma once
#include "parameters.hpp"

namespace mnfx {

class window;
class control_base
{
	friend class window;
	friend class panel;

public:
	control_base();

	virtual HRESULT initialize(control_base const& parent) noexcept;
	HRESULT measure(::mnfx::size available) noexcept;
	HRESULT arrange(::mnfx::rect final) noexcept;

	HRESULT invalidate_measure() noexcept;
	HRESULT invalidate_arrange() noexcept;

protected:
	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept = 0;
	virtual HRESULT arrange_override(::mnfx::rect& final) noexcept = 0;

	virtual HRESULT on_initialize() noexcept = 0;
	virtual HRESULT on_enable(bool enable) noexcept = 0;
	virtual HRESULT on_reposition(::mnfx::point /*point*/) noexcept = 0;
	virtual HRESULT on_resize(::mnfx::size /*size*/) noexcept = 0;
	virtual HRESULT on_rearrange(::mnfx::rect /*rect*/) noexcept = 0;
	virtual HRESULT on_command(HWND target, WORD id, WORD notify_code, bool& handled) noexcept = 0;

public:
	virtual window const& root() const noexcept { return parent_->root(); }
	control_base const* parent() const noexcept { return parent_; }
	virtual HWND const& hwnd() const noexcept = 0;

	bool enable() const noexcept { return enable_; }
	void set_enable(bool value) noexcept;

	::mnfx::margin margin() const noexcept { return margin_; }
	void set_margin(::mnfx::margin value) noexcept;

	dialog_unit height() const noexcept { return height_; }
	void set_height(dialog_unit value) noexcept;

	dialog_unit width() const noexcept { return width_; }
	void set_width(dialog_unit value) noexcept;

	::mnfx::size size() const noexcept { return ::mnfx::size(height_, width_); }
	void set_size(::mnfx::size value) noexcept;

protected:
	bool initialized() const noexcept { return initialized_; }

protected:
	static bool invalid_measure_message_queued;
	static bool invalid_arrange_message_queued;

	control_base const* parent_;
	bool initialized_, enable_;
	::mnfx::margin margin_;
	dialog_unit height_, width_;

	bool measure_dirty_, arrange_dirty_;
	::mnfx::size desired_size;
	::mnfx::rect final_rect;
};

}