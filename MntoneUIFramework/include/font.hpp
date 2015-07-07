#pragma once
#include "parameters.hpp"

namespace mnfx {

class dpi_scale_factor;
class font final
{
	friend class control_base;

public:
	font() noexcept;
	font(LOGFONTW const& font) noexcept;
	~font() noexcept;

private:
	void register_scale_factor(dpi_scale_factor const& scale_factor);

	HRESULT on_change();

public:
	typed_event_handler<font, event_args> const& font_change() const noexcept { return font_change_; }

public:
	HFONT handle() const noexcept { return handle_; }

	int32_t weight() const noexcept { return font_.lfWeight; }
	HRESULT set_weight(int32_t value) noexcept;

private:
	bool initialized() const noexcept { return initialized_; }

private:
	LOGFONTW font_;
	HFONT handle_;
	size_t id_;

	bool initialized_;
	dpi_scale_factor const* scale_factor_;

	typed_event_handler<font, event_args> font_change_;
};

}