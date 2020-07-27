#pragma once

#include <Lz/detail/DropWhileIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator, class Function>
    class DropWhile final : public detail::BasicIteratorView<detail::DropWhileIterator<Iterator, Function>> {
    public:
        using iterator = detail::DropWhileIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        DropWhile(const Iterator begin, const Iterator end, const Function& predicate) :
            _begin(begin, end, predicate),
            _end(end, end, predicate) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }
    };

    template<class Iterator, class Function>
    auto dropwhilerange(const Iterator begin, const Iterator end, const Function& predicate) {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        static_assert(std::is_same<decltype(std::declval<Function>()(std::declval<ValueType>())), bool>::value,
                      "the function predicate must return a bool");
        return DropWhile<Iterator, Function>(begin, end, predicate);
    }

    template<class Iterable, class Function>
    auto dropwhile(Iterable&& iterable, const Function& predicate) {
        return dropwhilerange(std::begin(iterable), std::end(iterable), predicate);
    }
}