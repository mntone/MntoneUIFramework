#pragma once
#include "parameters.hpp"

namespace mnfx {

enum class vector_change_action : uint8_t { clear, insert, erase };

template<typename T>
class const_iterator
{
public:
	virtual const_iterator<T>* next() noexcept = 0;
	virtual const_iterator<T>* clone() noexcept = 0;
	virtual T const& data() const noexcept = 0;
	virtual size_t position() const noexcept = 0;
};

template<typename T>
struct vector_change_event_args final : public event_args
{
	vector_change_action action;
	::std::shared_ptr<const_iterator<T>> begin;
	size_t count;
};

template<typename T>
class iterable_view
{
public:
	using const_iterator = const_iterator<T>;

	virtual const_iterator* begin() const noexcept = 0;
	virtual const_iterator* end() const noexcept = 0;
	virtual size_t count() const noexcept = 0;
};

template<typename T>
class observable_vector
	: public iterable_view<T>
{
public:
	virtual typed_event_handler<observable_vector, vector_change_event_args<T>> const& change() const noexcept = 0;
};

}