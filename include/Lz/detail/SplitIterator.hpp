#pragma once

#include <algorithm>
#include <string>
#include <iostream>

#include <Lz/detail/LzTools.hpp>


#ifdef CXX_LT_17

#include <string>

    namespace internal {
        template<class String>
        struct SplitViewIteratorHelper {
            std::string delimiter{};
            const String& string = String();

#else

#include <string_view>

#endif


namespace lz { namespace detail {
    template<class SubString>
    struct SplitViewIteratorHelper {
        std::string delimiter{};
        const std::string& string = std::string();
        mutable SubString substring{};
    };

    template<class SubString>
    class SplitIterator {
        mutable size_t _currentPos{}, _last{};
        const SplitViewIteratorHelper<SubString>* _splitIteratorHelper = SplitViewIteratorHelper<SubString>();

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = SubString;
        using reference = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using difference_type = std::string::const_iterator::difference_type;
        using pointer = FakePointerProxy<reference>;

        SplitIterator(const size_t startingPosition, const SplitViewIteratorHelper<SubString>* splitIteratorHelper) :
            _currentPos(startingPosition),
            _splitIteratorHelper(splitIteratorHelper) {
            if (startingPosition == splitIteratorHelper->string.size()) {
                return;
            }
            find();
        }

        void find() {
            _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _currentPos);

            if (_last != std::string::npos) {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_currentPos],
                                                            _last - _currentPos);
                // Check if end ends with delimiter
                if (_last == _splitIteratorHelper->string.size() - _splitIteratorHelper->delimiter.size()) {
                    _last = std::string::npos;
                }
                else {
                    _currentPos = _last + _splitIteratorHelper->delimiter.size();
                }
            }
            else {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_currentPos]);
            }
        }

        // Returns a reference to a std::string if C++14, otherwise it returns a std::string_view by value
        std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString> operator*() const {
            return _splitIteratorHelper->substring;
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
            _currentPos = _last == std::string::npos ? _splitIteratorHelper->string.size() : _currentPos;
            find();
            return *this;
        }

        SplitIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };
}}