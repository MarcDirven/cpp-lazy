#pragma once


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class MapIterator {
    private:
        Iterator _iterator{};
        Function _function{};

    public:
        using value_type = decltype(_function(*_iterator));
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = value_type;

        MapIterator(Iterator iterator, Function function) :
            _iterator(iterator),
            _function(function) {
        }

        value_type operator*() const {
            return _function(*_iterator);
        }

        bool operator!=(const MapIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const MapIterator& other) const {
            return !(*this != other);
        }

        MapIterator& operator++() {
            ++_iterator;
            return *this;
        }

        MapIterator& operator--() {
            --_iterator;
            return *this;
        }

        MapIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        MapIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        MapIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        MapIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }
    };
}}