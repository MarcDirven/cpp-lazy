#pragma once
#include <tuple>


namespace detail 
{
	template<typename... Containers>
	class ConstZipIterator
	{
		using ConstIterators = std::tuple<typename std::decay_t<Containers>::const_iterator...>;
		ConstIterators _begin{};
		ConstIterators _end{};

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using difference_type = std::tuple<typename std::decay_t<Containers>::difference_type...>;
		using pointer = std::tuple<const typename std::decay_t<Containers>::pointer...>;
		using reference = std::tuple<const typename std::decay_t<Containers>::reference...>;

	private:
		template<typename... Iterator>
		reference deref(Iterator... it) const
		{
			return { (*it)... };
		}

	public:
		ConstZipIterator(ConstIterators begin, ConstIterators end):
			_begin(std::move(begin)),
			_end(std::move(end))
		{
		}

		reference operator*() const
		{
			return detail::apply(deref<typename std::decay_t<Containers>::const_iterator...>, _begin);
		}
	};

	template<typename... Containers>
	class ZipIterator : public ConstZipIterator<Containers...>
	{
		using Iterators = std::tuple<typename std::decay_t<Containers>::iterator...>;
	public:
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;

		ZipIterator(Iterators begin, Iterators end):
			ConstZipIterator<Containers...>(begin, end)
		{
		}
	};
}

namespace lz
{
	template<typename... Containers>
	class ZipObject
	{
		std::tuple<typename std::decay_t<Containers>::const_iterator...> _begin{};
		std::tuple<typename std::decay_t<Containers>::const_iterator...> _end{};

	public:
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using size_type = std::tuple<typename std::decay_t<Containers>::size_type...>;
		using difference_type = std::tuple<typename std::decay_t<Containers>::difference_type...>;
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using const_pointer = std::tuple<typename std::decay_t<Containers>::const_pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
		using const_reference = std::tuple<typename std::decay_t<Containers>::const_reference...>;

		using iterator = detail::ZipIterator<Containers...>;
		using const_iterator = detail::ConstZipIterator<const Containers...>;

		explicit ZipObject(Containers&&... containers):
			_begin({containers.begin()...}),
			_end({containers.end()...})
		{
		}

		const_iterator begin() const
		{
			return const_iterator(_begin, _end);
		}

		const_iterator end() const
		{
			return const_iterator();
		}
	};

	template<typename... Containers>
	ZipObject<Containers...> zip(Containers&&... containers)
	{
		return ZipObject<Containers...>(std::forward<Containers>(containers)...);
	}
}
