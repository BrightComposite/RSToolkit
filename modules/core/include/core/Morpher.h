//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_H
#define MORPHER_H

//---------------------------------------------------------------------------

#include <core/container/Map.h>
#include <core/Object.h>
#include <atomic>

//---------------------------------------------------------------------------

namespace Rapture
{
	/**
	 *	Marks the Base class as a root of morphers hierarchy. But you'll still
	 *	need to create a pool for morphers based on this root with
	 *	'create_morph_pool' macro.
	 */
#define morph_base(Base) using MorphBase = Base; template<class> friend struct MorphType; template<class> friend struct is_morph_type

	type_checker(is_morph_type, MorphBase);

	/**
	 *	Creates pool of morphers based on the Base class.
	 */
#define create_morph_pool(module, Base)				\
	template<>										\
	struct api(module) MorphPool<Base>				\
	{												\
		static int increment()						\
		{											\
			static std::atomic<int> counter;		\
			return ++counter;						\
		}											\
	}

	/**
	 *	Creates morpher for the Type.
	 */
#define create_morph_type(module, Type)				\
	template<>										\
	struct api(module) MorphType<Type>				\
	{												\
		using Base = typename Type::MorphBase;		\
		using Pool = MorphPool<Base>;				\
													\
		static const char * name()					\
		{											\
			return #Type;							\
		}											\
													\
		static int id()								\
		{											\
			static int i = Pool::increment();		\
			return i;								\
		}											\
	}

//---------------------------------------------------------------------------

#define morphid(Type) MorphType<Type>::id()

	template<class Base>
	struct MorphPool {};

	template<class T>
	struct MorphType {};

	template<class Base, class ... Owner>
	class Morpher
	{
	public:
		static_assert(is_morph_type<Base>::value, "Morpher can only use morph types marked with the 'morph_base' macro");

		template<class T, class Context>
		using is_context = is_same<decltype(declval<Context>().init(declval<Handle<T, Owner...> &>())), Handle<T, Owner...> &>;

		template<class T, class ... A, useif <based_on<T, Base>::value, can_construct<T, A...>::value> endif>
		Handle<T, Owner...> construct(A && ... args)
		{
			Handle<T, Owner...> h;
			place<T>() = h.init(forward<A>(args)...);
			return h;
		}

		template<class T, class Context, useif <based_on<T, Base>::value, is_context<T, Context>::value> endif>
		Handle<T, Owner...> construct(Context * ctx)
		{
			Handle<T, Owner...> h;
			place<T>() = ctx->init(h);
			return h;
		}

		template<class T, useif <based_on<T, Base>::value, can_construct<T>::value> endif>
		Handle<T, Owner...> require()
		{
			auto & p = place<T>();

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> h;
			p = h.init();
			return h;
		}

		template<class T, class Context, useif <based_on<T, Base>::value, is_context<T, Context>::value> endif>
		Handle<T, Owner...> require(Context * ctx)
		{
			auto & p = place<T>();

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> h;
			p = ctx->init(h);
			return h;
		}

		template<class T, useif <based_on<T, Base>::value> endif>
		Handle<T, Owner...> seek() const
		{
			auto i = map.find(morphid(T));

			if(i == map.end())
				return nullptr;

			return handle_cast<T>(i->second);
		}

		template<class T, useif <based_on<T, Base>::value> endif>
		void remove()
		{
			map.erase(morphid(T));
		}

		void clear()
		{
			return map.clear();
		}

		size_t size() const
		{
			return map.size();
		}

	protected:
		template<class T>
		Handle<Base, Owner...> & place()
		{
			return map[morphid(T)];
		}

		UnorderedMap<int, Base, Owner...> map;
	};
};

//---------------------------------------------------------------------------
#endif
