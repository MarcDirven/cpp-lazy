#pragma once

#include <algorithm>
#include <string>
#include <array>


namespace detail
{
	class ConstSplitIterator
	{
		using StringIterator = std::string::const_iterator;

		StringIterator _delimiterBegin{};
		StringIterator _delimiterEnd{};
		StringIterator _begin{};
		StringIterator _end{};
		StringIterator _current{};

		std::string _substring;

		void find()
		{
			_current = std::find(_begin, _end, *_delimiterBegin);
			_substring = std::string(_begin, _current);
		}
		
	public:
		using reference = const std::string&;
		using pointer = const std::string*;
		using iterator_category = std::input_iterator_tag;
		using value_type = std::string;
		using difference_type = std::string::const_iterator::difference_type;
		
		ConstSplitIterator(StringIterator begin, StringIterator end, StringIterator delimiterBegin, StringIterator delimiterEnd) :
			_delimiterBegin(std::move(delimiterBegin)),
			_delimiterEnd(std::move(delimiterEnd)),
			_begin(std::move(begin)),
			_end(std::move(end))
		{
			find();
		}

		reference operator*() const
		{
			return _substring;
		}
		
		pointer operator->() const
		{
			return &_substring;
		}

		bool operator!=(const ConstSplitIterator& other) const
		{
			return _begin != other._end;
		}

		bool operator==(const ConstSplitIterator& other) const
		{
			return !(*this != other);
		}

		ConstSplitIterator& operator++()
		{
			_begin = _current == _end ? _end : _current + std::distance(_delimiterBegin, _delimiterEnd);
			find();
			return *this;
		}
		
		ConstSplitIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ConstSplitIterator& operator+=(const difference_type offset)
		{
			for (difference_type i = 0; i < offset; i++, ++*this)
			{
				if (_begin == _end)
				{
					break;
				}
			}
			return *this;
		}
		
		ConstSplitIterator operator+(const difference_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}
	};

	class SplitIterator : public ConstSplitIterator
	{
		using Iterator = std::string::iterator;

	public:
		using reference = std::string&;
		using pointer = std::string*;
		using iterator_category = std::input_iterator_tag;
		using value_type = std::string;
		using difference_type = std::string::const_iterator::difference_type;
		
		SplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd) :
			ConstSplitIterator(std::move(begin), std::move(end), std::move(delimiterBegin), std::move(delimiterEnd))
		{
		}

		reference operator*() const
		{
			return const_cast<reference>(ConstSplitIterator::operator*());
		}

		bool operator!=(const ConstSplitIterator& other) const
		{
			return ConstSplitIterator::operator!=(other);
		}

		bool operator==(const ConstSplitIterator& other) const
		{
			return ConstSplitIterator::operator==(other);
		}
		
		pointer operator->() const
		{
			return const_cast<pointer>(ConstSplitIterator::operator->());
		}
		
		SplitIterator& operator++()
		{
			ConstSplitIterator::operator++();
			return *this;
		}
		
		SplitIterator operator++(int)
		{
			auto tmp = *this;
			ConstSplitIterator::operator++();
			return tmp;
		}

		SplitIterator& operator+=(const difference_type offset)
		{
			ConstSplitIterator::operator+=(offset);
			return *this;
		}

		SplitIterator operator+(const difference_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}
	};
}

namespace lz
{
	class SplitObject
	{
		std::string _string;
		std::string _delimiter;

	public:
		using value_type = std::string;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = std::string*;
		using const_pointer = const std::string*;
		using reference = std::string&;
		using const_reference = const std::string&;

		using iterator = detail::SplitIterator;
		using const_iterator = detail::ConstSplitIterator;

		SplitObject(std::string&& string, std::string&& delimiter) :
			_string(std::move(string)),
			_delimiter(std::move(delimiter))
		{
		}

		iterator begin()
		{
			return iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
		}
		
		iterator end()
		{
			return iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
		}

		const_iterator begin() const
		{
			return const_iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
		}
		
		const_iterator end() const
		{
			return const_iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
		}

		template<template<typename, typename...> class Container, typename... Args>
		Container<std::string, Args...> to() const
		{
			return Container<std::string, Args...>(begin(), end());
		}

		std::vector<std::string> toVector() const
		{
			return to<std::vector>();
		}

		template<size_t N>
		auto toArray() const
		{
			std::array<value_type, N> container;
			detail::fillContainer(begin(), container);
			return container;
		}
	};

	inline SplitObject split(std::string string, std::string delimiter)
	{
		return SplitObject(std::move(string), std::move(delimiter));
	}
}
