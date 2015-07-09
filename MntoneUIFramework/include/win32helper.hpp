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

inline size_t get_selected_index(HWND target)
{
	return SendMessageW(target, static_cast<UINT>(::mnfx::window_message::combo_box_get_current_selection), 0, 0);
}

inline ::std::wstring get_selected_text(HWND target)
{
	auto index = get_selected_index(target);
	auto length = SendMessageW(target, static_cast<UINT>(::mnfx::window_message::combo_box_get_list_box_text_length), index, 0);

	::std::vector<wchar_t> buffer;
	buffer.resize(length + 1);
	send_message(target, ::mnfx::window_message::combo_box_get_list_box_text, index, reinterpret_cast<LPARAM>(buffer.data()));
	return ::std::wstring(buffer.data(), length);
}

inline HRESULT push_item_to_combo_box(HWND target, ::std::wstring text)
{
	return send_message(target, ::mnfx::window_message::combo_box_add_string, 0, reinterpret_cast<LPARAM>(text.c_str()));
}

inline HRESULT insert_item_to_combo_box(HWND target, size_t index, ::std::wstring text)
{
	return send_message(target, ::mnfx::window_message::combo_box_insert_string, index, reinterpret_cast<LPARAM>(text.c_str()));
}

inline HRESULT clear_to_combo_box(HWND target)
{
	return send_message(target, ::mnfx::window_message::combo_box_reset_content, 0, 0);
}

}