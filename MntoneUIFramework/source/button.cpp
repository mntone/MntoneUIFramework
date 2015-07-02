#include "pch.hpp"
#include "button.hpp"

using namespace mnfx;

HRESULT button::on_command(WORD id, WORD notify_code, bool& handled) noexcept
{
	HRESULT hr = S_OK;

	auto const nc = static_cast<button_notify_code>(notify_code);
	switch (nc)
	{
	case button_notify_code::clicked:
		hr = click_.invoke(*this, event_args());
		if (hr != S_FALSE) handled = true;
		break;
	}
	return hr;
}