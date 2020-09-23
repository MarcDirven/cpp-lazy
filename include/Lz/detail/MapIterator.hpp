#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP


namespace lz {
    template<class, class>
    class Map;

    namespace detail {

        template<class Iterator, class Function>
        class MapIterator {
        private:
            Iterator _iterator{};
            using FnParamType = decltype(*_iterator);
            using FnReturnType = detail::FunctionReturnType<Function, FnParamType>;
            Function _function;


            friend class Map<Iterator, Function>;


        public:
            using value_type = FnReturnType;
            using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
            using difference_type = std::ptrdiff_t;
            using reference = value_type;
            using pointer = FakePointerProxy<reference>;

            MapIterator(const Iterator iterator, const Function& function) :
                _iterator(iterator),
                _function(function) {
            }

            MapIterator() = default;

            MapIterator(const MapIterator&) = default;

            MapIterator(MapIterator&& other) noexcept:
                _iterator(other._iterator),
                _function(other._function)
            {}

            MapIterator& operator=(const MapIterator&) = default;

            MapIterator& operator=(MapIterator&& other) noexcept {
                _iterator = other._iterator;
                _function = std::move(other._function);
                return *this;
            }

            value_type operator*() const {
                return _function(*_iterator);
            }

            FakePointerProxy <reference> operator->() const {
                return FakePointerProxy<decltype(**this)>(**this);
            }

            MapIterator& operator++() {
                ++_iterator;
                return *this;
            }

            MapIterator operator++(int) {
                MapIterator tmp(*this);
                ++*this;
                return tmp;
            }

            MapIterator& operator--() {
                --_iterator;
                return *this;
            }

            MapIterator operator--(int) {
                MapIterator tmp(*this);
                --*this;
                return tmp;
            }

            MapIterator& operator+=(const difference_type offset) {
                _iterator += offset;
                return *this;
            }

            MapIterator& operator-=(const difference_type offset) {
                _iterator -= offset;
                return *this;
            }

            MapIterator operator+(const difference_type offset) const {
                MapIterator tmp(*this);
                tmp += offset;
                return tmp;
            }

            MapIterator operator-(const difference_type offset) const {
                MapIterator tmp(*this);
                tmp -= offset;
                return tmp;
            }

            difference_type operator-(const MapIterator& other) const {
                return _iterator - other._iterator;
            }

            reference operator[](const difference_type offset) const {
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
    }
}

#endif