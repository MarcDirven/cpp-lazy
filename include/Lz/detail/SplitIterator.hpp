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
        std::string delimiter;
        const std::string& string = std::string();
        mutable SubString substring;
        mutable size_t start{}, last{};
    };

    template<class SubString>
    class SplitIterator {
        const SplitViewIteratorHelper<SubString>* _splitIteratorHelper = SplitViewIteratorHelper<SubString>();

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using reference = std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString>;
        using difference_type = std::string::const_iterator::difference_type;
        using pointer = FakePointerProxy<reference>;

        SplitIterator(size_t startingPosition, const SplitViewIteratorHelper<SubString>* splitIteratorHelper) :
            _splitIteratorHelper(splitIteratorHelper) {
            _splitIteratorHelper->start = startingPosition;
            if (startingPosition == splitIteratorHelper->string.size()) {
                return;
            }
            find();
        }

        void find() {
            _splitIteratorHelper->last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter,
                                                                           _splitIteratorHelper->start);

            if (_splitIteratorHelper->last != std::string::npos) {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_splitIteratorHelper->start],
                                                            _splitIteratorHelper->last - _splitIteratorHelper->start);
                // Check if end ends with delimiter
                if (_splitIteratorHelper->last ==
                    _splitIteratorHelper->string.size() - _splitIteratorHelper->delimiter.size()) {
                    _splitIteratorHelper->last = std::string::npos;
                }
                else {
                    _splitIteratorHelper->start = _splitIteratorHelper->last + _splitIteratorHelper->delimiter.size();
                }
            }
            else {
                _splitIteratorHelper->substring = SubString(&_splitIteratorHelper->string[_splitIteratorHelper->start]);
            }
        }

        // Returns a reference to a std::string if C++14, otherwise it returns a std::string_view by value
        std::conditional_t<std::is_same<SubString, std::string>::value, SubString&, SubString> operator*() const {
            return _splitIteratorHelper->substring;
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        bool operator!=(const SplitIterator& /*other*/) const {
            // We do not want to check other, since we have a pointer to the mutual shared struct of begin and end
            return _splitIteratorHelper->start != _splitIteratorHelper->string.size();
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            _splitIteratorHelper->start =
                _splitIteratorHelper->last == std::string::npos ? _splitIteratorHelper->string.size()
                                                                : _splitIteratorHelper->start;
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