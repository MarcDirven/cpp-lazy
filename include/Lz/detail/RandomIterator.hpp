#pragma once

#include <iterator>
#include <random>
#include <limits>

#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
    template<class Arithmetic, class Distribution>
    struct RandomIteratorHelper {
        mutable std::mt19937 randomEngine;
        mutable Distribution distribution{};
        mutable size_t current{};
        size_t amount{};
        bool isWhileTrueLoop;

        RandomIteratorHelper(Arithmetic min, Arithmetic max, size_t amount) :
            distribution(min, max),
            amount(amount),
            isWhileTrueLoop(amount == std::numeric_limits<size_t>::max()) {
            static std::random_device r;
            std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
            randomEngine = std::mt19937(seed);
        }

        Arithmetic createRandomNumber() const {
            return distribution(randomEngine);
        }
    };

    template<class Arithmetic, class Distribution>
    class RandomIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = size_t;
        using pointer = detail::FakePointerProxy<Arithmetic>;
        using reference = Arithmetic&;

    private:
        const RandomIteratorHelper<Arithmetic, Distribution>* _randomIteratorHelper;


    public:
        explicit RandomIterator(const RandomIteratorHelper<Arithmetic, Distribution>* helper) :
            _randomIteratorHelper(helper) {}

        value_type operator*() const {
            return _randomIteratorHelper->createRandomNumber();
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        RandomIterator& operator++() {
            if (!_randomIteratorHelper->isWhileTrueLoop) {
                ++_randomIteratorHelper->current;
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
                --_randomIteratorHelper->current;
            }
            return *this;
        }

        RandomIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        RandomIterator& operator+=(const difference_type offset) {
            _randomIteratorHelper->current += offset;
            return *this;
        }

        RandomIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        RandomIterator& operator-=(const difference_type offset) {
            _randomIteratorHelper->current -= offset;
            return *this;
        }

        RandomIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const RandomIterator& /*other*/) const {
            return _randomIteratorHelper->amount - _randomIteratorHelper->current;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const RandomIterator& /*other*/) const {
            return _randomIteratorHelper->current != _randomIteratorHelper->amount;
        }

        bool operator==(const RandomIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const RandomIterator& /*other*/) const {
            return _randomIteratorHelper->current < _randomIteratorHelper->amount;
        }

        bool operator>(const RandomIterator& other) const {
            return _randomIteratorHelper->current > _randomIteratorHelper->amount;
        }

        bool operator<=(const RandomIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const RandomIterator& other) const {
            return !(*this < other);
        }

    };
}}