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
    DropWhile<Iterator, Function> dropwhilerange(const Iterator begin, const Iterator end, const Function& predicate) {
        return DropWhile<Iterator, Function>(begin, end, predicate);
    }

    template<class Iterable, class Function>
    auto dropwhile(Iterable&& iterable, const Function& predicate) -> DropWhile<decltype(std::begin(iterable)), Function> {
        return dropwhilerange(std::begin(iterable), std::end(iterable), predicate);
    }
}