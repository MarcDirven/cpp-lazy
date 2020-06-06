#pragma once


#include "Tools.hpp"


namespace detail 
{
	template<typename... Iterator>
	decltype(auto) deref(Iterator... it)
	{
		return std::forward_as_tuple((*it)...);
	}

	template<typename... Iterator>
	decltype(auto) arrow(Iterator... it)
	{
		return std::forward_as_tuple((it.operator->())...);
	}

	template<typename... Iterator>
	decltype(auto) plusplus(Iterator&... it)
	{
		return std::forward_as_tuple((++it)...);
	}

	template<typename... Iterator>
	decltype(auto) minmin(Iterator&... it)
	{
		return std::forward_as_tuple((--it)...);
	}

	template<size_t I, typename... Iterator>
	struct Nequal
	{
		bool operator()(const std::tuple<Iterator...>& begin, const std::tuple<Iterator...>& end)
		{
			return Nequal<I - 1, Iterator...>{}(begin, end);
		}
	};

	template<typename... Iterator>
	struct Nequal<0, Iterator...>
	{
		bool operator()(const std::tuple<Iterator...>& begin, const std::tuple<Iterator...>& end)
		{
			return std::get<0>(begin) != std::get<0>(end);
		}
	};

	template<typename... Iterator>
	bool nequal(const std::tuple<Iterator...>& begin, const std::tuple<Iterator...>& end)
	{
		return Nequal<sizeof...(Iterator) - 1, Iterator...>{}(begin, end);
	}
	
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
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
	
		ConstZipIterator(ConstIterators begin, ConstIterators end):
			_begin(std::move(begin)),
			_end(std::move(end))
		{
		}
	
		decltype(auto) operator*() const
		{
			return apply(deref<typename std::decay_t<Containers>::const_iterator...>, _begin);
		}

		decltype(auto) operator->() const
		{
			return arrow(arrow<typename std::decay_t<Containers>::const_iterator...>, _begin);
		}

		bool operator!=(const ConstZipIterator& other) const
		{
			return apply(nequal<typename std::decay_t<Containers>::const_iterator...>, std::make_tuple(_begin, other._end));
		}

		bool operator==(const ConstZipIterator& other) const
		{
			return !(*this == other);
		}

		ConstZipIterator& operator++()
		{
			apply(plusplus<typename std::decay_t<Containers>::const_iterator...>, _begin);
			return *this;
		}

		ConstZipIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ConstZipIterator& operator--()
		{
			apply(minmin<typename std::decay_t<Containers>::const_iterator...>, _begin);
			return *this;
		}

		ConstZipIterator operator--(int)
		{
			auto tmp = *this;
			--* this;
			return tmp;
		}
	};

	template<typename... Containers>
	class ZipIterator
	{
		using Iterators = std::tuple<typename std::decay_t<Containers>::iterator...>;
		Iterators _begin{};
		Iterators _end{};

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using difference_type = std::tuple<typename std::decay_t<Containers>::difference_type...>;
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
	
		ZipIterator(Iterators begin, Iterators end):
			_begin(std::move(begin)),
			_end(std::move(end))
		{
		}

		decltype(auto) operator*()
		{
			return apply(deref<typename std::decay_t<Containers>::iterator...>, _begin);
		}

		decltype(auto) operator->()
		{
			return arrow(arrow<typename std::decay_t<Containers>::iterator...>, _begin);
		}

		bool operator!=(const ZipIterator& other) const
		{
			return apply(nequal<typename std::decay_t<Containers>::iterator...>, std::make_tuple(_begin, other._end));
		}

		bool operator==(const ZipIterator& other) const
		{
			return !(*this == other);
		}

		ZipIterator& operator++()
		{
			apply(plusplus<typename std::decay_t<Containers>::iterator...>, _begin);
			return *this;
		}

		ZipIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ZipIterator& operator--()
		{
			apply(minmin<typename std::decay_t<Containers>::iterator...>, _begin);
			return *this;
		}

		ZipIterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}
	};
}

namespace lz
{
	template<typename... Containers>
	class ZipObject
	{
	public:
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using size_type = std::tuple<typename std::decay_t<Containers>::size_type...>;
		using difference_type = std::tuple<typename std::decay_t<Containers>::difference_type...>;
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using const_pointer = std::tuple<typename std::decay_t<Containers>::const_pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
		using const_reference = std::tuple<typename std::decay_t<Containers>::const_reference...>;

		using iterator = detail::ZipIterator<Containers...>;
		using const_iterator = detail::ConstZipIterator<Containers...>;

	private:
		std::tuple<Containers&&...> _containers;

		std::tuple<typename std::decay_t<Containers>::iterator...> beginTuple(Containers&&... containers)
		{
			return std::forward_as_tuple((containers.begin())...);
		}
		
		std::tuple<typename std::decay_t<Containers>::iterator...> endTuple(Containers&&... containers)
		{
			return std::forward_as_tuple((containers.end())...);
		}
		
		std::tuple<typename std::decay_t<Containers>::const_iterator...> beginTuple(Containers&&... containers) const
		{
			return std::forward_as_tuple((containers.begin())...);
		}
		
		std::tuple<typename std::decay_t<Containers>::const_iterator...> endTuple(Containers&&... containers) const
		{
			return std::forward_as_tuple((containers.end())...);
		}

		template<typename Iterator, typename... IteratorType>
		Iterator makeIterator() const
		{
			auto beginFn = std::mem_fn<std::tuple<IteratorType...>(Containers&&...)>(&ZipObject<Containers...>::beginTuple);
			auto endFn = std::mem_fn<std::tuple<IteratorType...>(Containers&&...)>(&ZipObject<Containers...>::endTuple);

			auto beginIterators = detail::apply(beginFn, std::tuple_cat(std::make_tuple(*this), _containers));
			auto endIterators = detail::apply(endFn, std::tuple_cat(std::make_tuple(*this), _containers));

			return Iterator(std::move(beginIterators), std::move(endIterators));
		}
	public:

		explicit ZipObject(Containers&&... containers):
			_containers({std::forward<Containers>(containers)...})
		{
		}

		// iterator begin()
		// {
		// 	return makeIterator<iterator, typename std::decay_t<Containers>::iterator...>();
		// }
		//
		// iterator end()
		// {
		// 	return makeIterator<iterator, typename std::decay_t<Containers>::iterator...>();
		// }

		const_iterator begin() const
		{
			return makeIterator<const_iterator, typename std::decay_t<Containers>::const_iterator...>();
		}
		
		const_iterator end() const
		{
			return makeIterator<const_iterator, typename std::decay_t<Containers>::const_iterator...>();
		}
	};

	template<typename... Containers>
	ZipObject<Containers...> zip(Containers&&... containers)
	{
		return ZipObject<Containers...>(std::forward<Containers>(containers)...);
	}
}
