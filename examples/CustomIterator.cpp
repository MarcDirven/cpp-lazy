#include <Lz/IterTools.hpp>
#include <Lz/IterBase.hpp>
#include <Lz/View.hpp>
#include <iostream>

class CustomForwardIterator
    : public lz::IterBase</* Name of 'this' class */ CustomForwardIterator, /* Reference type returned by operator* */ int&,
                          /* Pointer type returned by operator->()*/ int*,
                          /* Difference type returned by operator-() among others*/ std::ptrdiff_t,
                          /* The iterator category. Forward only in this case */ std::forward_iterator_tag> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    CustomForwardIterator(int iter) : _iter(iter) {
    }

    CustomForwardIterator() = default;

    void increment() {
        ++_iter;
    }

    int& dereference() {
        return _iter;
    }

    int* arrow() {
        return &_iter;
    }

    // Function must be const
    bool eq(const CustomForwardIterator& other) const {
        return _iter == other._iter;
    }
};

class CustomBidirectionalIterator
    : public lz::IterBase<CustomBidirectionalIterator, int&, int*, std::ptrdiff_t, std::bidirectional_iterator_tag> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    CustomBidirectionalIterator(int iter) : _iter(iter) {
    }

    CustomBidirectionalIterator() = default;

    void increment() {
        ++_iter;
    }

    // Added a decrement function
    void decrement() {
        --_iter;
    }

    int& dereference() {
        return _iter;
    }

    int* arrow() {
        return &_iter;
    }

    // Function must be const
    bool eq(const CustomBidirectionalIterator& other) const {
        return _iter == other._iter;
    }
};

class CustomRandomAccessIterator
    : public lz::IterBase<CustomRandomAccessIterator, int&, int*, std::ptrdiff_t, std::random_access_iterator_tag> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    CustomRandomAccessIterator(int iter) : _iter(iter) {
    }

    CustomRandomAccessIterator() = default;

    void increment() {
        ++_iter;
    }

    void decrement() {
        --_iter;
    }

    int& dereference() {
        return _iter;
    }

    int* arrow() {
        return &_iter;
    }

    // Function must be const
    bool eq(const CustomRandomAccessIterator& other) const {
        return _iter == other._iter;
    }

    // Added plusIs function
    void plusIs(difference_type n) {
        _iter += static_cast<int>(n);
    }

    // Added difference function, must be const
    difference_type difference(const CustomRandomAccessIterator& other) const {
        return static_cast<difference_type>(_iter - other._iter);
    }
};

int main() {
    std::cout << "Custom forward iterator example, going forward.\n";
    lz::View<CustomForwardIterator> view{ CustomForwardIterator{ 0 }, CustomForwardIterator{ 10 } };
    for (/* [const] */ int& i : view) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom bidirectional iterator example, going backward.\n";
    lz::View<CustomBidirectionalIterator> view2{ CustomBidirectionalIterator{ 0 }, CustomBidirectionalIterator{ 10 } };
    for (/* [const] */ int& i : lz::reverse(view2)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward.\n";
    lz::View<CustomRandomAccessIterator> view3{ CustomRandomAccessIterator{ 0 }, CustomRandomAccessIterator{ 10 } };
    for (/* [const] */ int& i : view3) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going backward.\n";
    for (/* [const] */ int& i : lz::reverse(view3)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward with step 2.\n";
    auto itStart = view3.begin();
    std::cout << "*itStart = " << *itStart << '\n';
    std::cout << "*(itStart + 2) = " << *(itStart + 2) << '\n';
}