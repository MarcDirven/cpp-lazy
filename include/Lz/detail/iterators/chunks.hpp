#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/iter_base.hpp"

#include <cmath>

namespace lz {
namespace detail {

template<class Iterator, class S>
/* Forward iterator */
class chunks_iterator
    : public iter_base<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                       diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _sub_range_begin{};
    Iterator _sub_range_end{};
    S _end{};
    difference_type _chunk_size{};

    LZ_CONSTEXPR_CXX_20 void next_chunk() {
        for (difference_type count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end) {
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 chunks_iterator(Iterator iterator, S end, const std::size_t chunk_size) :
        _sub_range_begin(iterator),
        _sub_range_end(std::move(iterator)),
        _end(std::move(end)),
        _chunk_size(static_cast<difference_type>(chunk_size)) {
        if (_sub_range_begin == _end) {
            return;
        }
        next_chunk();
    }

    constexpr chunks_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const chunks_iterator& rhs) const noexcept {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "incompatible iterators: different chunk sizes");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _sub_range_begin == _end;
    }
};

template<class Iterator>
class chunks_iterator<Iterator, Iterator /* Bidirectional or higher */>
    : public iter_base<chunks_iterator<Iterator, Iterator>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                       diff_type<Iterator>, iter_cat<Iterator>> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename iter_traits::iterator_category;
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _begin{};
    Iterator _sub_range_begin{};
    Iterator _sub_range_end{};
    Iterator _end{};
    difference_type _chunk_size{};

#ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_20 void next_chunk() {
        if constexpr (is_ra<Iterator>::value) {
            if (_end - _sub_range_end > _chunk_size) {
                _sub_range_end += _chunk_size;
            }
            else {
                _sub_range_end = _end;
            }
        }
        else {
            for (difference_type count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end) {
            }
        }
    }

    LZ_CONSTEXPR_CXX_20 void prevChunk() {
        if constexpr (is_ra<Iterator>::value) {
            if (_sub_range_begin - _begin > _chunk_size) {
                _sub_range_begin -= _chunk_size;
            }
            else {
                _sub_range_begin = _begin;
            }
        }
        else {
            for (difference_type count = _chunk_size; 0 < count && _sub_range_begin != _begin; count--, --_sub_range_begin) {
            }
        }
    }
#else
    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<is_ra<I>::value, void> next_chunk() {
        if (_end - _sub_range_end > _chunk_size) {
            _sub_range_end += _chunk_size;
        }
        else {
            _sub_range_end = _end;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<!is_ra<I>::value> next_chunk() {
        for (difference_type count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end) {
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<is_ra<I>::value> prevChunk() {
        if (_sub_range_begin - _begin > _chunk_size) {
            _sub_range_begin -= _chunk_size;
        }
        else {
            _sub_range_begin = _begin;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<!is_ra<I>::value> prevChunk() {
        for (difference_type count = _chunk_size; 0 < count && _sub_range_begin != _begin; count--, --_sub_range_begin) {
        }
    }
#endif // __cpp_if_constexpr

    void handle_negative_offset(const difference_type offset, difference_type total_offset) {
        auto dist = _sub_range_end - _begin;
        if (std::abs(total_offset) > dist) {
            _sub_range_begin = _begin;
            _sub_range_end = _begin + static_cast<difference_type>((_end - _begin) / _chunk_size);
            return;
        }
        if (offset != -1 || _sub_range_end != _end) {
            total_offset += _chunk_size;
            _sub_range_end += total_offset;
        }
        _sub_range_begin = total_offset >= _sub_range_end - _begin ? _begin : _sub_range_end + total_offset;
    }

    void handle_positive_offset(const difference_type total_offset) {
        if (total_offset >= _end - _sub_range_begin) {
            _sub_range_end = _end;
            _sub_range_begin += _end - _sub_range_begin;
        }
        else {
            _sub_range_begin += total_offset;
            _sub_range_end = total_offset >= _end - _sub_range_begin ? _end : _sub_range_begin + total_offset;
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 chunks_iterator(Iterator iterator, Iterator begin, Iterator end, const std::size_t chunk_size) :
        _begin(std::move(begin)),
        _sub_range_begin(iterator),
        _sub_range_end(std::move(iterator)),
        _end(std::move(end)),
        _chunk_size(static_cast<difference_type>(chunk_size)) {
        if (_sub_range_begin == _end) {
            return;
        }
        next_chunk();
    }

    constexpr chunks_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _sub_range_end = _sub_range_begin;
        prevChunk();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const chunks_iterator& rhs) const noexcept {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "incompatible iterators: different chunk sizes");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        const auto total_offset = _chunk_size * offset;
        if (total_offset < 0) {
            handle_negative_offset(offset, total_offset);
        }
        // Use > so that we distinguish 0
        else if (total_offset > 0) {
            handle_positive_offset(total_offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "incompatible iterators: different chunk sizes");
        const auto dist = _sub_range_begin - rhs._sub_range_begin;
        return dist == 0 ? 0 : round_even<difference_type>(dist, _chunk_size);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
