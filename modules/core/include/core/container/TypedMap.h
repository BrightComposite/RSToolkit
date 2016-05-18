//---------------------------------------------------------------------------

#ifndef TYPED_MAP_H
#define TYPED_MAP_H

//---------------------------------------------------------------------------

#include <core/container/TypedSet.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class K, class V, class ... OwnerAttr>
	class TypedMap
	{
	public:
		template<class T, class ... A,
			useif <based_on<T, K>::value && can_construct<V, A...>::value> endif
		>
		Handle<V, OwnerAttr...> & construct(A && ... args)
		{
			return place<T>().init(forward<A>(args)...);
		}

		template<class T, class Context,
			useif <based_on<T, K>::value> endif,
			typename = decltype(declval<Context>().template init<T>(declval<Handle<V, OwnerAttr...>>()))
		>
		Handle<V, OwnerAttr...> & construct(Context * ctx)
		{
			auto & h = place<T>();
			ctx->template init<T>(h);

			return h;
		}

		template<class T, useif <based_on<T, K>::value && can_construct<V>::value> endif>
		Handle<V, OwnerAttr...> & require()
		{
			auto & h = place<T>();

			if(h == nullptr)
				h.init();

			return h;
		}

		template<class T, class Context,
			useif <based_on<T, K>::value> endif,
			typename = decltype(declval<Context>().template init<T>(declval<Handle<V, OwnerAttr...>>()))
		>
		Handle<V, OwnerAttr...> & require(Context * ctx)
		{
			auto & h = place<T>();

			if(h == nullptr)
				ctx->template init<T>(h);

			return h;
		}

		template<class T, useif <based_on<T, K>::value> endif>
		Handle<V, OwnerAttr...> seek() const
		{
			auto i = map.find(TypeId<T, K>::get());

			if(i == map.end())
				return nullptr;

			return i->second;
		}

		size_t size() const
		{
			return map.size();
		}

		void clear()
		{
			return map.clear();
		}

	protected:
		template<class T>
		Handle<V, OwnerAttr...> & place() const
		{
			return map[TypeId<T, K>::get()];
		}

		mutable Map<int, V, OwnerAttr...> map;
	};
}

//---------------------------------------------------------------------------
#endif
