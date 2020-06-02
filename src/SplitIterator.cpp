#include "SplitIterator.hpp"

#include <utility>

using namespace detail;
using namespace lz;


// ConstSplitIterator

ConstSplitIterator::ConstSplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd) :
	_delimiterBegin(std::move(delimiterBegin)),
	_delimiterEnd(std::move(delimiterEnd)),
	_begin(std::move(begin)),
	_end(std::move(end))
{
	_current = std::search(_begin, _end, _delimiterBegin, _delimiterEnd);
	_substring = std::string(_begin, _current);
}

ConstSplitIterator::reference ConstSplitIterator::operator*() const
{
	return _substring;
}

ConstSplitIterator::pointer ConstSplitIterator::operator->() const
{
	return &_substring;
}

ConstSplitIterator& ConstSplitIterator::operator++()
{
	_begin = _current == _end ? _end : _current + std::abs(_delimiterBegin - _delimiterEnd);
	_current = std::search(_begin, _end, _delimiterBegin, _delimiterEnd);
	_substring = std::string(_begin, _current);
	return *this;
}

ConstSplitIterator ConstSplitIterator::operator++(int) const
{
	auto tmp = *this;
	tmp.operator++();
	return tmp;
}

bool ConstSplitIterator::operator!=(const ConstSplitIterator& other) const
{
	return _begin != other._end;
}

ConstSplitIterator::difference_type ConstSplitIterator::operator-(const ConstSplitIterator& other) const
{
	return std::abs(_begin - other._end);
}


// SplitIterator

SplitIterator::SplitIterator(Iterator begin, Iterator end, Iterator delimiterBegin, Iterator delimiterEnd) :
	ConstSplitIterator(std::move(begin), std::move(end), std::move(delimiterBegin), std::move(delimiterEnd))
{
}

SplitIterator::reference SplitIterator::operator*() const
{
	return const_cast<reference>(ConstSplitIterator::operator*());
}

SplitIterator::pointer SplitIterator::operator->() const
{
	return const_cast<pointer>(ConstSplitIterator::operator->());
}

SplitIterator& SplitIterator::operator++()
{
	ConstSplitIterator::operator++();
	return *this;
}

SplitIterator SplitIterator::operator++(int)
{
	auto tmp = *this;
	ConstSplitIterator::operator++();
	return tmp;
}

bool SplitIterator::operator!=(const ConstSplitIterator& other) const
{
	return ConstSplitIterator::operator!=(other);
}

SplitIterator::difference_type SplitIterator::operator-(const ConstSplitIterator& other) const
{
	return ConstSplitIterator::operator-(other);
}


// SplitObject

SplitObject::SplitObject(std::string&& string, std::string&& delimiter) :
	_string(std::move(string)),
	_delimiter(std::move(delimiter))
{
}

SplitObject::const_iterator SplitObject::begin() const
{
	return const_iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
}

SplitObject::const_iterator SplitObject::end() const
{
	return const_iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
}

SplitObject::iterator SplitObject::begin()
{
	return iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
}

SplitObject::iterator SplitObject::end()
{
	return iterator(_string.begin(), _string.end(), _delimiter.begin(), _delimiter.end());
}

SplitObject lz::split(std::string string, std::string delimiter)
{
	return SplitObject(std::move(string), std::move(delimiter));
}
