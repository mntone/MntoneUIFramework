#pragma once
#include <vector>
#include "collections.hpp"

namespace mnfx {

template<typename T> class observable_vector_impl;
template<typename T>
class const_iterator_impl
{
	using observable_vector_impl = observable_vector_impl<T>;

public:
	const_iterator_impl()
		: index_(0)
	{ }

	const_iterator_impl(observable_vector_impl* parent, size_t index)
		: parent_(parent), index_(index)
	{ }

	virtual const_iterator<T>* next() noexcept
	{
		++index_;
		return reinterpret_cast<const_iterator<T>*>(this);
	}

	virtual const_iterator<T>* clone() noexcept
	{
		return reinterpret_cast<const_iterator<T>*>(new const_iterator_impl<T>(parent_, index_));
	}

	virtual T const& data() const noexcept
	{
		return parent_->at(index_);
	}

	virtual size_t position() const noexcept
	{
		return index_;
	}

	const_iterator_impl& operator+=(size_t offset)
	{
		index_ += offset;
		return *this;
	}

	const_iterator_impl operator+(size_t offset) const
	{
		auto tmp = *this;
		return (tmp += offset);
	}

	const_iterator_impl& operator-=(size_t offset)
	{
		index_ -= offset;
		return *this;
	}

	const_iterator_impl operator-(size_t offset) const
	{
		auto tmp = *this;
		return (tmp -= offset);
	}

	bool operator==(const_iterator_impl other) const
	{
		return parent_ == other.parent_ && index_ == other.index_;
	}

	bool operator!=(const_iterator_impl other) const
	{
		return parent_ != other.parent_ || index_ != other.index_;
	}

private:
	observable_vector_impl* parent_;
	size_t index_;
};

template<typename T>
class observable_vector_impl
	: public observable_vector<T>
{
	using base_type = typename ::std::vector<T>;

public:
	virtual const_iterator* begin() const noexcept
	{
		return reinterpret_cast<const_iterator*>(new const_iterator_impl<T>(const_cast<observable_vector_impl<T>*>(this), 0));
	}

	virtual const_iterator* end() const noexcept
	{
		return reinterpret_cast<const_iterator*>(new const_iterator_impl<T>(const_cast<observable_vector_impl<T>*>(this), vec_.size()));
	}

	virtual size_t count() const noexcept
	{
		return vec_.size();
	}

	T& at(size_t index) noexcept
	{
		return vec_.at(index);
	}

	T const& at(size_t index) const noexcept
	{
		return vec_.at(index);
	}

	void push_back(T value)
	{
		vec_.push_back(value);

		vector_change_event_args<T> args;
		args.action = vector_change_action::insert;
		args.begin.reset(reinterpret_cast<const_iterator*>(new const_iterator_impl<T>(const_cast<observable_vector_impl<T>*>(this), vec_.size() - 1)));
		args.count = 1;
		change_.invoke(*this, ::std::move(args));
	}

	void clear()
	{
		vec_.clear();

		vector_change_event_args<T> args;
		args.action = vector_change_action::clear;
		change_.invoke(*this, ::std::move(args));
	}

public:
	virtual typed_event_handler<observable_vector<T>, vector_change_event_args<T>> const& change() const noexcept { return change_; }

private:
	base_type vec_;

	typed_event_handler<observable_vector<T>, vector_change_event_args<T>> change_;
};

}