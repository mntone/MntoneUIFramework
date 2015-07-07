#pragma once
#include <ShellScalingApi.h>
#include "os_version.hpp"
#include "module.hpp"
#include "parameters.hpp"

namespace mnfx {

class dpi_scale_factor final
{
public:
	using dpi_unit = float;

	dpi_scale_factor() noexcept
		: enabled_(false)
	{
#if( WINVER >= 0x0603 )
		if (os_version_ >= os_version::eight_one)
		{
			HRESULT hr = S_OK;
			module_ = ::std::make_shared<module>(L"shcore.dll", hr);
			if (SUCCEEDED(hr))
			{
				get_process_dpi_awareness_ = reinterpret_cast<decltype(GetProcessDpiAwareness)*>(GetProcAddress(module_->handle(), "GetProcessDpiAwareness"));
				if (get_process_dpi_awareness_ != nullptr)
				{
					PROCESS_DPI_AWARENESS awareness = PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE;
					hr = get_process_dpi_awareness_(nullptr, &awareness);
					if (SUCCEEDED(hr) && awareness != PROCESS_DPI_AWARENESS::PROCESS_DPI_UNAWARE)
					{
						get_dpi_for_monitor_ = reinterpret_cast<decltype(GetDpiForMonitor)*>(GetProcAddress(module_->handle(), "GetDpiForMonitor"));
						if (get_dpi_for_monitor_ != nullptr)
						{
							enabled_ = true;
							return;
						}
					}
				}
				get_process_dpi_awareness_ = nullptr;
			}
			module_ = nullptr;
		}
#endif
	}

	void initialize(HMONITOR hmonitor) noexcept
	{
		if (enabled_)
		{
			UINT y, x;
			get_dpi_for_monitor_(hmonitor, MONITOR_DPI_TYPE::MDT_DEFAULT, &x, &y);
			ydpi_ = static_cast<dpi_unit>(y) / 96;
			xdpi_ = static_cast<dpi_unit>(x) / 96;
		}
		else
		{
			HDC hdc = GetDC(nullptr);
			ydpi_ = static_cast<dpi_unit>(GetDeviceCaps(hdc, LOGPIXELSY)) / 96;
			xdpi_ = static_cast<dpi_unit>(GetDeviceCaps(hdc, LOGPIXELSX)) / 96;
			ReleaseDC(nullptr, hdc);
		}
	}

	::std::pair<dpi_unit, dpi_unit> try_dpi(uint16_t y, uint16_t x) const noexcept
	{
		dpi_unit new_ydpi, new_xdpi;
		new_ydpi = static_cast<dpi_unit>(y) / 96;
		new_xdpi = static_cast<dpi_unit>(x) / 96;

		float scale_factor_y, scale_factor_x;
		scale_factor_y = new_ydpi / ydpi_;
		scale_factor_x = new_xdpi / xdpi_;

		return ::std::make_pair(scale_factor_y, scale_factor_x);
	}

	::std::pair<dpi_unit, dpi_unit> set_dpi(uint16_t y, uint16_t x) noexcept
	{
		dpi_unit new_ydpi, new_xdpi;
		new_ydpi = static_cast<dpi_unit>(y) / 96;
		new_xdpi = static_cast<dpi_unit>(x) / 96;

		float scale_factor_y, scale_factor_x;
		scale_factor_y = new_ydpi / ydpi_;
		scale_factor_x = new_xdpi / xdpi_;

		dpi_unit old_ydpi, old_xdpi;
		old_ydpi = ydpi_;
		old_xdpi = xdpi_;
		ydpi_ = new_ydpi;
		xdpi_ = new_xdpi;

		dpi_change_.invoke(*this, event_args());
		return ::std::make_pair(scale_factor_y, scale_factor_x);
	}

	template<typename T>
	T scale_y(T virtual_y) const noexcept
	{
		return static_cast<T>(ydpi_ * static_cast<dpi_unit>(virtual_y));
	}

	template<typename T>
	T scale_x(T virtual_x) const noexcept
	{
		return static_cast<T>(xdpi_ * static_cast<dpi_unit>(virtual_x));
	}

	template<typename T>
	T scale_inverse_y(T physical_y) const noexcept
	{
		return static_cast<T>(static_cast<dpi_unit>(physical_y) / ydpi_);
	}

	template<typename T>
	T scale_inverse_x(T physical_x) const noexcept
	{
		return static_cast<T>(static_cast<dpi_unit>(physical_x) / xdpi_);
	}

public:
	typed_event_handler<dpi_scale_factor, event_args> const& dpi_change() const noexcept { return dpi_change_; }

private:
	dpi_unit ydpi_, xdpi_;
	os_version os_version_;

	bool enabled_;
	::std::shared_ptr<module> module_;
#if( WINVER >= 0x0603 )
	decltype(GetProcessDpiAwareness)* get_process_dpi_awareness_;
	decltype(GetDpiForMonitor)* get_dpi_for_monitor_;
#endif

	typed_event_handler<dpi_scale_factor, event_args> dpi_change_;
};

}