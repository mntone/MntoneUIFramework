#pragma once
#include "control_base.hpp"

namespace mnfx
{

class panel
	: public control_base
{
public:
	panel()
		: control_base()
	{ }

	panel(::std::initializer_list<control_base*> i)
		: control_base()
	{
		for (auto item : i)
		{
			children_.emplace_back(item);
		}
	}

protected:
	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept;
	virtual HRESULT arrange_override(::mnfx::rect& final) noexcept;

	virtual HRESULT on_initialize() noexcept final;
	virtual HRESULT on_enable(bool enable) noexcept final;
	virtual HRESULT on_reposition(::mnfx::point /*point*/) noexcept final { return S_OK; }
	virtual HRESULT on_resize(::mnfx::size /*size*/) noexcept final { return S_OK; }
	virtual HRESULT on_rearrange(::mnfx::rect /*rect*/) noexcept final { return S_OK; }
	virtual HRESULT on_command(HWND target, WORD id, WORD notify_code, bool& handled) noexcept final;

public:
	virtual HWND const& hwnd() const noexcept { return parent()->hwnd(); }

protected:
	::std::vector<::std::unique_ptr<control_base>> children_;
};

enum class grid_unit_type : uint8_t
{
	auto_ = 0,
	pixel = 1,
	star = 2,
};
struct grid_length
{
	grid_unit_type unit;
	dialog_unit length;

	grid_length() : unit(grid_unit_type::auto_), length(auto_size) { }
	grid_length(dialog_unit length) : unit(grid_unit_type::pixel), length(length) { }
	grid_length(dialog_unit length, grid_unit_type unit) : unit(unit), length(length) { }

	bool auto_() const { return unit == grid_unit_type::auto_; }
	bool star() const { return unit == grid_unit_type::star; }
};

class grid
	: public panel
{
public:
	grid() : panel() { }

	grid(
		::std::initializer_list<grid_length> row_definition,
		::std::initializer_list<grid_length> column_definition)
		: panel()
		, row_definition_(row_definition)
		, column_definition_(column_definition)
	{ }

	grid(
		::std::initializer_list<grid_length> row_definition,
		::std::initializer_list<grid_length> column_definition,
		::std::initializer_list<::std::tuple<size_t, size_t, control_base*>> i)
		: panel()
		, row_definition_(row_definition)
		, column_definition_(column_definition)
	{
		for (auto item : i)
		{
			children_.emplace_back(::std::get<2>(item));
			related_.insert({ ::std::get<2>(item), ::std::make_pair(::std::get<0>(item), ::std::get<1>(item)) });
		}
		if (row_definition_.size() == 0) row_definition_.emplace_back();
		if (column_definition_.size() == 0) column_definition_.emplace_back();
	}

protected:
	virtual HRESULT measure_override(::mnfx::size available, ::mnfx::size& desired) noexcept;
	virtual HRESULT arrange_override(::mnfx::rect& final) noexcept;

private:
	void reset_grid_info() noexcept;
	HRESULT get_target_desired_size(size_t row, size_t column, ::mnfx::size*& desiredSize) noexcept;

private:
	bool grid_info_valid() const noexcept;

protected:
	::std::vector<grid_length> row_definition_, column_definition_;
	::std::unordered_map<control_base*, ::std::pair<size_t, size_t>> related_;
	::std::vector<::mnfx::size> grid_info_;
};

};