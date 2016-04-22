//---------------------------------------------------------------------------

#ifndef TYPED_MAP_H
#define TYPED_MAP_H

//---------------------------------------------------------------------------

#include <atomic>
#include <core/container/Map.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class Base>
	struct TypesCounter
	{
		static std::atomic<int> counter;
	};

	template<class Base>
	std::atomic<int> TypesCounter<Base>::counter;

	template<class T, class Base>
	struct TypeId
	{
		static int get()
		{
			static int id = TypesCounter<Base>::counter++;
			return id;
		}
	};

	template<class Base>
	class TypedMap
	{
	public:
		template<class T, class ... A,
			useif(based_on(T, Base) && can_construct(T, A...))
		>
		Handle<T> construct(A && ... args)
		{
			Handle<T> h;
			place<T>() = h.init(forward<A>(args)...);

			return h;
		}

		template<class T, useif(based_on(T, Base))>
		Handle<T> request()
		{
			auto & h = place<T>();

			if(h == nullptr)
				h = handle<T>();

			return Handle<T>::cast(h);
		}

		template<class T, class Context,
			useif(based_on(T, Base)),
			typename = decltype(declval<Context>.template init<T>(handle<Base>()))
		>
		static Handle<T> request(Context * ctx)
		{
			auto & h = place<T>();

			if(h == nullptr)
				ctx->init<T>(h);

			return Handle<T>::cast(h);
		}

	protected:
		template<class T>
		Handle<Base> & place()
		{
			return map[TypeId<T, Base>::get()];
		}

		OrderedMap<int, Base> map;
	};

#define concurrent_types(...) using Concurrents = tuple<__VA_ARGS__>;

	template<class T>
	using get_concurrent_types = typename T::Concurrents;

#define check_concurrent_types(T, Checker, map) foreach_t<get_concurrent_types<T>>::iterate<Checker>(map);
}

//---------------------------------------------------------------------------
#endif
