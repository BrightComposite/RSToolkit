//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_H
#define MORPHER_H

//---------------------------------------------------------------------------

#include <container/Map.h>
#include <core/Object.h>
#include <atomic>

//---------------------------------------------------------------------------

#include <component/Morpher.hpp>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class Base, class ... Owner>
	class Morpher
	{
	public:
		static_assert(is_morph_type<Base>::value, "Morpher can only use morph types marked with the 'morph_base' macro");

		template<class T, class Context>
		using is_context = is_same<decltype(declval<Context>().init(declval<Handle<T, Owner...> &>())), Handle<T, Owner...> &>;

		template<class T, class ... A, useif<based_on<T, Base>::value, can_construct<T, A...>::value>>
		Handle<T, Owner...> construct(A && ... args)
		{
			Handle<T, Owner...> h;
			place<T>() = h.init(forward<A>(args)...);
			return h;
		}

		template<class T, class Context, useif<based_on<T, Base>::value, is_context<T, Context>::value>>
		Handle<T, Owner...> construct(Context * ctx)
		{
			Handle<T, Owner...> h;
			place<T>() = ctx->init(h);
			return h;
		}

		template<class T, useif<based_on<T, Base>::value, can_construct<T>::value>>
		Handle<T, Owner...> require()
		{
			auto & p = place<T>();

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> h;
			p = h.init();
			return h;
		}

		template<class T, class Context, useif<based_on<T, Base>::value, is_context<T, Context>::value>>
		Handle<T, Owner...> require(Context * ctx)
		{
			auto & p = place<T>();

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> h;
			p = ctx->init(h);
			return h;
		}

		template<class T, useif<based_on<T, Base>::value>>
		Handle<T, Owner...> seek() const
		{
			auto i = map.find(morphid(T));

			if(i == map.end())
				return nullptr;

			return handle_cast<T>(i->second);
		}

		template<class T, useif<based_on<T, Base>::value>>
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

		Map<int, Base, Owner...> map;
	};
};

//---------------------------------------------------------------------------
#endif
