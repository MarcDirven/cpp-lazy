#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP


namespace lz { namespace internal {
template<class Iterator>
class ExcludeIterator {
	using IterTraits = std::iterator_traits<Iterator>;

public:
	using iterator_category =
		typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
	using value_type = typename IterTraits::value_type;
	using difference_type = typename IterTraits::difference_type;
	using reference = typename IterTraits::reference;
	using pointer = typename IterTraits::pointer;

private:
	Iterator _iterator{};
	Iterator _begin{};
	Iterator _end{};
	difference_type _index{};
	difference_type _from{};
	difference_type _to{};

public:
	LZ_CONSTEXPR_CXX_17 ExcludeIterator(Iterator it, Iterator begin, Iterator end, const difference_type from,
										const difference_type to) :
		_iterator(std::move(it)),
		_begin(std::move(begin)),
		_end(std::move(end)),
		_from(from),
		_to(to) {
		using std::next; using lz::next;
		if (it == begin && from == 0) {
			_iterator = next(std::move(_iterator), _to);
			_index = _to;
		}
	}

	constexpr ExcludeIterator() = default;

	constexpr reference operator*() const {
		return *_iterator;
	}

	constexpr pointer operator->() const {
		return &*_iterator;
	}

	LZ_CONSTEXPR_CXX_17 ExcludeIterator& operator++() {
		using std::next; using lz::next;
		++_iterator;
		++_index;
		if (_index == _from) {
			_iterator = next(std::move(_iterator), _to - _from);
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 ExcludeIterator operator++(int) {
		ExcludeIterator tmp(*this);
		++*this;
		return tmp;
	}

	constexpr friend bool operator==(const ExcludeIterator& a, const ExcludeIterator& b) {
		return !(a != b); // NOLINT
	}

	constexpr friend bool operator!=(const ExcludeIterator& a, const ExcludeIterator& b) {
		LZ_ASSERT(a._to == b._to && a._from == b._from, "incompatible iterator types: from and to must be equal");
		return a._iterator != b._iterator;
	}
};
}}

#endif // LZ_EXCLUDE_ITERATOR_HPP
