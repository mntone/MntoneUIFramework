#pragma once
#include "hwnd_base.hpp"
#include "dpi_scale_factor.hpp"

namespace mnfx {

using physical_unit = int32_t;
using dialog_unit = float;

const dialog_unit default_window_padding = 11;
const dialog_unit related_control_spacing = 7;
const dialog_unit unrelated_control_spacing = 11;

class window
	: public hwnd_base
{
	friend class control_base;

public:
	window();
	window(control_base* child);
	window(::std::wstring class_name);
	window(::std::wstring class_name, control_base* child);

public:
	HRESULT initialize(HINSTANCE hinstance) noexcept;
	HRESULT initialize(HINSTANCE hinstance, window const& owner) noexcept;
	virtual int32_t run() noexcept;

protected:
	virtual HRESULT register_window_class() noexcept;
	virtual LRESULT window_procedure(window_message message, WPARAM wparam, LPARAM lparam, bool& handled) noexcept;

	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept;
	virtual HRESULT arrange_override(::mnfx::rect final) noexcept;

	virtual HRESULT on_initialize() noexcept;
	virtual HRESULT on_create() noexcept { return S_OK; }
	virtual HRESULT on_reposition(::mnfx::point point) noexcept;
	virtual HRESULT on_resize(::mnfx::size size) noexcept;
	virtual HRESULT on_rearrange(::mnfx::rect rect) noexcept;
	virtual HRESULT on_dpi_changed(const RECT /*suggest*/) const noexcept { return S_OK; }

private:
	virtual HRESULT initialize(control_base const& parent) noexcept;
	HRESULT initialize(HINSTANCE hinstance, window const* owner) noexcept;

	RECT get_window_size_from_client_area() noexcept;
	HRESULT set_position() noexcept;
	HRESULT set_size() noexcept;
	HRESULT set_position_and_size() noexcept;

	HRESULT prepare_resize(LPARAM lparam) noexcept;
	HRESULT prepare_command(WPARAM wparam, LPARAM lparam, bool& handled) noexcept;
	HRESULT prepare_dpi_changed(WPARAM wparam, LPARAM lparam) noexcept;

	static LRESULT CALLBACK window_procedure_lancher(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

public:
	window const* owner() const noexcept { return owner_; }
	virtual window const& root() const noexcept { return *this; }
	HINSTANCE const& hinstance() const noexcept { return hinstance_; }
	dpi_scale_factor const& scale_factor() const noexcept { return scale_factor_; }

	physical_unit top() const noexcept { return top_; }
	physical_unit left() const noexcept { return left_; }
	physical_unit bottom() const noexcept { return top_ + static_cast<physical_unit>(std::ceil(scale_factor_.scale_y(height_))); }
	physical_unit right() const noexcept { return left_ + static_cast<physical_unit>(std::ceil(scale_factor_.scale_x(width_))); }

	physical_unit physical_height() const noexcept { return static_cast<physical_unit>(std::ceil(scale_factor_.scale_y(height_))); }
	physical_unit physical_width() const noexcept { return static_cast<physical_unit>(std::ceil(scale_factor_.scale_x(width_))); }

	void set_top(physical_unit value) noexcept;
	void set_left(physical_unit value) noexcept;

	control_base const& child() const { return *child_.get(); }
	void set_child(control_base* value) { child_.reset(value); }

private:
	window const* owner_;
	HINSTANCE hinstance_;

	physical_unit top_, left_;
	dpi_scale_factor scale_factor_;

	::std::unique_ptr<control_base> child_;
};

}