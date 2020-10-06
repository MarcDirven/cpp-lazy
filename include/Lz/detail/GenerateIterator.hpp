#pragma once

#include <Lz/detail/LzTools.hpp>

#include <iterator>
#include <functional>


namespace lz { namespace internal {
    template<LZ_CONCEPT_INVOCABLE GeneratorFunc>
    class GenerateIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = decltype(std::declval<GeneratorFunc>()());
        using difference_type = size_t;
        using reference = value_type;
        using pointer = FakePointerProxy<value_type>;

    private:
        size_t _current{};
        const GenerateIteratorHelper<GeneratorFunc, value_type>* _iterHelper;

    public:
        GenerateIterator():
            _iterHelper(GenerateIteratorHelper<GeneratorFunc, value_type>{}){
        }

        GenerateIterator(const size_t start, const GenerateIteratorHelper<GeneratorFunc, value_type>* helper):
            _current(start),
            _iterHelper(helper)
        {}

        value_type operator*() const {
            return _iterHelper->generator();
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        GenerateIterator& operator++() {
            if (!_iterHelper->isWhileTrueLoop) {
                ++_current;
            }
            return *this;
        }

        GenerateIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        GenerateIterator& operator--() {
            if (!_iterHelper->isWhileTrueLoop) {
                --_current;
            }
            return *this;
        }

        GenerateIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        GenerateIterator& operator+=(const difference_type offset) {
            if (!_iterHelper->isWhileTrueLoop) {
                _current += offset;
            }
            return *this;
        }

        GenerateIterator& operator-=(const difference_type offset) {
            if (!_iterHelper->isWhileTrueLoop) {
                _current -= offset;
            }
            return *this;
        }

        GenerateIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        GenerateIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const GenerateIterator& other) const {
            return _current - other._current;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const GenerateIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const GenerateIterator& other) const {
            return _current != other._current;
        }

        bool operator<(const GenerateIterator& other) const {
            return _current < other._current;
        }

        bool operator>(const GenerateIterator& other) const {
            return other < *this;
        }

        bool operator<=(const GenerateIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const GenerateIterator& other) const {
            return !(*this < other);
        }
    };
}}