#pragma once

#include <iterator>
#include <random>
#include <limits>

#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
    static std::seed_seq& getSeed() {
        static std::random_device rd;
        static std::seed_seq seed{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
        return seed;
    }

    static std::mt19937& createRandomEngine() {
        static std::mt19937 instance(getSeed());
        return instance;
    }

    template<class Arithmetic, class Distribution>
    struct RandomIteratorHelper {
        static std::random_device rd;
        static std::seed_seq seed;
        static std::mt19937& randomEngine;
        mutable Distribution distribution{};
        bool isWhileTrueLoop;

        RandomIteratorHelper(Arithmetic min, Arithmetic max, size_t amount) :
            distribution(min, max),
            isWhileTrueLoop(amount == std::numeric_limits<size_t>::max()) {
        }

        Arithmetic rand() const {
            return distribution(randomEngine);
        }
    };

    template<class Arithmetic, class Distribution>
    std::mt19937& RandomIteratorHelper<Arithmetic, Distribution>::randomEngine = createRandomEngine();

    template<class Arithmetic, class Distribution>
    class RandomIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = size_t;
        using pointer = detail::FakePointerProxy<Arithmetic>;
        using reference = value_type;

    private:
        size_t _current{};
        const RandomIteratorHelper<Arithmetic, Distribution>* _randomIteratorHelper;

    public:
        explicit RandomIterator(const size_t current, const RandomIteratorHelper<Arithmetic, Distribution>* helper) :
            _current(current),
            _randomIteratorHelper(helper) {
        }

        value_type operator*() const {
            return _randomIteratorHelper->rand();
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        RandomIterator& operator++() {
            if (!_randomIteratorHelper->isWhileTrueLoop) {
                ++_current;
            }
            return *this;
        }

        RandomIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        RandomIterator& operator--() {
            if (!_randomIteratorHelper->isWhileTrueLoop) {
                --_current;
            }
            return *this;
        }

        RandomIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        RandomIterator& operator+=(const difference_type offset) {
            if (!_randomIteratorHelper->isWhileTrueLoop) {
                _current += offset;
            }
            return *this;
        }

        RandomIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        RandomIterator& operator-=(const difference_type offset) {
            if (!_randomIteratorHelper->isWhileTrueLoop) {
                _current -= offset;
            }
            return *this;
        }

        RandomIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const RandomIterator& other) const {
            return _current - other._current;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const RandomIterator& other) const {
            return _current != other._current;
        }

        bool operator==(const RandomIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const RandomIterator& other) const {
            return _current < other._current;
        }

        bool operator>(const RandomIterator& other) const {
            return other < *this;
        }

        bool operator<=(const RandomIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const RandomIterator& other) const {
            return !(*this < other);
        }
    };
}}