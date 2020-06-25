#pragma once


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class MapIterator {
    private:
        Iterator _iterator{};
        Function _function{};

    public:
        using value_type = decltype(_function(*_iterator));
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = FakePointerProxy<reference>;

        MapIterator(Iterator iterator, Function function) :
            _iterator(iterator),
            _function(function) {
        }

        reference operator*() const {
            return _function(*_iterator);
        }

        FakePointerProxy<reference> operator->() {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        MapIterator& operator++() {
            ++_iterator;
            return *this;
        }

        MapIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
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

        difference_type operator-(const MapIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) {
            return *(*this + offset);
        }

        bool operator==(const MapIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const MapIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator<(const MapIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const MapIterator& other) const {
            return other < *this;
        }

        bool operator<=(const MapIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const MapIterator& other) const {
            return !(*this < other);
        }
    };
}}
