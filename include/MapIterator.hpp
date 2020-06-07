#pragma once
#include <functional>
#include <type_traits>

namespace detail
{
	template<class Container, class Function, class ReturnType>
	class ConstMapIterator
	{
		using ConstIterator = typename std::decay_t<Container>::const_iterator;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename ConstIterator::value_type;
		using difference_type = typename ConstIterator::difference_type;
		using pointer = const std::remove_reference_t<ReturnType>*;
		using reference = const ReturnType;

	private:
		ConstIterator _begin{};
		ConstIterator _end{};
		Function _function{ nullptr };

	public:
		ConstMapIterator(ConstIterator begin, ConstIterator end, Function function):
			_begin(std::move(begin)),
			_end(std::move(end)),
			_function(function)
		{
		}

		reference operator*() const
		{
			return _function(_begin.operator*());
		}

		pointer operator->() const
		{
			return _function(_begin.operator->());
		}

		bool operator!=(const ConstMapIterator& other) const
		{
			return _begin != other._end;
		}

		bool operator==(const ConstMapIterator& other) const
		{
			return !(*this == other);
		}
		
		ConstMapIterator& operator++()
		{
			++_begin;
			return *this;
		}
		
		ConstMapIterator operator++(int)
		{
			auto tmp = *this;
			++this;
			return tmp;
		}
		
		ConstMapIterator& operator--()
		{
			--_begin;
			return *this;
		}
		
		ConstMapIterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}
		
		ConstMapIterator& operator+=(const difference_type offset)
		{
			_begin += offset;
			return *this;
		}
		
		ConstMapIterator operator+(const difference_type offset) const
		{
			auto* tmp = *this;
			return tmp += offset;
		}
		
		ConstMapIterator& operator-=(const difference_type offset)
		{
			_begin -= offset;
			return *this;
		}
		
		ConstMapIterator operator-(const difference_type other) const
		{
			auto tmp = *this;
			return tmp -= other;
		}
	};
	
	template<class Container, class Function, class ReturnType>
	class MapIterator : public ConstMapIterator<Container, Function, ReturnType>
	{
		using Iterator = typename std::decay_t<Container>::iterator;
		using Base = ConstMapIterator<Container, Function, ReturnType>;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = ReturnType*;
		using reference = ReturnType&;

		MapIterator(Iterator begin, Iterator end, Function function):
			ConstMapIterator<Container, Function, ReturnType>(begin, end, function)
		{
		}

		reference operator*()
		{
			return const_cast<reference>(Base::operator*());
		}

		pointer operator->()
		{
			return const_cast<reference>(Base::operator->());
		}

		bool operator!=(const MapIterator& other) const
		{
			return Base::operator!=(other);
		}

		bool operator==(const MapIterator& other) const
		{
			return Base::operator==(other);
		}

		MapIterator& operator++()
		{
			Base::operator++();
			return *this;
		}

		MapIterator operator++(int)
		{
			auto tmp = *this;
			Base::operator++();
			return tmp;
		}

		MapIterator& operator--()
		{
			Base::operator--();
			return *this;
		}

		MapIterator operator--(int)
		{
			auto tmp = *this;
			Base::operator--();
			return tmp;
		}

		MapIterator& operator+=(const difference_type offset)
		{
			Base::operator+=(offset);
			return *this;
		}

		MapIterator operator+(const difference_type offset) const
		{
			auto tmp = *this;
			tmp += offset;
			return tmp;
		}

		MapIterator& operator-=(const difference_type offset)
		{
			Base::operator-=(offset);
			return *this;
		}

		MapIterator operator-(const difference_type offset) const
		{
			auto tmp = *this;
			tmp -= offset;
			return *this;
		}
	};
}

namespace lz
{
	template<class Container, class Function, class ReturnType>
	class MapObject
	{
		using ContainerAlias = std::decay_t<Container>;
		
	public:
		using value_type = typename ContainerAlias::value_type;
		using size_type = typename ContainerAlias::size_type;
		using difference_type = typename ContainerAlias::difference_type;
		using pointer = typename ContainerAlias::pointer;
		using const_pointer = typename ContainerAlias::const_pointer;
		using reference = typename ContainerAlias::reference;
		using const_reference = typename ContainerAlias::const_reference;

		using iterator = detail::MapIterator<Container, Function, ReturnType>;
		using const_iterator = detail::ConstMapIterator<Container, Function, ReturnType>;
		
	private:
		Container&& _container{};
		Function _function{ nullptr };
		
	public:
		MapObject(Container&& container,Function function):
			_container(std::forward<Container>(container)),
			_function(function)
		{
		}

		iterator begin()
		{
			return iterator(_container.begin(), _container.end(), _function);
		}
		
		iterator end()
		{
			return iterator(_container.begin(), _container.end(), _function);
		}

		const_iterator begin() const
		{
			return const_iterator(_container.begin(), _container.end(), _function);
		}

		const_iterator end() const
		{
			return const_iterator(_container.begin(), _container.end(), _function);
		}

		template<template<class, class...> class ContainerType, class... Args>
		ContainerType<ReturnType, Args...> to() const
		{
			return ContainerType<ReturnType, Args...>(begin(), end());
		}

		std::vector<ReturnType> toVector() const
		{
			return to<std::vector>();
		}

		template<size_t N>
		std::array<ReturnType, N> toArray() const
		{
			std::array<ReturnType, N> container;
			detail::fillContainer(begin(), container);
			return container;
		}
	};

	template<class Container, class Function,
			 class ReturnType = typename std::result_of<Function(typename std::decay_t<Container>::value_type)>::type>
	MapObject<Container, Function, ReturnType> map(Container&& container, Function function)
	{
		return MapObject<Container, Function, ReturnType>(std::forward<Container>(container), function);
	}
}
