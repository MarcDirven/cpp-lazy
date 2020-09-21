#pragma once

#ifndef LZ_CHOOSE_ITERATOR_HPP
#define LZ_CHOOSE_ITERATOR_HPP

#include <iterator>
#include <functional>
#include <algorithm>

#include "LzTools.hpp"


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class ChooseIterator {
        using FunctionParamType = decltype(*std::declval<Iterator>());
        using Pair = FunctionReturnType<Function, FunctionParamType>;
        using ChooseFunction = std::function<Pair(FunctionParamType)>;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = typename Pair::second_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = value_type&;
        using pointer = value_type*;

    private:
        Iterator _iterator{};
        Iterator _end{};

        mutable value_type _current{};
        const ChooseFunction* _func{};

    private:
        void findNext(const size_t offset = 0) {
            _iterator = std::find_if(std::next(_iterator, offset), _end, [this](const FunctionParamType value) {
                std::pair<bool, value_type> result = (*_func)(value);
                if (result.first) {
                    _current = result.second;
                    return true;
                }
                return false;
            });
        }

    public:
        ChooseIterator() = default;

        explicit ChooseIterator(const Iterator begin, const Iterator end, const ChooseFunction* func) :
            _iterator(begin),
            _end(end),
            _func(func) {
            findNext();
        }

        reference operator*() const {
            return _current;
        }

        pointer operator->() const {
            return &_current;
        }

        ChooseIterator& operator++() {
            findNext(1);
            return *this;
        }

        ChooseIterator operator++(int) {
            ChooseIterator tmp(*this);
            ++*this;
            return tmp;
        }

        bool operator==(const ChooseIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const ChooseIterator& /*other*/) const {
            return _iterator != _end;
        }
    };
}}

#endif