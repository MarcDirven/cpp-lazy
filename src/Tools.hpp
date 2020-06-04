#pragma once


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
}
