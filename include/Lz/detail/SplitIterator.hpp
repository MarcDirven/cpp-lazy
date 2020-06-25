#pragma once

#include <algorithm>
#include <string>
#include <Lz/detail/LzTools.hpp>

#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#include <string>
#else
#include <string_view>
#endif


namespace lz { namespace detail {
    template<class SubString>
    struct SplitViewIteratorHelper {
        std::string delimiter{};
        const std::string& string = std::string();
        mutable SubString substring;
    };

    template<class SubString>
    class SplitIterator {
        const SplitViewIteratorHelper<SubString>* _splitIteratorHelper = SplitViewIteratorHelper<SubString>();
        size_t _start{};
        size_t _last{};

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type =  std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using reference = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using difference_type = std::string::const_iterator::difference_type;
        using pointer = FakePointerProxy<reference>;

        SplitIterator(size_t startingPosition, const SplitViewIteratorHelper<SubString>* splitIteratorHelper) :
            _splitIteratorHelper(splitIteratorHelper),
            _start(startingPosition) {
            if (startingPosition == splitIteratorHelper->string.size()) {
                return;
            }
            find();
        }

        void find() {
            _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _start);

            if (_last != std::string::npos) {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_start],
                                                            _last - _start);
                // Check if end ends with delimiter
                if (_last == _splitIteratorHelper->string.size() - _splitIteratorHelper->delimiter.size()) {
                    _last = std::string::npos;
                }
                else {
                    _start = _last + _splitIteratorHelper->delimiter.size();
                }
            }
            else {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_start]);
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
            return _start != other._start;
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            _start = _last == std::string::npos ? _splitIteratorHelper->string.size() : _start;
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