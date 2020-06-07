#pragma once


#include <tuple>
#include <type_traits>
#include <utility>


namespace detail
{
	template<class Container, class Iterator>
	void fillContainer(Iterator first, Container& container)
	{
		for (size_t i = 0; i < container.size(); i++)
		{
			container[i] = *first;
			++first;
		}
	}

	template<typename T>
	struct IsCallable {
	private:
		typedef char(&Yes)[1];
		typedef char(&No)[2];

		struct Fallback
		{
			void operator()();
		};
		
		struct Derived : T, Fallback
		{
		};

		template<typename U, U>
		struct Check;

		template<typename>
		static Yes test(...);

		template<typename C>
		static No test(Check<void (Fallback::*)(), &C::operator()>*);

	public:
		static const bool value = sizeof(test<Derived>(nullptr)) == sizeof(Yes);
	};

	template<size_t N>
	struct Apply {
		template<typename F, typename T, typename... A>
		static inline auto apply(F&& f, T&& t, A&&... a)-> decltype(
							     Apply<N - 1>::apply(::std::forward<F>(f), ::std::forward<T>(t), ::std::get<N - 1>(
												     ::std::forward<T>(t)), ::std::forward<A>(a)...))
		{
			return Apply<N - 1>::apply(
				::std::forward<F>(f), ::std::forward<T>(t), ::std::get<N - 1>(::std::forward<T>(t)), ::std::forward<A>(a)...	);
		}
	};

	template<>
	struct Apply<0> {
		template<typename F, typename T, typename... A>
		static inline auto apply(F&& f, T&&, A&&... a)-> decltype(::std::forward<F>(f)(::std::forward<A>(a)...))
		{
			return ::std::forward<F>(f)(::std::forward<A>(a)...);
		}
	};

	template<typename F, typename T>
	inline auto apply(F&& f, T&& t)	-> decltype(
				      Apply<::std::tuple_size<typename ::std::decay<T>::type>::value>::apply(::std::forward<F>(f), ::std::forward<T>(t)))
	{
		return Apply<::std::tuple_size<typename ::std::decay<T>::type>::value>::apply(::std::forward<F>(f), ::std::forward<T>(t));
	}

}
