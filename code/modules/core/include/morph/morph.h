//---------------------------------------------------------------------------

#pragma once

#ifndef MORPH_H
#define MORPH_H

//---------------------------------------------------------------------------

#include <container/map.h>
#include <core/object.h>
#include <atomic>

//---------------------------------------------------------------------------

#include <morph/morph.hpp>

//---------------------------------------------------------------------------

namespace asd
{
	template<class Origin, class ... Context>
	class morph
	{
	public:
		static_assert(is_morph_type<Origin>::value, "Morph can only use morph types marked with the 'morph_origin' macro");

		template<class T, useif<based_on<T, Origin>::value>>
		void set(const handle<T> & h)
		{
			_map[morph_id<T>] = h;
		}

		template<class T, class InitType = T, useif<based_on<T, Origin>::value, based_on<InitType, T>::value, can_construct<InitType>::value>>
		handle<T> require()
		{
			auto & p = _map[morph_id<T>];

			if(p != nullptr)
				return cast::as<T>(p);

			handle<InitType> h;
			p = h.init();
			return h;
		}

		template<class T, useif<based_on<T, Origin>::value>>
		handle<T> seek() const
		{
			auto i = _map.find(morph_id<T>);
			return i != _map.end() ? cast::as<T>(i->second) : nullptr;
		}

		template<class T, useif<based_on<T, Origin>::value>>
		void remove()
		{
			_map.erase(morph_id<T>);
		}

		void clear()
		{
			return _map.clear();
		}

		size_t size() const
		{
			return _map.size();
		}

	protected:
		map<morph_id_t, handle<Origin>> _map;
	};

	template<class Origin, class Context>
	class morph<Origin, Context>
	{
	public:
		static_assert(is_morph_type<Origin>::value, "Morph can only use morph types marked with the 'morph_origin' macro");

		morph(Context * ctx) : _ctx(ctx) {}

		template<class T, useif<based_on<T, Origin>::value>>
		void set(const handle<T> & h)
		{
			_map[morph_id<T>] = h;
		}

		template<class T, class InitType = T, useif<based_on<T, Origin>::value, based_on<InitType, T>::value, can_construct<InitType, Context *>::value>>
		handle<T> require()
		{
			auto & p = _map[morph_id<T>];

			if(p != nullptr)
				return cast::as<T>(p);

			handle<InitType> h;
			p = h.init(_ctx);
			return h;
		}

		template<class T, useif<based_on<T, Origin>::value>>
		handle<T> seek() const
		{
			auto i = _map.find(morph_id<T>);

			if(i == _map.end())
				return nullptr;

			return cast::as<T>(i->second);
		}

		template<class T, useif<based_on<T, Origin>::value>>
		void remove()
		{
			_map.erase(morph_id<T>);
		}

		void clear()
		{
			return _map.clear();
		}

		size_t size() const
		{
			return _map.size();
		}

	protected:
		Context * _ctx;
		map<morph_id_t, handle<Origin>> _map;
	};
};

//---------------------------------------------------------------------------
#endif
