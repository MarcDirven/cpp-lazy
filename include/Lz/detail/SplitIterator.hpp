#pragma once

#include <algorithm>

#if __cplusplus < 201703L
    #include <string>
#else
    #include <string_view>
#endif


namespace lz { namespace detail {
    template<class SubString>
    class SplitIterator {
        std::string _delimiter{};
        const std::string& _string = std::string();
        size_t _iterIndex{};
        size_t _current{};
        SubString _substring{};

    public:
        using reference = typename std::conditional<std::is_same<SubString, std::string>::value,
                                                    const SubString&, SubString>::type;
        using pointer = const SubString*;
        using iterator_category = std::input_iterator_tag;
        using value_type = SubString;
        using difference_type = std::string::const_iterator::difference_type;

        SplitIterator(size_t iterIndex, const std::string& string, std::string delimiter) :
            _delimiter(std::move(delimiter)),
            _string(string),
            _iterIndex(iterIndex) {
        }

        void find() {
            _current = _string.find(_delimiter, _iterIndex);

            if (_current == std::string::npos) {
                _substring = SubString(&_string[_iterIndex]);
            }
            else {
                _substring = SubString(&_string[_iterIndex], _current - _iterIndex);
                _iterIndex = _current + _delimiter.length();
            }
        }

        reference operator*() const {
            return _substring;
        }

        pointer operator->() const {
            return &_substring;
        }

        bool operator!=(const SplitIterator& other) const {
            return _iterIndex != other._iterIndex;
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            _iterIndex = _current == std::string::npos ? _string.size() : _iterIndex;
            find();
            return *this;
        }
    };
}}