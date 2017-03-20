//---------------------------------------------------------------------------

#pragma once

#ifndef MORPHER_H
#define MORPHER_H

//---------------------------------------------------------------------------

#include <container/Map.h>
#include <core/Object.h>
#include <atomic>

//---------------------------------------------------------------------------

#include <morph/Morph.hpp>

//---------------------------------------------------------------------------

namespace asd
{
	template<class Base, class ... Context>
	class Morph
	{
	public:
		static_assert(is_morph_type<Base>::value, "Morph can only use morph types marked with the 'morph_origin' macro");

		template<class T, useif<based_on<T, Base>::value>>
		void set(const Handle<T> & h)
		{
			map[morphid(T)] = h;
		}

		template<class T, class InitType = T, useif<based_on<T, Base>::value, based_on<InitType, T>::value, can_construct<InitType>::value>>
		Handle<T> require()
		{
			auto & p = map[morphid(T)];

			if(p != nullptr)
				return cast::as<T>(p);

			Handle<InitType> h;
			p = h.init();
			return h;
		}

		template<class T, useif<based_on<T, Base>::value>>
		Handle<T> seek() const
		{
			auto i = map.find(morphid(T));
			return i != map.end() ? cast::as<T>(i->second) : nullptr;
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
		Map<int, Base> map;
	};

	template<class Base, class Context>
	class Morph<Base, Context>
	{
	public:
		static_assert(is_morph_type<Base>::value, "Morph can only use morph types marked with the 'morph_origin' macro");

		Morph(Context * ctx) : ctx(ctx) {}

		template<class T, useif<based_on<T, Base>::value>>
		void set(const Handle<T> & h)
		{
			map[morphid(T)] = h;
		}

		template<class T, class InitType = T, useif<based_on<T, Base>::value, based_on<InitType, T>::value, can_construct<InitType, Context *>::value>>
		Handle<T> require()
		{
			auto & p = map[morphid(T)];

			if(p != nullptr)
				return cast::as<T>(p);

			Handle<InitType> h;
			p = h.init(ctx);
			return h;
		}

		template<class T, useif<based_on<T, Base>::value>>
		Handle<T> seek() const
		{
			auto i = map.find(morphid(T));

			if(i == map.end())
				return nullptr;

			return cast::as<T>(i->second);
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
		Context * ctx;
		Map<int, Base> map;
	};
};

//---------------------------------------------------------------------------
#endif
