#pragma once

#include <iterator>
#include <Lz/detail/LzTools.hpp>



namespace lz { namespace internal {
	// ReSharper disable once CppUnnamedNamespaceInHeaderFile
	namespace {
        template<class Tuple, std::size_t I, class = void>
        struct PlusPlus {
            void operator()(Tuple& iterators, const Tuple& end) const {
                if (std::get<I>(iterators) != std::get<I>(end)) {
                    ++std::get<I>(iterators);
                }
                else {
                    PlusPlus<Tuple, I + 1>()(iterators, end);
                }
            }
        };


        template<class Tuple, std::size_t I>
        struct PlusPlus<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value>> {
            void operator()(const Tuple& /*iterators*/, const Tuple& /*end*/) const {
            }
        };


        template<class Tuple, std::size_t I, class = void>
        struct NotEqual {
            bool operator()(const Tuple& iterators, const Tuple& end) const {
                if (std::get<I>(iterators) != std::get<I>(end)) {
                    return std::get<I>(iterators) != std::get<I>(end);
                }
                return NotEqual<Tuple, I + 1>()(iterators, end);
            }
        };


        template<class Tuple, std::size_t I>
        struct NotEqual<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
            bool operator()(const Tuple& iterators, const Tuple& end) const {
                return std::get<I>(iterators) != std::get<I>(end);
            }
        };


        template<class Tuple, std::size_t I, class = void>
        struct Deref {
            auto operator()(const Tuple& iterators, const Tuple& end) const -> decltype(*std::get<I>(iterators)) {
                if (std::get<I>(iterators) != std::get<I>(end)) {
                    return *std::get<I>(iterators);
                }
                return Deref<Tuple, I + 1>()(iterators, end);
            }
        };


        template<class Tuple, std::size_t I>
        struct Deref<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
            auto operator()(const Tuple& iterators, const Tuple&) const -> decltype(*std::get<I>(iterators)) {
                return *std::get<I>(iterators);
            }
        };


        template<class Tuple, std::size_t I>
        struct MinusMinus {
            void operator()(Tuple& iterators, Tuple& end) const {
                if (std::get<I>(iterators) == std::get<I>(end)) {
                    --std::get<I>(iterators);
                }
                else {
                    MinusMinus<Tuple, I - 1>()(iterators, end);
                }
            }
        };


        template<class Tuple>
        struct MinusMinus<Tuple, 0> {
            void operator()(Tuple& iterators, Tuple&) const {
                --std::get<0>(iterators);
            }
        };


        template<class Tuple, std::size_t I>
        struct MinIs {
            template<class DifferenceType>
            void operator()(Tuple& iterators, const Tuple& begin, const Tuple& end, const DifferenceType offset) const {
                using TupElem = TupleElement<I, Tuple>;

                const TupElem current = std::get<I>(iterators);
                const TupElem currentBegin = std::get<I>(begin);

                // Current is begin, move on to next iterator
                if (current == currentBegin) {
                    MinIs<Tuple, I - 1>()(iterators, begin, end, offset);
                }
                else {
                    const auto distance = static_cast<DifferenceType>(std::distance(std::get<I>(begin), std::get<I>(end)));
                    if (distance <= offset) {
                        std::get<I>(iterators) = std::get<I>(begin);
                        MinIs<Tuple, I - 1>()(iterators, begin, end, distance == 0 ? 1 : offset - distance);
                    }
                    else {
                        std::get<I>(iterators) = std::prev(std::get<I>(iterators), offset);
                    }
                }
            }
        };


        template<class Tuple>
        struct MinIs<Tuple, 0> {
            template<class DifferenceType>
            void operator()(Tuple& iterators, const Tuple& begin, const Tuple& /*end*/, const DifferenceType offset) const {
                using TupElem = TupleElement<0, Tuple>;

                TupElem& current = std::get<0>(iterators);
                const TupElem currentBegin = std::get<0>(begin);

                // first iterator is at indexOf begin, and distance bigger than 0
                if (std::distance(currentBegin, current) < offset) {
                    throw std::out_of_range(LZ_FILE_LINE ": cannot access elements before begin");
                }
                current = std::prev(current, offset);
            }
        };


        template<class Tuple, std::size_t I, class = void>
        struct PlusIs {
            template<class DifferenceType>
            void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) const {
                using TupElem = TupleElement<I, Tuple>;

                TupElem& currentIterator = std::get<I>(iterators);
                const TupElem currentEnd = std::get<I>(end);
                auto distance = static_cast<DifferenceType>(std::distance(currentIterator, currentEnd));

                if (distance > offset) {
                    currentIterator = std::next(currentIterator, offset);
                }
                else {
                    currentIterator = currentEnd;
                    PlusIs<Tuple, I + 1>()(iterators, end, offset - distance);
                }
            }
        };


        template<class Tuple, std::size_t I>
        struct PlusIs<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
            template<class DifferenceType>
            void operator()(Tuple& /*iterators*/, const Tuple& /*end*/, const DifferenceType /*offset*/) const {
            }
        };
    } // anonymous namespace


    template<LZ_CONCEPT_ITERATOR... Iterators>
    class ConcatenateIterator {
        using IterTuple = std::tuple<Iterators...>;
        IterTuple _iterators{};
        IterTuple _begin{};
        IterTuple _end{};

        using FirstTupleIterator = std::iterator_traits<TupleElement<0, IterTuple>>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator1>::value_type;
        using difference_type = typename std::iterator_traits<Iterator1>::difference_type;
        using reference = typename std::iterator_traits<Iterator1>::reference;
        using pointer = typename std::iterator_traits<Iterator1>::pointer;

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