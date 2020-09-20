#pragma once

#include "detail/JoinIterator.hpp"
#include "detail/BasicIteratorView.hpp"


 namespace lz {
     template<class Iterator>
class Join : public detail::BasicIteratorView<detail::JoinIterator<Iterator>>{
     public:
         using iterator = detail::JoinIterator<Iterator>;
         using const_iterator = iterator;
         using value_type = typename iterator::value_type;

     private:
         std::string _delimiter{};
         Iterator _begin{};
         Iterator _end{};
         typename iterator::difference_type _distance{};

     public:
         /**
          * @brief Creates a Join object.
          * @details Combines the iterator values followed by the delimiter. It is evaluated in a
          * `"[value][delimiter][value][delimiter]..."`-like fashion.
          * @param begin The beginning of the sequence.
          * @param end The ending of the sequence.
          * @param delimiter The delimiter to separate the previous and the next values in the sequence.
          */
         Join(const Iterator begin, const Iterator end, std::string&& delimiter):
             _delimiter(std::move(delimiter)),
             _begin(begin),
             _end(end),
             _distance(std::distance(begin, end) * 2 - 1)
         {
         }

         Join() = default;

         /**
          * @brief Returns the ending of the sequence.
          * @return The ending of the sequence.
          */
         iterator begin() const {
             return iterator(_begin, _delimiter, true, _distance);
         }

         /**
          * @brief Returns the ending of the sequence.
          * @return The ending of the sequence.
          */
         iterator end() const {
             return iterator(_end, _delimiter, false, _distance);
         }
     };

     /**
      * @addtogroup ItFns
      * @{
      */

     /**
      * @brief Creates a Join object.
      * @details Combines the iterator values followed by the delimiter. It is evaluated in a
      * `"[value][delimiter][value][delimiter]..."`-like fashion.
      * @tparam Iterator Is automatically deduced.
      * @param begin The beginning of the sequence.
      * @param end The ending of the sequence.
      * @param delimiter The delimiter to separate the previous and the next values in the sequence.
      * @return A Join iterator view object.
      */
     template<class Iterator>
     Join<Iterator> joinrange(const Iterator begin, const Iterator end, std::string delimiter) {
         return Join<Iterator>(begin, end, std::move(delimiter));
     }

     /**
      * @brief Creates a Join object.
      * @details Combines the iterator values followed by the delimiter. It is evaluated in a
      * `"[value][delimiter][value][delimiter]..."`-like fashion.
      * @tparam Iterable Is automatically deduced.
      * @param iterable The iterable to join with the delimiter.
      * @param delimiter The delimiter to separate the previous and the next values in the sequence.
      * @return A Join iterator view object.
      */
     template<class Iterable>
     auto join(Iterable&& iterable, std::string delimiter) {
         return joinrange(std::begin(iterable), std::end(iterable), std::move(delimiter));
     }

     // End of group
     /**
      * @}
      */
 }