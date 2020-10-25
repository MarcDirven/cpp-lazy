#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include <utility>


namespace lz { namespace internal {

    template<class Func>
    class FunctionContainer {
        Func _func;
        bool _isConstructed{false};

        explicit FunctionContainer(std::false_type /*isDefaultConstructible*/) {}

        explicit FunctionContainer(std::true_type /*isDefaultConstructible*/) :
            _func(),
            _isConstructed(true) {}

        template<class F>
        void construct(F&& f) {
            ::new(static_cast<void*>(std::addressof(_func))) Func(static_cast<F&&>(f));
            _isConstructed = true;
        }

        void move(Func&& f, std::true_type /* isMoveAssignable */) {
            _func = std::move(f);
        }

        void move(Func&& f, std::false_type /* isMoveAssignable */) {
            reset();
            construct(std::move(f));
        }

        void reset() {
            if (_isConstructed) {
                _func.~Func();
                _isConstructed = false;
            }
        }

        void copy(const Func& f, std::true_type /*isCopyAssignable*/) {
            _func = f;
        }

        void copy(const Func& f, std::false_type /*isCopyAssignable*/) {
            reset();
            construct(f);
        }

    public:
        explicit FunctionContainer(const Func& func) :
            _func(func),
            _isConstructed(true) {
        }

        explicit FunctionContainer(Func&& func) :
            _func(std::move(func)),
            _isConstructed(true) {
        }

        FunctionContainer() :
            FunctionContainer(std::is_default_constructible<Func>()) {
        }

        FunctionContainer(FunctionContainer&& other) noexcept:
            _func(std::move(other._func)),
            _isConstructed(true) {
            other._isConstructed = false;
        }

        FunctionContainer(const FunctionContainer& other) :
            _func(other._func),
            _isConstructed(true) {
        }

        FunctionContainer& operator=(const FunctionContainer& other) {
            if (_isConstructed && other._isConstructed) {
                copy(other._func, std::is_copy_assignable<Func>());
            }
            else if (other._isConstructed) {
                construct(other._func);
            }
            else if (_isConstructed) {
                reset();
            }
            return *this;
        }

        FunctionContainer& operator=(FunctionContainer&& other) noexcept {
            if (_isConstructed && other._isConstructed) {
                move(std::move(other._func), std::is_move_assignable<Func>());
            }
            else if (other._isConstructed) {
                construct(std::move(other._func));
            }
            else if (_isConstructed) {
                reset();
            }
            return *this;
        }

        template<class... Args>
        auto operator()(Args&& ... args) const -> decltype(_func(args...)) {
            return _func(std::forward<Args>(args)...);
        }
    };
}}
#endif // LZ_FUNCTION_CONTAINER_HPP
