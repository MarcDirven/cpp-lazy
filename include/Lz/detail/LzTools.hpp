#pragma once

#include <tuple>
#include <vector>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <map>


namespace lz { namespace detail {
    template<class Container, class Iterator>
    void fillContainer(Iterator first, Container& container) {
        for (size_t i = 0; i < container.size(); i++) {
            container[i] = *first;
            ++first;
        }
    }

    template<class ValueType, size_t N, class Iterator>
    std::array<ValueType, N> fillArray(Iterator first) {
        std::array<ValueType, N> array{};
        fillContainer(first, array);
        return array;
    }

    template<class Value,
             class Key,
             class Compare,
             class Allocator,
             class KeySelectorFunc,
             class Iterator>
    std::map<Key, Value, Compare, Allocator>
    toMap(Iterator begin, Iterator end, KeySelectorFunc selector, const Allocator& allocator) {
        std::map<Key, Value, Compare, Allocator> map(allocator);
        std::transform(begin, end, std::inserter(map, map.end()), [selector](const Value& value) {
            return std::make_pair(selector(value), value);
        });
        return map;
    }

    template<class T>
    class FakePointerProxy {
        T t;

    public:
        explicit FakePointerProxy(const T& t):
            t(t)
        {
        }

        T* operator->() {
            return &t;
        }
    };
}}
