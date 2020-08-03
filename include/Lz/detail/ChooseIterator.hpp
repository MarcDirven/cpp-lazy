#pragma once

#include <iterator>
#include <functional>
#include <algorithm>


namespace lz { namespace detail {
    template<class Iterator, class Function>
    struct ChooseIteratorHelper {
        using FunctionParamType = decltype(*std::declval<Iterator>());
        using Pair = decltype(std::declval<Function>()(std::declval<FunctionParamType>()));
        using FunctionReturnValuePairSecond = typename Pair::second_type;

        static_assert(std::is_same<decltype(std::declval<Pair>().first), bool>::value,
            "the function must return a std::pair<bool, T>");

        std::function<Pair(FunctionParamType)> function{};
        mutable FunctionReturnValuePairSecond current{};
    };

    template<class Iterator, class Function>
    class ChooseIterator {
        Iterator _iterator{};
        Iterator _end;
        const ChooseIteratorHelper<Iterator, Function>* _iterHelper;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = typename ChooseIteratorHelper<Iterator, Function>::FunctionReturnValuePairSecond;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = value_type&;
        using pointer = value_type*;

    private:
        void findNext(const size_t offset = 0) {
            _iterator = std::find_if(
                std::next(_iterator, offset), _end,
                [this](typename ChooseIteratorHelper<Iterator, Function>::FunctionParamType value) {
                    auto result = _iterHelper->function(value);
                    if (result.first) {
                        _iterHelper->current = std::move(result.second);
                        return true;
                    }
                    return false;
                }
            );
        }

    public:
        ChooseIterator() :
            _iterHelper(ChooseIteratorHelper<Iterator, Function>{}) {}

        explicit ChooseIterator(const Iterator begin, const Iterator end,
                                const ChooseIteratorHelper<Iterator, Function>* helper) :
            _iterator(begin),
            _end(end),
            _iterHelper(helper) {
            findNext();
        }

        reference operator*() const {
            return _iterHelper->current;
        }

        pointer operator->() const {
            return &_iterHelper->current;
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