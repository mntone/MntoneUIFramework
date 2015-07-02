#include "pch.hpp"
#include "layouts.hpp"

using namespace std;
using namespace mnfx;

HRESULT panel::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	for (auto&& child : children_)
	{
		if (child == nullptr) continue;

		mnfx::size child_desired;
		HRESULT hr = child->measure(available);
		if (FAILED(hr)) return hr;

		desired.height = max(desired.height, child_desired.height);
		desired.width = max(desired.width, child_desired.width);
	}
	return S_OK;
}

HRESULT panel::arrange_override(rect& final) noexcept
{
	for (auto&& child : children_)
	{
		HRESULT hr = child->arrange(final);
		if (FAILED(hr)) return hr;
	}
	return S_OK;
}

HRESULT panel::on_initialize() noexcept
{
	for (auto&& child : children_)
	{
		HRESULT hr = child->initialize(*this);
		if (FAILED(hr)) return hr;
	}
	return S_OK;
}

HRESULT panel::on_enable(bool enable) noexcept
{
	HRESULT error_hr = S_OK;
	for (auto&& child : children_)
	{
		HRESULT hr = child->on_enable(enable);
		if (FAILED(hr)) error_hr = hr;
	}
	return error_hr;
}

HRESULT panel::on_command_internal(HWND target, WORD id, WORD notify_code, bool& handled, bool& traversed) noexcept
{
	HRESULT hr = S_OK;
	for (auto&& child : children_)
	{
		hr = child->on_command_internal(target, id, notify_code, handled, traversed);
		if (FAILED(hr)) return hr;
		if (traversed) return hr;
	}
	return S_OK;
}


HRESULT grid::measure_override(mnfx::size available, mnfx::size& desired) noexcept
{
	auto const row_size = row_definition_.size();
	auto const column_size = column_definition_.size();
	if (row_size == 1 && column_size == 1)
	{
		for (auto&& child : children_)
		{
			if (child == nullptr) continue;

			mnfx::size child_desired;
			HRESULT hr = child->measure(available);
			if (FAILED(hr)) return hr;

			desired.height = max(desired.height, child_desired.height);
			desired.width = max(desired.width, child_desired.width);
		}
	}
	else
	{
		auto const get_pixel_length = [](dialog_unit s, grid_length g) -> dialog_unit { return s + (g.auto_() || g.star() ? 0 : g.length); };
		auto const get_star_length = [](dialog_unit s, grid_length g) -> dialog_unit { return s + (g.auto_() ? 1 : (g.star() ? g.length : 0)); };
		auto const get_length = [](grid_length g, dialog_unit pixel_per_star) -> dialog_unit { return g.auto_() ? pixel_per_star : (g.star() ? pixel_per_star * g.length : g.length); };
		dialog_unit const row_pixel_sum = accumulate(row_definition_.cbegin(), row_definition_.cend(), 0.0, get_pixel_length);
		dialog_unit const row_star_sum = accumulate(row_definition_.cbegin(), row_definition_.cend(), 0.0, get_star_length);
		dialog_unit const column_pixel_sum = std::accumulate(column_definition_.cbegin(), column_definition_.cend(), 0.0, get_pixel_length);
		dialog_unit const column_star_sum = std::accumulate(column_definition_.cbegin(), column_definition_.cend(), 0.0, get_star_length);

		dialog_unit row_pixel_for_star = available.height - row_pixel_sum;
		dialog_unit row_pixel_per_star = row_pixel_for_star / row_star_sum;
		dialog_unit column_pixel_for_star = available.width - column_star_sum;
		dialog_unit column_pixel_per_star = column_pixel_for_star / column_star_sum;

		if (!grid_info_valid()) reset_grid_info();
		for (auto&& child : children_)
		{
			if (child == nullptr) continue;

			auto const& itr = related_.find(child.get());
			if (itr == related_.cend()) continue;

			auto const related = itr->second;
			auto child_row_definition = related.first < row_size ? row_definition_[related.first] : grid_length();
			auto child_column_definition = related.second < column_size ? column_definition_[related.second] : grid_length();

			mnfx::size child_available(
				max<dialog_unit>(0, get_length(child_row_definition, row_pixel_per_star)),
				max<dialog_unit>(0, get_length(child_column_definition, column_pixel_per_star)));
			mnfx::size child_desired;
			HRESULT hr = child->measure(available);
			if (FAILED(hr)) return hr;

			mnfx::size* current_desired;
			hr = get_target_desired_size(related.first, related.second, current_desired);
			if (FAILED(hr)) return hr;

			current_desired->height = max(current_desired->height, child_desired.height);
			current_desired->width = max(current_desired->width, child_desired.width);
		}

		for (size_t c = 0; c < column_size; ++c)
		{
			for (size_t r = 0; r < row_size; ++r)
			{
				auto info = grid_info_[r * column_size + c];
				desired.height = max(desired.height, info.height);
			}
		}
		for (size_t r = 0; r < row_size; ++r)
		{
			for (size_t c = 0; c < column_size; ++c)
			{
				auto info = grid_info_[r * column_size + c];
				desired.width = max(desired.width, info.width);
			}
		}
	}
	return S_OK;
}


void grid::reset_grid_info() noexcept
{
	grid_info_.clear();

	auto const size = row_definition_.size() * column_definition_.size();
	grid_info_.resize(size);
	for (auto&& i : grid_info_)
	{
		i = { 0, 0 };
	}
}

HRESULT grid::get_target_desired_size(size_t row, size_t column, mnfx::size*& desired_size) noexcept
{
	if (row > row_definition_.size()) return E_INVALIDARG;
	if (column > column_definition_.size()) return E_INVALIDARG;

	desired_size = &grid_info_[row * column_definition_.size() + column];
	return S_OK;
}

bool grid::grid_info_valid() const noexcept
{
	return grid_info_.size() == (row_definition_.size() * column_definition_.size());
}

HRESULT grid::arrange_override(rect& final) noexcept
{
	HRESULT hr = S_OK;
	if (!grid_info_valid()) hr = measure(final.size());
	if (FAILED(hr)) return hr;

	auto const row_size = row_definition_.size();
	auto const column_size = column_definition_.size();
	if (row_size == 1 && column_size == 1)
	{
		for (auto&& child : children_)
		{
			hr = child->arrange(final);
			if (FAILED(hr)) return hr;
		}
	}
	else
	{
		auto const get_pixel_length = [](dialog_unit s, grid_length g) -> dialog_unit { return s + (g.auto_() || g.star() ? 0 : g.length); };
		auto const get_star_length = [](dialog_unit s, grid_length g) -> dialog_unit { return s + (g.auto_() ? 1 : (g.star() ? g.length : 0)); };
		auto const get_length = [](grid_length g, dialog_unit pixel_per_star) -> dialog_unit { return g.auto_() ? pixel_per_star : (g.star() ? pixel_per_star * g.length : g.length); };
		dialog_unit const row_pixel_sum = accumulate(row_definition_.cbegin(), row_definition_.cend(), 0.0, get_pixel_length);
		dialog_unit const row_star_sum = accumulate(row_definition_.cbegin(), row_definition_.cend(), 0.0, get_star_length);
		dialog_unit const column_pixel_sum = std::accumulate(column_definition_.cbegin(), column_definition_.cend(), 0.0, get_pixel_length);
		dialog_unit const column_star_sum = std::accumulate(column_definition_.cbegin(), column_definition_.cend(), 0.0, get_star_length);

		dialog_unit row_pixel_for_star = final.height - row_pixel_sum;
		dialog_unit row_pixel_per_star = row_pixel_for_star / row_star_sum;
		dialog_unit column_pixel_for_star = final.width - column_star_sum;
		dialog_unit column_pixel_per_star = column_pixel_for_star / column_star_sum;

		dialog_unit cy = final.y, cx = final.x;
		for (size_t r = 0; r < row_size; ++r)
		{
			cx = final.x;

			auto const rd = row_definition_[r];
			auto const rl = max<dialog_unit>(0, get_length(rd, row_pixel_per_star));
			for (size_t c = 0; c < column_size; ++c)
			{
				auto const cd = column_definition_[c];
				auto const cl = max<dialog_unit>(0, get_length(cd, column_pixel_per_star));

				auto getter = [r, c](pair<control_base*, ::std::pair<size_t, size_t>> i) { return i.second.first == r && i.second.second == c; };
				auto itr = find_if(related_.cbegin(), related_.cend(), getter);
				while (itr != related_.cend())
				{
					rect child_final(cy, cx, rl, cl);

					hr = itr->first->arrange(child_final);
					if (FAILED(hr)) return hr;

					itr = find_if(++itr, related_.cend(), getter);
				}

				cx += cl;
			}

			cy += rl;
		}
	}
	return S_OK;
}