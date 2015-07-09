#pragma once
#include "hwnd_base.hpp"
#include "collections.hpp"

namespace mnfx {

class combo_box_base
	: public hwnd_base
{
protected:
	combo_box_base() noexcept;

	virtual HRESULT on_initialize() noexcept;
	virtual HRESULT on_command(WORD id, WORD notify_code, bool& handled) noexcept;

private:
	void register_handler() noexcept;

	HRESULT items_change_callback(observable_vector<::std::wstring> const& sender, vector_change_event_args<::std::wstring> args) noexcept;
	void insert(vector_change_event_args<::std::wstring> args, HRESULT& hr) noexcept;

public:
	observable_vector<::std::wstring>& items() noexcept { return *items_.get(); }

	typed_event_handler<combo_box_base, value_change_event_args<::std::wstring>> const& select() const noexcept { return select_; }

private:
	size_t items_id_;
	::std::shared_ptr<observable_vector<::std::wstring>> items_;
	::std::wstring select_value_;

	typed_event_handler<combo_box_base, value_change_event_args<::std::wstring>> select_;
};

class combo_box final
	: public combo_box_base
{
public:
	combo_box() noexcept
		: combo_box_base()
	{
		set_style(style() | window_style::combo_box_dropdown);
	}
};

class dropdown_box final
	: public combo_box_base
{
public:
	dropdown_box() noexcept
		: combo_box_base()
	{
		set_style(style() | window_style::combo_box_dropdownlist);
	}
};

}