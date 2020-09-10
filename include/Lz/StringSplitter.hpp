#pragma once


#include "detail/SplitIterator.hpp"
#include "detail/BasicIteratorView.hpp"


#include <array>
#include <vector>


#ifdef CXX_LT_17
  #include <string>
#else
  #include <string_view>
#endif


namespace lz {
    template<class SubString, class String>
    class StringSplitter final : public detail::BasicIteratorView<detail::SplitIterator<SubString, String>> {
    public:
        using const_iterator = detail::SplitIterator<SubString, String>;
        using iterator = const_iterator;

    private:
        detail::SplitViewIteratorHelper<String> _splitIteratorHelper;

    public:
        using value_type = SubString;

        /**
         * @brief Creates a String splitter object. Its `begin()` and `end()` return an input iterator.
         * @param str The string to split.
         * @param delimiter The delimiter to split on.
         */
        StringSplitter(String&& str, std::string&& delimiter) :
            _splitIteratorHelper{std::move(delimiter), std::forward<String>(str)} {
        }

        /**
         * @brief Returns an input string split iterator to the beginning.
         * @return A input string split iterator to the beginning.
         */
        const_iterator begin() const override {
            return const_iterator(0, &_splitIteratorHelper);
        }

        /**
         * @brief Returns an input string split iterator to the ending.
         * @return A input string split iterator to the ending.
         */
        const_iterator end() const override {
            return const_iterator(_splitIteratorHelper.string.size(), &_splitIteratorHelper);
        }
    };

#ifdef CXX_LT_17
    template class StringSplitter<std::string, std::string>;
    template<class SubString = std::string, class String = std::string>
#else
    template class StringSplitter<std::string_view, std::string_view>;
    template<class SubString = std::string_view, class String = std::string_view>
#endif
    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief This is a lazy evaluated string splitter function. If not using C++17 or higher, you can use `std::move`
     * to safely move the substring, that is returned by the
     * `StringSplitter<SubString>::const_iterator::operator*`. Its `begin()` and `end()` return an input iterator.
     * @tparam SubString The type that gets returned when the `StringSplitter<SubString>::const_iterator::operator*` is
     * called. Can be specified, but if C++17 or higher is defined, `std::string_view` is used, otherwise `std::string`.
     * @param str The string to split.
     * @param delimiter The delimiter to split on.
     * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::split(...))`.
     */
    StringSplitter<SubString, String> split(String&& str, std::string delimiter) {
        return StringSplitter<SubString, String>(std::forward<String>(str), std::move(delimiter));
    }

    // End of group
    /**
     * @}
     */
}
