#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/View.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <cstring>

namespace lz {
namespace detail {

template<class Iterator, class S, class Iterator2, class S2>
class SplitIterator : public IterBase<SplitIterator<Iterator, S, Iterator2, S2>, View<Iterator>, FakePointerProxy<View<Iterator>>,
                                      std::ptrdiff_t, std::forward_iterator_tag, DefaultSentinel> {
    Iterator _subRangeBegin{};
    std::pair<Iterator, Iterator> _subRangeEnd{};
    Iterator2 _toSearch{};
    S _end{};
    S2 _toSearchEnd{};
    bool _trailingEmpty{ true };

    std::pair<Iterator, Iterator> search() const {
        return detail::search(_subRangeEnd.second, _end, _toSearch, _toSearchEnd);
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = View<Iterator>;
    using reference = value_type;
    using difference_type = DiffType<Iterator>;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 SplitIterator(Iterator begin, S end, Iterator2 begin2, S2 end2) :
        _subRangeBegin(std::move(begin)),
        _subRangeEnd({ _subRangeBegin, _subRangeBegin }),
        _toSearch(std::move(begin2)),
        _end(std::move(end)),
        _toSearchEnd(std::move(end2)) {
        if (_subRangeBegin != _end) {
            _subRangeEnd = search();
        }
        else {
            _trailingEmpty = false;
        }
    }

    SplitIterator() = default;

    LZ_CONSTEXPR_CXX_20 value_type dereference() const {
        return { _subRangeBegin, _subRangeEnd.first };
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_trailingEmpty && _subRangeEnd.second == _end) {
            _subRangeBegin = _subRangeEnd.first;
            _trailingEmpty = false;
            return;
        }

        if (!_trailingEmpty && _subRangeEnd.second == _end) {
            _subRangeBegin = _subRangeEnd.first = _end;
            return;
        }

        _subRangeEnd.first = _subRangeEnd.second;
        if (_subRangeEnd.first != _end) {
            _subRangeBegin = _subRangeEnd.first;
            _subRangeEnd = search();
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const SplitIterator& rhs) const noexcept {
        return _subRangeBegin == rhs._subRangeBegin && _subRangeEnd.first == rhs._subRangeEnd.first &&
               _trailingEmpty == rhs._trailingEmpty;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const {
        return _subRangeBegin == _end && !_trailingEmpty;
    }
};
} // namespace detail
} // namespace lz

#endif