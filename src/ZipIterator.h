#pragma once


#include "Tools.hpp"


namespace detail 
{
	// template<typename... Iterator>
	// decltype(auto) deref(Iterator... it)
	// {
	// 	return std::forward_as_tuple((*it)...);
	// }
	//
	// template<typename... Iterator>
	// decltype(auto) arrow(Iterator... it)
	// {
	// 	return std::forward_as_tuple((it.operator->())...);
	// }
	//
	// template<typename... Iterator>
	// decltype(auto) plusplus(Iterator&... it)
	// {
	// 	return std::forward_as_tuple((++it)...);
	// }
	//
	// template<typename... Iterator>
	// decltype(auto) minmin(Iterator&... it)
	// {
	// 	return std::forward_as_tuple((--it)...);
	// }

	template<size_t I, typename... Iterator>
	struct Nequal
	{
		bool operator()(const std::tuple<Iterator...>& begin, const std::tuple<Iterator...>& end)
		{
			return Nequal<I - 1, Iterator...>{}(begin, end) || Nequal<I - 2, Iterator...>{}(begin, end);
		}
	};

	template<typename... Iterator>
	struct Nequal<1, Iterator...>
	{
		bool operator()(const std::tuple<Iterator...>& begin, const std::tuple<Iterator...>& end)
		{
			return std::get<1>(begin) != std::get<1>(end) || std::get<0>(begin) != std::get<0>(end);
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
	protected:
		ConstIterators _begin{};
		ConstIterators _end{};
	
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using difference_type = ptrdiff_t;
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
	
		ConstZipIterator(ConstIterators begin, ConstIterators end):
			_begin(std::move(begin)),
			_end(std::move(end))
		{
		}
	
		reference operator*() const
		{
			return apply([](typename std::decay_t<Containers>::const_iterator... iterators)
			{
				return std::forward_as_tuple((*iterators)...);
			}, _begin);	
		}

		pointer operator->() const
		{
			return apply([](typename std::decay_t<Containers>::const_iterator... iterators)
			{
				return std::forward_as_tuple((iterators.operator->())...);
			}, _begin);
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
			apply([](typename std::decay_t<Containers>::const_iterator&... iterators)
			{
				return std::forward_as_tuple((++iterators)...);
			}, _begin);
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
			apply([](typename std::decay_t<Containers>::const_iterator&... iterators)
			{
				return std::forward_as_tuple((--iterators)...);
			}, _begin);
			return *this;
		}

		ConstZipIterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}
	};

	template<typename... Containers>
	class ZipIterator : public ConstZipIterator<Containers...>
	{
		using Base = ConstZipIterator<Containers...>;
		using Iterators = std::tuple<typename std::decay_t<Containers>::iterator...>;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;
		using difference_type = ptrdiff_t;
		using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
		using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
	
		ZipIterator(Iterators begin, Iterators end):
			Base(std::move(begin), std::move(end))
		{
		}

		reference operator*()
		{
			return apply([](typename std::decay_t<Containers>::iterator... iterators)
			{
				return std::forward_as_tuple(const_cast<typename std::decay_t<Containers>::reference>((*iterators))...);
			}, Base::_begin);
		}

		pointer operator->()
		{
			return apply([](typename std::decay_t<Containers>::iterator... iterators)
			{
				return std::forward_as_tuple(const_cast<typename std::decay_t<Containers>::pointer>((*iterators))...);
			}, Base::_begin);
		}

		bool operator!=(const ZipIterator& other) const
		{
			return Base::operator!=(other);
		}

		bool operator==(const ZipIterator& other) const
		{
			return Base::operator==(other);
		}

		ZipIterator& operator++()
		{
			Base::operator++();
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
			Base::operator--();
			return *this;
		}

		ZipIterator operator--(int)
		{
			auto tmp = *this;
			Base::operator--();
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

		iterator makeIterator()
		{
			auto beginIterators = detail::apply([](Containers&&... containers)
			{
				return std::forward_as_tuple((containers.begin())...);
			}, _containers);

			auto endIterators = detail::apply([](Containers&&... containers)
			{
				return std::forward_as_tuple((containers.end())...);
			}, _containers);

			return iterator(std::move(beginIterators), std::move(endIterators));
		}

		const_iterator makeConstIterator() const
		{
			auto beginIterators = detail::apply([](Containers&&... containers)
			{
				return std::forward_as_tuple((containers.begin())...);
			}, _containers);

			auto endIterators = detail::apply([](Containers&&... containers)
			{
				return std::forward_as_tuple((containers.end())...); 
			}, _containers);

			return const_iterator(std::move(beginIterators), std::move(endIterators));
		}

	public:

		explicit ZipObject(Containers&&... containers):
			_containers({std::forward<Containers>(containers)...})
		{
		}

		iterator begin()
		{
			return makeIterator();
		}
		
		iterator end()
		{
			return makeIterator();
		}

		const_iterator begin() const
		{
			return makeConstIterator();
		}
		
		const_iterator end() const
		{
			return makeConstIterator();
		}
	};

	template<typename... Containers>
	ZipObject<Containers...> zip(Containers&&... containers)
	{
		return ZipObject<Containers...>(std::forward<Containers>(containers)...);
	}
}
