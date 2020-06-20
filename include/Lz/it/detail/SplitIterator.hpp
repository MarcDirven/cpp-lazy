#pragma once

#include <algorithm>
#include <sstream>

#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
    #include <string>
#else
    #include <string_view>
#endif


namespace lz {  namespace it { namespace detail {
    template<class SubString>
    class SplitIterator {
        std::string _delimiter{};
        const std::string& _string = std::string();
        size_t _pos{};
        size_t _next{};
        SubString _substring{};

    public:
        using reference = std::conditional_t<std::is_same_v<SubString, std::string>, SubString&, SubString>;
        using pointer = const SubString*;
        using iterator_category = std::forward_iterator_tag;
        using value_type = SubString;
        using difference_type = std::string::const_iterator::difference_type;

        SplitIterator(size_t iterIndex, const std::string& string, std::string delimiter) :
            _delimiter(std::move(delimiter)),
            _string(string),
            _pos(iterIndex){
        }

        void find() {
            _next = _string.find(_delimiter, _pos);

            if (_next == std::string::npos) {
                _substring = SubString(&_string[_pos]);
            }
            else {
                _substring = SubString(&_string[_pos], _next - _pos);
                _pos = _next + _delimiter.length();
            }
        }

        reference operator*() {
            return _substring;
        }

        pointer operator->() {
            return &_substring;
        }

        bool operator!=(const SplitIterator& other) const {
            return _pos != other._pos ;
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            _pos = _next == std::string::npos ? _string.size() : _pos;
            find();
            return *this;
        }
    };
}}}