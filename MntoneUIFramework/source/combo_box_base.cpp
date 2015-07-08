#include "pch.hpp"
#include "combo_box_base.hpp"

#include "win32helper.hpp"
#include "collections_impl.hpp"

using namespace std;
using namespace mnfx;

combo_box_base::combo_box_base() noexcept
	: hwnd_base(L"COMBOBOX")
	, items_id_(0)
{
	items_.reset(new observable_vector_impl<wstring>());
	register_handler();
}

HRESULT combo_box_base::on_initialize() noexcept
{
	HRESULT hr = S_OK;
	vector_change_event_args<wstring> args;
	args.action = vector_change_action::insert;
	args.begin.reset(items_->begin());
	args.count = items_->count();
	insert(move(args), hr);
	return hr;
}

HRESULT combo_box_base::on_command(WORD id, WORD notify_code, bool& handled) noexcept
{
	HRESULT hr = S_OK;

	auto const nc = static_cast<combo_box_notify_code>(notify_code);
	switch (nc)
	{
	case combo_box_notify_code::selection_change:
		select_value_ = win32::get_selected_text(hwnd());
		break;
	}
	return hr;
}

void combo_box_base::register_handler() noexcept
{
	size_t id = 0;
	items_->change().add(bind(&combo_box_base::items_change_callback, this, placeholders::_1, placeholders::_2), id);
	items_id_ = id;
}

HRESULT combo_box_base::items_change_callback(observable_vector<wstring> const& /*sender*/, vector_change_event_args<wstring> args) noexcept
{
	HRESULT hr = S_OK;
	switch (args.action)
	{
	case vector_change_action::clear:
		win32::clear_to_combo_box(hwnd());
		break;

	case vector_change_action::insert:
		insert(move(args), hr);
		break;
	}
	return hr;
}

void combo_box_base::insert(vector_change_event_args<wstring> args, HRESULT& /*hr*/) noexcept
{
	unique_ptr<const_iterator<wstring>> end;
	end.reset(items_->end());
	if (args.begin->position() + args.count == end->position())
	{
		unique_ptr<const_iterator<wstring>> itr;
		itr.reset(args.begin->clone());
		while (args.count-- != 0)
		{
			win32::push_item_to_combo_box(hwnd(), itr->data());
			itr->next();
		}
	}
	else
	{
		unique_ptr<const_iterator<wstring>> begin;
		begin.reset(items_->begin());
		unique_ptr<const_iterator<wstring>> itr;
		itr.reset(args.begin->clone());
		auto pos = itr->position() - begin->position();
		while (args.count-- != 0)
		{
			win32::insert_item_to_combo_box(hwnd(), pos++, itr->data());
			itr->next();
		}
	}
}