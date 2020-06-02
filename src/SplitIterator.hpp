#pragma once

#include <algorithm>
#include <string>

namespace detail
{
	class ConstSplitIterator
	{
	public:
		using iterator_category = std::string::const_iterator::iterator_category;
		using value_type = std::string;
		using difference_type = std::string::const_iterator::difference_type;
		using pointer = const std::string*;
		using reference = const std::string&;

	private:
		using Iterator = std::string::const_iterator;

		Iterator _delimiterBegin;
		Iterator _delimiterEnd;
		Iterator _begin;
		Iterator _end;
		Iterator _current;

		std::string _substring;

	public:
		ConstSplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd);

		reference operator*() const;
		pointer operator->() const;
		ConstSplitIterator& operator++();
		ConstSplitIterator operator++(int) const;
		bool operator!=(const ConstSplitIterator& other) const;
		difference_type operator-(const ConstSplitIterator& other) const;
	};

	class SplitIterator : public ConstSplitIterator
	{
	public:
		using iterator_category = std::string::iterator::iterator_category;
		using value_type = std::string;
		using difference_type = std::string::iterator::difference_type;
		using pointer = std::string*;
		using reference = std::string&;

	private:
		using Iterator = std::string::iterator;

	public:
		SplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd);

		reference operator*() const;
		pointer operator->() const;
		SplitIterator& operator++();
		SplitIterator operator++(int);
		bool operator!=(const ConstSplitIterator& other) const;
		difference_type operator-(const ConstSplitIterator& other) const;
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

		SplitObject(std::string&& string, std::string&& delimiter);

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;
	};

	SplitObject split(std::string string, std::string delimiter);
}
