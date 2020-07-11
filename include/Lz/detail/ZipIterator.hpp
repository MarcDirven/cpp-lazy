#pragma once

#include <iterator>
#include <algorithm>


namespace lz { namespace detail {
    template<class... Containers>
    class ZipIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::tuple<std::decay_t<decltype(*std::declval<Containers>().begin())>...>;
        using difference_type = std::ptrdiff_t;
        using reference = std::tuple<decltype(*std::declval<Containers>().begin())...>;
        using pointer = FakePointerProxy<reference>;

    private:
#ifdef _MSC_VER
        std::tuple<decltype(&(*std::declval<Containers>().begin()))...> _iterators;
#else
        std::tuple<decltype(std::declval<Containers>().begin())...> _iterators;
#endif

        template<size_t... I>
        reference dereference(std::index_sequence<I...> /*is*/) const {
            return reference{*std::get<I>(_iterators)...};
        }

        template<size_t... I>
        void increment(std::index_sequence<I...> /*is*/) {
            auto expand = {(++std::get<I>(_iterators), 0)...};
            (void) expand;
        }

        template<size_t... I>
        void decrement(std::index_sequence<I...> /*is*/) {
            auto expand = {(--std::get<I>(_iterators), 0)...};
            (void) expand;
        }

        template<size_t... I>
        void plusIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) += differenceType, 0)...};
            (void) expand;
        }

        template<size_t... I>
        void minIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) -= differenceType, 0)...};
            (void) expand;
        }

        template<size_t... I>
        difference_type iteratorMin(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            return std::min({(std::get<I>(_iterators) - std::get<I>(other._iterators))...});
        }

        bool evaluateBooleanArray(const std::initializer_list<bool> booleans, const ZipIterator& other) const {
            auto end = booleans.end();
            // Check if false not in boolValues
            return std::find(booleans.begin(), end, false) == end;
        }

        template<size_t... I>
        bool lessThan(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            auto boolValues = {(std::get<I>(_iterators) < std::get<I>(other._iterators))...};
            // Check if false not in boolValues
            return evaluateBooleanArray(boolValues, other);
        }

        template<size_t... I>
        bool notEqual(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            auto boolValues = {(std::get<I>(_iterators) != std::get<I>(other._iterators))...};
            // Check if false not in boolValues
            return evaluateBooleanArray(boolValues, other);
        }

    public:
        explicit ZipIterator(decltype(_iterators) iters) :
            _iterators(iters) {
        }

        reference operator*() const {
            return dereference(std::index_sequence_for<Containers...>{});
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        ZipIterator& operator++() {
            increment(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        ZipIterator& operator--() {
            decrement(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        ZipIterator& operator+=(const difference_type offset) {
            plusIs(std::index_sequence_for<Containers...>{}, offset);
            return *this;
        }

        ZipIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        ZipIterator& operator-=(const difference_type offset) {
            minIs(std::index_sequence_for<Containers...>{}, offset);
            return *this;
        }

        ZipIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ZipIterator& other) const {
            return iteratorMin(std::index_sequence_for<Containers...>{}, other);
        }

        value_type operator[](const difference_type offset) {
            return *(*this + offset);
        }

        bool operator==(const ZipIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const ZipIterator& other) const {
            return notEqual(std::index_sequence_for<Containers...>{}, other);
        }

        bool operator<(const ZipIterator& other) const {
            return lessThan(std::index_sequence_for<Containers...>{}, other);
        }

        bool operator>(const ZipIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ZipIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ZipIterator& other) const {
            return !(*this < other);
        }
    };
}}
