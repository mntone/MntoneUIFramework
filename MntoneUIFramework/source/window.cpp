#include "pch.hpp"
#include "window.hpp"

using namespace std;
using namespace mnfx;

auto const default_class_name = L"MntoneUIFrameworkWindow";

window::window()
	: window(default_class_name)
{ }

window::window(control_base* child)
	: window(default_class_name)
{
	child_.reset(child);
}

window::window(wstring class_name)
	: hwnd_base(class_name)
	, owner_(nullptr)
	, hinstance_(nullptr)
	, top_(48)
	, left_(48)
	, resizing_(false)
	, border_(false)
{
	set_height(360);
	set_width(640);
	set_style(window_style::overlapped_window | window_style::visible);
	set_exstyle(extended_window_style::control_parent);

	non_client_metrics_ = { sizeof(NONCLIENTMETRICSW) };
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &non_client_metrics_, 0);
}

window::window(wstring class_name, control_base* child)
	: window(class_name)
{
	child_.reset(child);
}

HRESULT window::initialize(HINSTANCE hinstance) noexcept
{
	return initialize(move(hinstance), nullptr);
}

HRESULT window::initialize(HINSTANCE hinstance, window const& owner) noexcept
{
	return initialize(move(hinstance), &owner);
}

HRESULT window::initialize(HINSTANCE hinstance, window const* owner) noexcept
{
	HRESULT hr = S_OK;

	if (isnan(height_) || isnan(width_)) return E_INVALIDARG;

	hinstance_ = std::move(hinstance);
	owner_ = owner;

	hr = register_window_class();
	if (FAILED(hr)) return hr;

	CreateWindowExW(
		static_cast<DWORD>(exstyle()),
		class_name_.c_str(),
		text_.c_str(),
		static_cast<DWORD>(style()),
		0, 0, 0, 0,
		owner_ ? owner_->hwnd() : nullptr,
		nullptr,
		hinstance_,
		this);
	if (!hwnd()) return E_FAIL;

	BOOL b = UpdateWindow(hwnd());
	if (b == 0) return HRESULT_FROM_WIN32(GetLastError());

	hr = on_initialize();
	if (FAILED(hr)) return hr;

	non_client_metrics_.lfMessageFont.lfHeight = scale_factor_.scale_inverse_y(non_client_metrics_.lfMessageFont.lfHeight);
	non_client_metrics_.lfMessageFont.lfWidth = scale_factor_.scale_inverse_y(non_client_metrics_.lfMessageFont.lfWidth);
	hr = set_font(new(nothrow) mnfx::font(non_client_metrics_.lfMessageFont));
	if (FAILED(hr)) return hr;

	hr = set_position_and_size();
	if (FAILED(hr)) return hr;

	initialized_ = true;
	hr = invalidate_arrange();
	return hr;
}

HRESULT window::initialize(control_base const& /*parent*/) noexcept
{
	return E_UNEXPECTED;
}

int32_t window::run() noexcept
{
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!IsDialogMessageW(hwnd(), &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return static_cast<int32_t>(msg.wParam);
}

HRESULT window::on_initialize() noexcept
{
	if (child_ == nullptr) return S_OK;
	return child_->initialize(*this);
}

HRESULT window::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	if (child_ == nullptr)
	{
		desired = available;
		return S_OK;
	}

	HRESULT hr = child_->measure(available);
	desired = available;
	return hr;
}

HRESULT window::arrange_override(rect& final) noexcept
{
	if (child_ == nullptr) return S_OK;
	return child_->arrange(final);
}

LRESULT window::window_procedure(window_message message, WPARAM wparam, LPARAM lparam, bool& handled) noexcept
{
	HRESULT hr = S_OK;
	LRESULT lr = 0;

	if ((UINT)message == WM_INVALIDMEASURE)
	{
		measure(mnfx::size(height_, width_));
		invalid_measure_message_queued = false;
		arrange(mnfx::rect(0, 0, height_, width_));
		invalid_arrange_message_queued = false;
		handled = true;
		return 0;
	}
	if ((UINT)message == WM_INVALIDARRANGE)
	{
		hr = arrange(mnfx::rect(0, 0, height_, width_));
		invalid_arrange_message_queued = false;
		handled = true;
		return 0;
	}

	switch (message)
	{
	case window_message::move:
		hr = prepare_move(lparam);
		handled = true;
		break;

	case window_message::size:
		hr = prepare_resize(lparam);
		handled = true;
		break;

	case window_message::nc_hittest:
		hr = prepare_nc_hittest(lparam, lr);
		handled = true;
		break;

	case window_message::command:
		hr = prepare_command(wparam, lparam, handled);
		break;

	case window_message::create:
		hr = on_create();
		handled = true;
		break;

	case window_message::destroy:
		PostQuitMessage(0);
		handled = true;
		break;

#if( WINVER >= 0x0601 )
	case window_message::dpi_changed:
		hr = prepare_dpi_changed(wparam, lparam);
		handled = true;
		break;
#endif

	case window_message::enter_size_move:
		hr = on_enter_size_move();
		break;

	case window_message::exit_size_move:
		hr = on_exit_size_move();
		break;
	}

	return lr;
}

HRESULT window::register_window_class() noexcept
{
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
	wcex.style = /*CS_HREDRAW | CS_VREDRAW |*/ CS_DBLCLKS;
	wcex.lpfnWndProc = &window_procedure_lancher;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(window*);
	wcex.hInstance = hinstance_;
	wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = class_name_.c_str();
	wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	return RegisterClassExW(&wcex) ? S_OK : E_FAIL;
}

RECT window::get_window_size_from_client_area() noexcept
{
	RECT rect =
	{
		left_,
		top_,
		left_ + static_cast<physical_unit>(ceil(scale_factor().scale_x(width_))),
		top_ + static_cast<physical_unit>(ceil(scale_factor().scale_y(height_)))
	};
	AdjustWindowRectEx(&rect, static_cast<DWORD>(style()), FALSE, static_cast<DWORD>(exstyle()));
	return rect;
}

HRESULT window::set_position() noexcept
{
	RECT const& rect = get_window_size_from_client_area();
	return SetWindowPos(hwnd(), nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOSIZE) != 0 ? S_OK : E_FAIL;
}

HRESULT window::set_size() noexcept
{
	RECT const& rect = get_window_size_from_client_area();
	return SetWindowPos(hwnd(), nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE) != 0 ? S_OK : E_FAIL;
}

HRESULT window::set_position_and_size() noexcept
{
	RECT const& rect = get_window_size_from_client_area();
	return SetWindowPos(hwnd(), nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER) != 0 ? S_OK : E_FAIL;
}

HRESULT window::on_reposition(mnfx::point /*point*/) noexcept
{
	return S_OK;
}

HRESULT window::on_resize(mnfx::size /*size*/) noexcept
{
	return S_OK;
}

HRESULT window::on_rearrange(mnfx::rect /*rect*/) noexcept
{
	return S_OK;
}

HRESULT window::on_command_internal(HWND target, WORD id, WORD notify_code, bool& handled, bool& traversed) noexcept
{
	if (child_ == nullptr) return S_OK;
	return child_->on_command_internal(target, id, notify_code, handled, traversed);
}


HRESULT window::on_dpi_changed(RECT /*suggest*/) noexcept
{
	if (initialized_) return invalidate_measure();
	return S_OK;
}

HRESULT window::prepare_move(LPARAM lparam) noexcept
{
	if (initialized_)
	{
		top_ = HIWORD(lparam);
		left_ = LOWORD(lparam);
	}
	return S_OK;
}

HRESULT window::prepare_resize(LPARAM lparam) noexcept
{
	if (initialized_)
	{
		uint16_t height = scale_factor().scale_inverse_y(HIWORD(lparam));
		uint16_t width = scale_factor().scale_inverse_x(LOWORD(lparam));
		height_ = height;
		width_ = width;
		return invalidate_measure();
	}
	return S_OK;
}

HRESULT window::prepare_command(WPARAM wparam, LPARAM lparam, bool& handled) noexcept
{
	WORD id = LOWORD(wparam);
	WORD notify_code = HIWORD(wparam);
	HWND target = reinterpret_cast<HWND>(lparam);
	bool traversed = false;
	return on_command_internal(target, id, notify_code, handled, traversed);
}

HRESULT window::prepare_dpi_changed(WPARAM wparam, LPARAM lparam) noexcept
{
	uint16_t const& ydpi = HIWORD(wparam);
	uint16_t const& xdpi = LOWORD(wparam);

	//if (resizing_ && !border_)
	//{
	//	dpi_change_reserved_ = true;
	//	new_dpi_ = make_pair(ydpi, xdpi);
	//	return S_OK;
	//}

	auto scale = scale_factor_.set_dpi(ydpi, xdpi);
	if (resizing_ && border_)
	{
		height_ /= scale.first;
		width_ /= scale.second;
	}

	HRESULT hr = set_position_and_size();
	if (FAILED(hr)) return hr;

	RECT const& suggest = *reinterpret_cast<LPRECT>(lparam);
	return on_dpi_changed(suggest);
}

HRESULT window::prepare_nc_hittest(LPARAM lparam, LRESULT& lr) noexcept
{
	lr = DefWindowProcW(hwnd(), static_cast<UINT>(window_message::nc_hittest), 0, lparam);

	auto ht = static_cast<hit_test>(lr);
	switch (ht)
	{
	case hit_test::left:
	case hit_test::right:
	case hit_test::top:
	case hit_test::top_left:
	case hit_test::top_right:
	case hit_test::bottom:
	case hit_test::bottom_left:
	case hit_test::bottom_right:
		border_ = true;
		break;
	}
	return S_OK;
}

HRESULT window::on_enter_size_move() noexcept
{
	resizing_ = true;
	return S_OK;
}

HRESULT window::on_exit_size_move() noexcept
{
	resizing_ = false;
	border_ = false;
	//dpi_change_reserved_ = false;
	return S_OK;
}

LRESULT CALLBACK window::window_procedure_lancher(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lparam);
		window* wnd = static_cast<window*>(create_struct->lpCreateParams);
		if (wnd == nullptr)
		{
			exit(1);
		}

		wnd->hwnd_ = hwnd;
		wnd->scale_factor_.initialize(MonitorFromWindow(wnd->hwnd_, MONITOR_DEFAULTTONEAREST));
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));

		bool handled = false;
		LRESULT lresult = wnd->window_procedure(static_cast<window_message>(message), wparam, lparam, handled);
		if (!handled)
		{
			return DefWindowProcW(hwnd, message, wparam, lparam);
		}
		return lresult;
	}

	window* wnd = reinterpret_cast<window*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
	if (wnd == nullptr)
	{
		return DefWindowProcW(hwnd, message, wparam, lparam);
	}

	bool handled = false;
	LRESULT lresult = wnd->window_procedure(static_cast<window_message>(message), wparam, lparam, handled);
	if (!handled)
	{
		return DefWindowProcW(hwnd, message, wparam, lparam);
	}
	return lresult;
}

void window::set_top(physical_unit value) noexcept
{
	top_ = value;
	if (initialized_) on_reposition(mnfx::point());
}

void window::set_left(physical_unit value) noexcept
{
	left_ = value;
	if (initialized_) on_reposition(mnfx::point());
}

HRESULT window::set_font_internal(shared_ptr<mnfx::font> old_value, shared_ptr<mnfx::font> new_value) noexcept
{
	HRESULT hr = on_font_change(old_value.get(), new_value.get());
	if (FAILED(hr)) return hr;

	if (child_ == nullptr) return S_OK;
	return child_->set_font_internal(old_value, new_value);
}