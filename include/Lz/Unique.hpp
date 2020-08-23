#pragma once

#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/UniqueIterator.hpp>


namespace lz {
    template<class Iterator>
    class Unique final : public detail::BasicIteratorView<detail::UniqueIterator<Iterator>> {
    public:
        using iterator = detail::UniqueIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        Iterator _begin{};
        Iterator _end{};

    public:
        Unique(const Iterator begin, const Iterator end):
            _begin(begin),
            _end(end)
        {
        }

        iterator begin() const {
            return iterator(_begin, _end);
        }

        iterator end() const {
            return iterator(_end, _end);
        }
    };

    template<class Iterator>
    Unique<Iterator> uniquerange(const Iterator begin, const Iterator end) {
        return Unique<Iterator>(begin, end);
    }

    template<class Iterable>
    auto unique(Iterable&& iterable) -> Unique<decltype(std::begin(iterable))> {
        return uniquerange(std::begin(iterable), std::end(iterable));
    }
}