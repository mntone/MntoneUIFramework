#include "pch.hpp"
#include "font.hpp"

#include "dpi_scale_factor.hpp"

using namespace std;
using namespace mnfx;

font::font() noexcept
	: font(LOGFONTW())
{ }

font::font(LOGFONTW const& font) noexcept
	: font_(font)
	, handle_(nullptr)
	, id_(0)
	, initialized_(false)
	, scale_factor_(nullptr)
{
	on_change();
}

font::~font() noexcept
{
	if (scale_factor_ != nullptr)
	{
		if (id_ != 0)
		{
			scale_factor_->dpi_change().remove(id_);
		}
		scale_factor_ = nullptr;
	}
	if (handle_ != nullptr)
	{
		DeleteObject(handle_);
		handle_ = nullptr;
	}
}

void font::register_scale_factor(dpi_scale_factor const& scale_factor)
{
	scale_factor_ = &scale_factor;
	size_t id = 0;
	scale_factor_->dpi_change().add([&](dpi_scale_factor const& /*sender*/, event_args)
	{
		on_change();
		return S_OK;
	}, id);
	id_ = id;
	on_change();
	initialized_ = true;
}

HRESULT font::on_change()
{
	if (handle_ != nullptr)
	{
		DeleteObject(handle_);
		handle_ = nullptr;
	}
	LOGFONT copy = font_;
	if (scale_factor_ != nullptr)
	{
		copy.lfHeight = scale_factor_->scale_y(copy.lfHeight);
		copy.lfWidth = scale_factor_->scale_x(copy.lfWidth);
	}
	handle_ = CreateFontIndirectW(&copy);
	if (handle_ == nullptr) return HRESULT_FROM_WIN32(GetLastError());

	font_change_.invoke(*this, event_args());
	return S_OK;
}

HRESULT font::set_weight(int32_t value) noexcept
{
	if (font_.lfWeight == value) return S_OK;
	font_.lfWeight = value;
	return on_change();
}