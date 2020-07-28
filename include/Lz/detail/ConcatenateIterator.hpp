#pragma once

#include <iterator>
#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
    template<class Iterator1, class Iterator2>
    class ConcatenateIterator {
    private:
        using ValueType2 = typename std::iterator_traits<Iterator2>::value_type;
        using Pointer2 = typename std::iterator_traits<Iterator2>::pointer;
        using Reference2 = typename std::iterator_traits<Iterator2>::reference;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator1>::value_type;
        using difference_type = typename std::iterator_traits<Iterator1>::difference_type;
        using reference = typename std::iterator_traits<Iterator1>::reference;
        using pointer = typename std::iterator_traits<Iterator1>::pointer;

        static_assert(std::is_same<value_type, ValueType2>::value, "value types of the iterators do not match");
        static_assert(std::is_same<pointer, Pointer2>::value, "pointer types of the iterators do not match");
        static_assert(std::is_same<reference, Reference2>::value, "reference types of the iterators do not match");
    private:
        Iterator1 _iterator1{}, _end1{};
        Iterator2 _iterator2{}, _begin2{}, _end2{};

    public:
        ConcatenateIterator(const Iterator1 iterator1, const Iterator2 end1, const Iterator1 iterator2,
                            const Iterator2 end2) :
            _iterator1(iterator1),
            _end1(end1),
            _iterator2(iterator2),
            _begin2(iterator2),
            _end2(end2) {}

        reference operator*() const {
            if (_iterator1 != _end1) {
                return *_iterator1;
            }
            else if (_iterator2 != _end2) {
                return *_iterator2;
            }
            throw std::out_of_range("all iterators have been exhausted");
        }

        pointer operator->() const {
            if (_iterator1 != _end1) {
                return _iterator1.operator->();
            }
            else if (_iterator2 != _end2) {
                return _iterator2.operator->();
            }
            return nullptr;
        }

        ConcatenateIterator& operator++() {
            if (_iterator1 != _end1) {
                ++_iterator1;
            }
            else if (_iterator2 != _end2) {
                ++_iterator2;
            }
            return *this;
        }

        ConcatenateIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        ConcatenateIterator& operator--() {
            if (_iterator2 == _end2 && _iterator1 == _end1) {
                --_iterator2;
            }
            else if (_iterator2 != _end2 && _iterator1 != _end2) {
                --_iterator1;
            }
            return *this;
        }

        ConcatenateIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        ConcatenateIterator& operator+=(const difference_type offset) {
            auto distance = static_cast<difference_type>(std::distance(_iterator1, _end1));

            if (distance > offset) {
                _iterator1 = std::next(_iterator1, offset);
            }
            else {
                _iterator1 = _end1;
                _iterator2 = std::next(_iterator2, offset - distance);
            }
            return *this;
        }

        ConcatenateIterator& operator-=(const difference_type offset) {
            if (_iterator1 != _end1) {
                _iterator1 = std::prev(_iterator1, offset);
            }
            else {
                // Check if overlap
                auto distance = static_cast<difference_type>(std::distance(_iterator2, _end2));

                if (_iterator2 == _begin2) {
                    _iterator1 = std::prev(_iterator1, offset);
                }
                else if (distance <= offset) {
                    _iterator2 = _begin2;
                    _iterator1 = std::prev(_iterator1, offset - distance);
                }
            }
            return *this;
        }

        ConcatenateIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        ConcatenateIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ConcatenateIterator& other) const {
            return static_cast<difference_type>(std::distance(other._iterator1, _end1) +
                                                std::distance(other._iterator2, _end2));
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const ConcatenateIterator& other) const {
            return _iterator1 != other._iterator1 || _iterator2 != other._iterator2;
        }

        bool operator==(const ConcatenateIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const ConcatenateIterator& other) const {
            return other - *this > 0;
        }

        bool operator>(const ConcatenateIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ConcatenateIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ConcatenateIterator& other) const {
            return !(*this < other);
        }
    };
}}