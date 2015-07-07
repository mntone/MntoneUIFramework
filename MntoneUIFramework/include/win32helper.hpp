#pragma once
#include <cstdint>
#include <string>
#include "win32define.hpp"

namespace win32 {

inline HRESULT send_message(HWND target, ::mnfx::window_message message, WPARAM wparam, LPARAM lparam)
{
	LRESULT lr = SendMessageW(target, static_cast<UINT>(message), wparam, lparam);
	return HRESULT_FROM_WIN32(static_cast<uint32_t>(lr));
}

inline HRESULT enable_window(HWND target, bool enable)
{
	BOOL b = EnableWindow(target, enable ? TRUE : FALSE);
	if (b != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_text(HWND target, ::std::wstring text)
{
	BOOL b = SetWindowTextW(target, text.c_str());
	if (b != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_position_and_size(HWND target, int32_t top, int32_t left, int32_t height, int32_t width)
{
	BOOL b = SetWindowPos(target, nullptr, left, top, width, height, SWP_NOZORDER);
	if (b != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_position(HWND target, int32_t top, int32_t left)
{
	BOOL b = SetWindowPos(target, nullptr, left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	if (b != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_size(HWND target, int32_t height, int32_t width)
{
	BOOL b = SetWindowPos(target, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
	if (b != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_font(HWND target, HFONT font, bool redraw = true)
{
	LRESULT lr = SendMessageW(target, WM_SETFONT, reinterpret_cast<WPARAM>(font), redraw ? TRUE : FALSE);
	return HRESULT_FROM_WIN32(static_cast<uint32_t>(lr));
}

inline HRESULT set_style(HWND target, ::mnfx::window_style style)
{
	auto r = SetWindowLongPtrW(target, GWL_STYLE, static_cast<DWORD>(style));
	if (r != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

inline HRESULT set_exstyle(HWND target, ::mnfx::extended_window_style exstyle)
{
	auto r = SetWindowLongPtrW(target, GWL_EXSTYLE, static_cast<DWORD>(exstyle));
	if (r != 0) return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

}