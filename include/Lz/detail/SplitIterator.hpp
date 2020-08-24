#pragma once

#include <algorithm>
#include <string>
#include <iostream>

#include <Lz/detail/LzTools.hpp>


#ifdef CXX_LT_17
  #include <string>
#else
  #include <string_view>
#endif


namespace lz {
    template<class>
    class StringSplitter;

    namespace detail {
        struct SplitViewIteratorHelper {
            std::string delimiter{};
            const std::string& string = std::string();
        };


        template<class SubString>
        class SplitIterator {
            size_t _currentPos{}, _last{};
            mutable SubString _substring{};
            const SplitViewIteratorHelper* _splitIteratorHelper{};


            friend class StringSplitter<SubString>;


        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = SubString;
            using reference = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
            using difference_type = std::ptrdiff_t;
            using pointer = FakePointerProxy<reference>;

            SplitIterator(const size_t startingPosition, const SplitViewIteratorHelper* splitIteratorHelper) :
                _currentPos(startingPosition),
                _splitIteratorHelper(splitIteratorHelper) {
                // Micro optimization, check if object is created from begin(), only then we want to search
                if (startingPosition == 0) {
                    _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _currentPos);
                }
            }

            // Returns a reference to a std::string if C++14, otherwise it returns a std::string_view by value
            std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString> operator*() const {
                if (_last != std::string::npos) {
                    _substring = SubString(&_splitIteratorHelper->string[_currentPos], _last - _currentPos);
                }
                else {
                    _substring = SubString(&_splitIteratorHelper->string[_currentPos]);
                }
                return _substring;
            }

            pointer operator->() const {
                return FakePointerProxy<decltype(**this)>(**this);
            }

            bool operator!=(const SplitIterator& other) const {
                return _currentPos != other._currentPos;
            }

            bool operator==(const SplitIterator& other) const {
                return !(*this != other);
            }

            SplitIterator& operator++() {
                const size_t delimLen = _splitIteratorHelper->delimiter.length();
                const size_t stringLen = _splitIteratorHelper->string.length();

                if (_last == std::string::npos) {
                    _currentPos = stringLen;
                }
                else {
                    // Check if ends with delimiter
                    if (_last == stringLen - delimLen) {
                        _last = std::string::npos;
                        _currentPos = _splitIteratorHelper->string.length();
                    }
                    else {
                        _currentPos = _last + delimLen;
                        _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _currentPos);
                    }
                }
                return *this;
            }

            SplitIterator operator++(int) {
                SplitIterator tmp(*this);
                ++*this;
                return tmp;
            }
        };
    }
}