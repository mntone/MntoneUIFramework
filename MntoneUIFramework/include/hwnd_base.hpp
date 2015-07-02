#pragma once
#include "control_base.hpp"

namespace mnfx {

class hwnd_base
	: public control_base
{
public:
	hwnd_base(::std::wstring class_name);
	virtual ~hwnd_base();

public:
	virtual HRESULT initialize(control_base const& parent) noexcept;

	bool is_self(HWND const hwnd) const noexcept { return hwnd == hwnd_; }

protected:
	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept;
	virtual HRESULT arrange_override(::mnfx::rect final) noexcept;

	virtual HRESULT on_initialize() noexcept;
	virtual HRESULT on_enable(bool enable) noexcept final;
	virtual HRESULT on_reposition(::mnfx::point point) noexcept;
	virtual HRESULT on_resize(::mnfx::size size) noexcept;
	virtual HRESULT on_rearrange(::mnfx::rect rect) noexcept;
	virtual HRESULT on_command(HWND target, WORD id, WORD notify_code, bool& handled) noexcept;

public:
	virtual HWND const& hwnd() const noexcept { return hwnd_; }
	::std::wstring const& class_name() const noexcept { return class_name_; }

	virtual ::std::wstring const& text() const noexcept { return text_; }
	void set_text(::std::wstring value) noexcept;

	window_style style() const noexcept { return style_; }
	void set_style(window_style value) noexcept;

	extended_window_style exstyle() const noexcept { return exstyle_; }
	void set_exstyle(extended_window_style value) noexcept;

protected:
	HWND hwnd_;
	::std::wstring class_name_;
	mutable std::wstring text_;
	window_style style_;
	extended_window_style exstyle_;

private:
	static NONCLIENTMETRICSW non_client_metrics_;
	static HFONT gui_font_;
};

}