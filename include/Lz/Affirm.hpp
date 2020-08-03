#pragma once

#include <Lz/detail/AffirmIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Exception, class Iterator, class Function>
    class Affirm final : public detail::BasicIteratorView<detail::AffirmIterator<Exception, Iterator, Function>> {
    public:
        using iterator = detail::AffirmIterator<Exception, Iterator, Function>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        detail::AffirmIteratorHelper<Iterator, Exception, value_type> _helper{};
        iterator _begin{};
        iterator _end{};

    public:
        Affirm(const Iterator begin, const Iterator end, Exception&& exception, const Function& predicate) :
            _helper{predicate, std::forward<Exception>(exception)},
            _begin(begin, &_helper),
            _end(end, &_helper) {}

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }
    };

    /**
     * @brief Returns an Affirm view object, that can be iterated over.
     * @details This iterator checks whether predicate returns true or false. If true is returned from the function,
     * the value is returned from the iterator. If false is returned, it throws `exception`. The best way to iterate
     * over the sequence is (if using `for (... : affirm) {}` you're not able to try-catch each individual element):
     * auto beg = affirm.begin();
     * auto end = affirm.end();
     *
     * while (beg != end) {
     *      try {
     *             std::cout << *beg << '\n';
     *      }
     *      catch (const std::logic_error& err) {
     *          std::cout << err.what() << '\n';
     *      }
     *      ++beg;
     * }
     * @tparam Exception Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterator, class Function>
    Affirm<Exception, Iterator, Function>
    affirmrange(const Iterator begin, const Iterator end, Exception&& exception, const Function& predicate) {
        using FunctionReturnType = decltype(std::declval<Function>()(*begin));
        static_assert(std::is_same<FunctionReturnType, bool>::value, "function predicate must return bool");

        return Affirm<Exception, Iterator, Function>(begin, end, std::forward<Exception>(exception), predicate);
    }

    /**
     * @brief Returns an Affirm view object, that can be iterated over.
     * @details This iterator checks whether predicate returns true or false. If true is returned from the function,
     * the value is returned from the iterator. If false is returned, it throws `exception`. The best way to iterate
     * over the sequence is (if using `for (... : affirm) {}` you're not able to try-catch each individual element):
     * ```cpp
     * auto beg = affirm.begin();
     * auto end = affirm.end();
     *
     * while (beg != end) {
     *      try {
     *             std::cout << *beg << '\n';
     *      }
     *      catch (const std::logic_error& err) {
     *          std::cout << err.what() << '\n';
     *      }
     *      ++beg;
     * }
     * @tparam Exception Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterable, class Function>
    auto affirm(Iterable&& iterable, Exception&& exception, const Function& predicate) ->
    Affirm<Exception, decltype(std::begin(iterable)), Function> {
        return affirmrange(std::begin(iterable), std::end(iterable), std::forward<Exception>(exception), predicate);
    }
}