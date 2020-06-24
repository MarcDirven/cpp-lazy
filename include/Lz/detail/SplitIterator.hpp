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
        size_t _startingPosition{};
        size_t _last{};

    public:
        using pointer = const SubString*;
        using iterator_category = std::input_iterator_tag;
        using value_type = SubString;
        using reference = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using difference_type = std::string::const_iterator::difference_type;

        SplitIterator(size_t startingPosition, const SplitViewIteratorHelper<SubString>* splitIteratorHelper) :
            _splitIteratorHelper(splitIteratorHelper),
            _startingPosition(startingPosition),
            _last(splitIteratorHelper->string.find(_splitIteratorHelper->delimiter)) {
        }

        reference operator*() const {
            if (_last != _splitIteratorHelper->string.size()) {
                _splitIteratorHelper->substring =
                    _splitIteratorHelper->string.substr(_startingPosition, _last - _startingPosition);
                return _splitIteratorHelper->substring;
            }
            _splitIteratorHelper->substring = _splitIteratorHelper->string.substr(_startingPosition);
            return _splitIteratorHelper->substring;
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        bool operator!=(const SplitIterator& /*other*/) const {
            return _last != _splitIteratorHelper->string.size();
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            if (_last != std::string::npos) {
                _startingPosition = _last + _splitIteratorHelper->delimiter.length();
                _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _startingPosition);
            }
            else {
                _last = _splitIteratorHelper->string.size();
            }
            return *this;
        }

        SplitIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };
}}