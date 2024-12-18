#pragma once

#ifndef LZ_DETAIL_COMMON_ITERATOR_HPP
#define LZ_DETAIL_COMMON_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/variant.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {

template<class Iterator, class S>
class common_iterator : public iter_base<common_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                         diff_type<Iterator>, iter_cat_t<Iterator>> {
    variant<Iterator, S> _data;

    using traits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename traits::iterator_category;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    common_iterator() = default;

    common_iterator(const Iterator& iter) : _data(iter) {
    }

    common_iterator(const S& sent) : _data(sent) {
    }

    common_iterator& operator=(const Iterator& iter) {
        _data = iter;
        return *this;
    }

    common_iterator& operator=(const S& sent) {
        _data = sent;
        return *this;
    }

    reference dereference() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot get pointer from a sentinel");
        return **iter;
    }

    fake_ptr_proxy<reference> arrow() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot get pointer from a sentinel");
        return fake_ptr_proxy<decltype(**this)>(**iter);
    }

    void increment() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot increment a sentinel");
        ++(*iter);
    }

    void decrement() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot decrement a sentinel");
        --*iter;
    }

    bool eq(const common_iterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get;
        using std::get_if;
#endif

        auto&& lhs_iter = get_if<Iterator>(&_data);
        auto&& rhs_iter = get_if<Iterator>(&rhs._data);

        if (lhs_iter && rhs_iter) {
            return *lhs_iter == *rhs_iter;
        }
        if (lhs_iter /* && !rhs_iter */) {
            return *lhs_iter == get<S>(rhs._data);
        }
        if (rhs_iter /* && !lhs_iter */) {
            return get<S>(_data) == *rhs_iter;
        }
        return true;
    }

    void plus_is(const difference_type n) {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot increment sentinel");
        *iter += n;
    }

    difference_type difference(const common_iterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get;
        using std::get_if;
#endif

        auto&& lhs_iter = get_if<Iterator>(&_data);
        auto&& rhs_iter = get_if<Iterator>(&rhs._data);
        LZ_ASSERT(lhs_iter && rhs_iter, "Cannot get difference between a sentinel and an iterator");
        return *lhs_iter - *rhs_iter;
    }
};
} // namespace detail
} // namespace lz

#endif