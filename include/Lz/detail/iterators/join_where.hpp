#pragma once

#ifndef LZ_JOIN_WHERE_ITERATOR_HPP
#define LZ_JOIN_WHERE_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"


namespace lz {
namespace detail {
// TODO: possible to make this bidirectional?
template<class IterA, class SA, class IterB, class SB, class SelectorA, class SelectorB, class ResultSelector>
class join_where_iterator : public iter_base<join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>,
                                             func_container_ret_type<ResultSelector, IterA, IterB>,
                                             fake_ptr_proxy<func_container_ret_type<ResultSelector, IterA, IterB>>,
                                             std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
private:
    using IterTraitsA = std::iterator_traits<IterA>;
    using IterTraitsB = std::iterator_traits<IterB>;
    using value_type_b = typename IterTraitsB::value_type;
    using RefTypeA = typename IterTraitsA::reference;

    using SelectorARetVal = decay<func_ret_type<SelectorA, RefTypeA>>;

    IterA _iter_a{};
    IterB _iter_b{};
    IterB _begin_b{};
    SA _end_a{};
    SB _end_b{};

    mutable func_container<SelectorA> _selector_a{};
    mutable func_container<SelectorB> _selector_b{};
    mutable func_container<ResultSelector> _result_selector{};

    template<class IA = IterA>
    enable_if<std::is_same<IterB, SB>::value && std::is_same<IA, SA>::value> find_next() {
        _iter_a = std::find_if(std::move(_iter_a), _end_a, [this](const value_type<IterA>& a) {
            auto&& toFind = _selector_a(a);
            _iter_b =
                std::lower_bound(std::move(_iter_b), _end_b, toFind,
                                 [this](const value_type_b& b, const SelectorARetVal& val) { return _selector_b(b) < val; });
            if (_iter_b != _end_b && !(toFind < _selector_b(*_iter_b))) {
                return true;
            }
            _iter_b = _begin_b;
            return false;
        });
    }

    template<class IA = IterA>
    enable_if<!std::is_same<IterB, SB>::value && std::is_same<IA, SA>::value> find_next() {
        _iter_a = std::find_if(std::move(_iter_a), _end_a, [this](const value_type<IterA>& a) {
            auto&& toFind = _selector_a(a);
            _iter_b =
                detail::lower_bound(std::move(_iter_b), _end_b, toFind,
                                    [this](const value_type_b& b, const SelectorARetVal& val) { return _selector_b(b) < val; });
            if (_iter_b != _end_b && !(toFind < _selector_b(*_iter_b))) {
                return true;
            }
            _iter_b = _begin_b;
            return false;
        });
    }

    template<class IA = IterA>
    enable_if<std::is_same<IterB, SB>::value && !std::is_same<IA, SA>::value> find_next() {
        _iter_a = detail::find_if(std::move(_iter_a), _end_a, [this](const value_type<IterA>& a) {
            auto&& toFind = _selector_a(a);
            _iter_b =
                std::lower_bound(std::move(_iter_b), _end_b, toFind,
                                 [this](const value_type_b& b, const SelectorARetVal& val) { return _selector_b(b) < val; });
            if (_iter_b != _end_b && !(toFind < _selector_b(*_iter_b))) {
                return true;
            }
            _iter_b = _begin_b;
            return false;
        });
    }

    template<class IA = IterA>
    enable_if<!std::is_same<IterB, SB>::value && !std::is_same<IA, SA>::value> find_next() {
        _iter_a = detail::find_if(std::move(_iter_a), _end_a, [this](const value_type<IterA>& a) {
            auto&& toFind = _selector_a(a);
            _iter_b =
                detail::lower_bound(std::move(_iter_b), _end_b, toFind,
                                    [this](const value_type_b& b, const SelectorARetVal& val) { return _selector_b(b) < val; });
            if (_iter_b != _end_b && !(toFind < _selector_b(*_iter_b))) {
                return true;
            }
            _iter_b = _begin_b;
            return false;
        });
    }

public:
    using reference = decltype(_result_selector(*_iter_a, *_iter_b));
    using value_type = decay<reference>;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    join_where_iterator(IterA iterA, SA endA, IterB iterB, SB endB, SelectorA a, SelectorB b, ResultSelector result_selector) :
        _iter_a(std::move(iterA)),
        _iter_b(iterB),
        _begin_b(std::move(iterB)),
        _end_a(std::move(endA)),
        _end_b(std::move(endB)),
        _selector_a(std::move(a)),
        _selector_b(std::move(b)),
        _result_selector(std::move(result_selector)) {
        find_next();
    }

    constexpr join_where_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return _result_selector(*_iter_a, *_iter_b);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iter_b;
        find_next();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const join_where_iterator& b) const noexcept {
        return _iter_a == b._iter_a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _iter_a == _end_a;
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_JOIN_WHERE_ITERATOR_HPP
