#pragma once

#include <algorithm>
#include <string>


namespace detail
{
	class ConstSplitIterator
	{
		using StringIterator = std::string::const_iterator;

		StringIterator _delimiterBegin;
		StringIterator _delimiterEnd;
		StringIterator _begin;
		StringIterator _end;
		StringIterator _current;

		std::string _substring;

	public:
		using reference = const std::string&;
		using pointer = const std::string*;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string;
		using difference_type = std::string::const_iterator::difference_type;
		
		ConstSplitIterator(StringIterator begin, StringIterator end, StringIterator delimiterBegin, StringIterator delimiterEnd) :
			_delimiterBegin(std::move(delimiterBegin)),
			_delimiterEnd(std::move(delimiterEnd)),
			_begin(std::move(begin)),
			_end(std::move(end))
		{
			_current = std::search(_begin, _end, _delimiterBegin, _delimiterEnd);
			_substring = std::string(_begin, _current);
		}

		reference operator*() const
		{
			return _substring;
		}
		
		pointer operator->() const
		{
			return &_substring;
		}
		
		ConstSplitIterator& operator++()
		{
			_begin = _current == _end ? _end : _current + std::abs(_delimiterBegin - _delimiterEnd);
			_current = std::search(_begin, _end, _delimiterBegin, _delimiterEnd);
			_substring = std::string(_begin, _current);
			return *this;
		}
		
		ConstSplitIterator operator++(int) const
		{
			auto tmp = *this;
			tmp.operator++();
			return tmp;
		}
		
		bool operator!=(const ConstSplitIterator& other) const
		{
			return _begin != other._end;
		}
		
		difference_type operator-(const ConstSplitIterator& other) const
		{
			return std::distance(_begin, other._end);
		}

		ConstSplitIterator operator-(const difference_type other) const
		{
			return ConstSplitIterator(_begin - other, _end, _delimiterBegin, _delimiterEnd);
		}

		ConstSplitIterator operator+(const difference_type other) const
		{
			return *this - -other;
		}
	};

	class SplitIterator : public ConstSplitIterator
	{
		using Iterator = std::string::iterator;

	public:
		SplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd) :
			ConstSplitIterator(std::move(begin), std::move(end), std::move(delimiterBegin), std::move(delimiterEnd))
		{
		}

		reference operator*() const
		{
			return const_cast<reference>(ConstSplitIterator::operator*());
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
		
		bool operator!=(const ConstSplitIterator& other) const
		{
			return ConstSplitIterator::operator!=(other);
		}
		
		difference_type operator-(const ConstSplitIterator& other) const
		{
			return ConstSplitIterator::operator-(other);
		}

		ConstSplitIterator operator+(const difference_type other) const
		{
			return ConstSplitIterator::operator+(other);
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

		std::vector<std::string> toVector() const
		{
			return std::vector<std::string>(begin(), end());
		}

		template<size_t N>
		std::array<value_type, N> toArray() const
		{
			return detail::makeArray<value_type, N>(begin());
		}
	};

	inline SplitObject split(std::string string, std::string delimiter)
	{
		return SplitObject(std::move(string), std::move(delimiter));
	}
}
