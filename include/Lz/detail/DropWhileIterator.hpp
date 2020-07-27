#pragma once


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class DropWhileIterator {
    public:
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

    private:
        Iterator _iterator{};

    public:
        DropWhileIterator(const Iterator begin, const Iterator end, const Function& function) :
            _iterator(begin != end ? std::find_if(begin, end, [function](const value_type& value) {
                return !function(value);
            }) : end) {}

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return _iterator.operator->();
        }

        DropWhileIterator& operator++() {
            ++_iterator;
            return *this;
        }

        DropWhileIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        DropWhileIterator& operator--() {
            --_iterator;
            return *this;
        }

        DropWhileIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        bool operator!=(const DropWhileIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const DropWhileIterator& other) const {
            return !(*this != other);
        }

        DropWhileIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        DropWhileIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        DropWhileIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        DropWhileIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const DropWhileIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator<(const DropWhileIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const DropWhileIterator& other) const {
            return other < *this;
        }

        bool operator<=(const DropWhileIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const DropWhileIterator& other) const {
            return !(*this < other);
        }
    };
}}