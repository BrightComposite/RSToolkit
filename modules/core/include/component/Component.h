//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

//---------------------------------------------------------------------------

#include <component/Morpher.h>
#include <core/Exception.h>
#include <container/ArrayList.h>

//---------------------------------------------------------------------------

#include <component/Component.hpp>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Component : public Object
	{
		template<class, class>
		friend class ComponentSet;

	public:
		bool hasLinks() const
		{
			return !_links.empty();
		}

		String printLinks() const
		{
			String s;

			for(auto i = _links.begin(); i != _links.end(); ++i)
			{
				if(i != _links.begin())
					s << ", ";

				s << valueof(i)->className();
			}

			return s;
		}

		template<class C, useif<based_on<C, Component>::value>>
		void link(C * c)
		{
			_links[MorphType<C>::id()] = c;
		}

		template<class C, useif<based_on<C, Component>::value>>
		void unlink(C *)
		{
			_links.erase(MorphType<C>::id());
		}

	protected:
		dictionary<int, Component *> _links;
	};

	class LinkedComponentException : public Exception
	{
	public:
		LinkedComponentException(Component * c) :
			Exception("Can't remove component of the ", c->className(), " class! Linked components: ", c->printLinks()) {}

		virtual ~LinkedComponentException() {}
	};

	template<class Base, class Context>
	class ComponentSet
	{
	public:
		static_assert(can_clone<Base, Context *>::value, "A Base class must accept a context");
		
		ComponentSet(Context * ctx) : ctx(ctx) {}

		ComponentSet(Context * ctx, const ComponentSet & set) : ctx(ctx), map(set.map)
		{
			for(auto & entry : set.map)
				map.insert({entry.first, entry.second->clone(ctx)});
		}

		void set(const ComponentSet & set)
		{
			map.clear();

			for(auto & entry : set.map)
				map.insert({entry.first, entry.second->clone(ctx)});
		}

		template<class T, class InitType = T, useif<based_on<T, Base>::value, based_on<InitType, T>::value, can_construct<InitType, Context *>::value>>
		Handle<T> require()
		{
			auto & p = map[MorphType<T>::id()];

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<InitType> c;
			p = c.init(ctx);
			return c;
		}

		template<class T, useif<based_on<T, Base>::value>>
		void set(const Handle<T> & component)
		{
			map[MorphType<T>::id()] = component;
		}

		template<class T, class C, useif<based_on<T, Base>::value, can_construct<T, Context *>::value>>
		Handle<T> link(C * component)
		{
			auto c = require<T>();
			c->link(component);
			return c;
		}

		template<class T, class C, useif<based_on<T, Base>::value, can_construct<T, Context *>::value>>
		void unlink(C * component)
		{
			auto c = seek<T>();

			if(c != nullptr)
				c->unlink(component);
		}

		template<class T, useif<based_on<T, Base>::value>>
		Handle<T> seek() const
		{
			auto i = map.find(MorphType<T>::id());

			if(i == map.end())
				return nullptr;

			return handle_cast<T>(i->second);
		}

		template<class T, useif<based_on<T, Base>::value>>
		void remove()
		{
			auto i = map.find(MorphType<T>::id());

			if(i == map.end())
				return;

			auto & h = valueof(i);

			if(h != nullptr && h->hasLinks())
				throw LinkedComponentException(h);

			map.erase(i);
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
}

//---------------------------------------------------------------------------
#endif
