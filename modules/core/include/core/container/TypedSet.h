//---------------------------------------------------------------------------

#ifndef TYPED_SET_H
#define TYPED_SET_H

//---------------------------------------------------------------------------

#include <core/container/Map.h>

#include <atomic>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class Base>
	struct TypeBase
	{
		static int nextId()
		{
			static std::atomic<int> counter;
			return ++counter;
		}
	};

	template<class T, class Base>
	struct TypeId
	{
		static int id()
		{
			static int i = TypeBase<Base>::nextId();
			return i;
		}
	};

	template<class Base>
	struct TypeId<Base, Base>
	{
		static int id()
		{
			return 0;
		}
	};

#define typebase_api(module, ... /* base */) \
	template struct api(module) TypeBase<__VA_ARGS__>

#define typeid_api(module, ... /* type, base */) \
	template struct api(module) TypeId<__VA_ARGS__>

	template<class Base, class ... OwnerAttr>
	class TypedSet
	{
	public:
		template<class T, class ... A,
			useif <based_on<T, Base>::value && can_construct<T, A...>::value> endif
		>
		Handle<T, OwnerAttr...> construct(A && ... args)
		{
			Handle<T, OwnerAttr...> h;
			place<T>() = h.init(forward<A>(args)...);

			return h;
		}

		template<class T, class Context,
			useif <based_on<T, Base>::value> endif,
			typename = decltype(declval<Context>().template init<T>(declval<Handle<Base, OwnerAttr...>>()))
		>
		Handle<T, OwnerAttr...> & construct(Context * ctx)
		{
			auto & h = place<T>();
			ctx->template init<T>(h);

			return h;
		}

		template<class T, useif <based_on<T, Base>::value && can_construct<T>::value> endif>
		Handle<T, OwnerAttr...> require()
		{
			auto & h = place<T>();

			if(h == nullptr)
				h = handle<T>();

			return handle_cast<T>(h);
		}

		template<class T, class Context,
			useif <based_on<T, Base>::value> endif,
			typename = decltype(declval<Context>().template init<T>(declval<Handle<Base, OwnerAttr...>>()))
		>
		Handle<T, OwnerAttr...> require(Context * ctx)
		{
			auto & h = place<T>();

			if(h == nullptr)
				ctx->template init<T>(h);

			return handle_cast<T>(h);
		}

		template<class T, useif <based_on<T, Base>::value> endif>
		Handle<T, OwnerAttr...> seek() const
		{
			auto i = map.find(TypeId<T, Base>::id());

			if(i == map.end())
				return nullptr;

			return handle_cast<T>(i->second);
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
		Handle<Base, OwnerAttr...> & place() const
		{
			return map[TypeId<T, Base>::id()];
		}

		mutable Map<int, Base, OwnerAttr...> map;
	};

#define concurrent_types(...) using Concurrents = tuple<__VA_ARGS__>;

	template<class T>
	using get_concurrent_types = typename T::Concurrents;

#define check_concurrent_types(T, Checker, map) foreach_t<get_concurrent_types<T>>::iterate<Checker>(map);
}

//---------------------------------------------------------------------------
#endif
