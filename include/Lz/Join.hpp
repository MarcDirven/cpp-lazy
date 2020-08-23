#pragma once

#include <Lz/detail/JoinIterator.hpp>


namespace lz {
    template<class Iterator>
    class Join {
    public:
        using iterator = detail::JoinIterator<Iterator>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        std::string _delimiter{};
        Iterator _begin{};
        Iterator _end{};
        typename iterator::difference_type _distance{};

    public:
        Join(const Iterator begin, const Iterator end, std::string&& delimiter):
            _delimiter(std::move(delimiter)),
            _begin(begin),
            _end(end),
            _distance(std::distance(begin, end) * 2 - 1)
        {
        }

        iterator begin() const {
            return iterator(_begin, _delimiter, true, _distance);
        }

        iterator end() const {
            return iterator(_end, _delimiter, false, _distance);
        }
    };

    template<class Iterator>
    Join<Iterator> joinrange(const Iterator begin, const Iterator end, std::string delimiter) {
        return Join<Iterator>(begin, end, std::move(delimiter));
    }

    template<class Iterable>
    auto join(Iterable&& iterable, std::string delimiter) {
        return joinrange(std::begin(iterable), std::end(iterable), std::move(delimiter));
    }
}