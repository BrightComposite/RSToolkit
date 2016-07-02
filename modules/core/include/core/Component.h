//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

//---------------------------------------------------------------------------

#include <core/Morpher.h>
#include <core/Exception.h>
#include <core/container/ArrayList.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#define component_base(Base) morph_base(Base)
#define create_component_pool(module, Base) create_morph_pool(module, Base)
#define create_component(module, Component) create_morph_type(module, Component)

	class Component : public Object
	{
		template<class, class ...>
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

				s << i->second->className();
			}

			return s;
		}

	protected:
		template<class C, useif <based_on<C, Component>::value> endif>
		void link(C * c)
		{
			_links[MorphType<C>::id()] = c;
		}

		template<class C, useif <based_on<C, Component>::value> endif>
		void unlink(C *)
		{
			_links.erase(MorphType<C>::id());
		}

		map<int, Component *> _links;
	};

	class LinkedComponentException : public Exception
	{
	public:
		LinkedComponentException(Component * c) :
			Exception("Can't remove component of the ", c->className(), " class! Linked components: ", c->printLinks()) {}

		virtual ~LinkedComponentException() {}
	};

	template<class Base, class ... Owner>
	class ComponentSet
	{
	public:
		template<class T, class Context>
		using is_context = is_same<decltype(declval<Context>().init(declval<Handle<T, Owner...> &>())), Handle<T, Owner...> &>;

		template<class T, class ... A, useif <based_on<T, Base>::value, can_construct<T, A...>::value> endif>
		Handle<T, Owner...> construct(A && ... args)
		{
			auto & p = map[MorphType<T>::id()];

			if(p != nullptr && p->hasLinks())
				throw LinkedComponentException(p);

			Handle<T, Owner...> c;
			p = c.init(forward<A>(args)...);
			return c;
		}

		template<class T, class Context, useif <based_on<T, Base>::value, is_context<T, Context>::value> endif>
		Handle<T, Owner...> construct(Context * ctx)
		{
			auto & p = map[MorphType<T>::id()];

			if(p != nullptr && p->hasLinks())
				throw LinkedComponentException(p);

			Handle<T, Owner...> c;
			p = ctx->init(c);
			return c;
		}

		template<class T, useif <based_on<T, Base>::value, can_construct<T>::value> endif>
		Handle<T, Owner...> require()
		{
			auto & p = map[MorphType<T>::id()];

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> c;
			p = c.init();
			return c;
		}

		template<class T, class Context, useif <based_on<T, Base>::value, is_context<T, Context>::value> endif>
		Handle<T, Owner...> require(Context * ctx)
		{
			auto & p = map[MorphType<T>::id()];

			if(p != nullptr)
				return handle_cast<T>(p);

			Handle<T, Owner...> c;
			p = ctx->init(c);
			return c;
		}

		template<class T, class C, useif <based_on<T, Base>::value, can_construct<T>::value> endif>
		Handle<T, Owner...> link(C * component)
		{
			auto c = require<T>();
			c->link(component);
			return c;
		}

		template<class T, class C, class Context, useif <based_on<T, Base>::value, is_context<T, Context>::value> endif>
		Handle<T, Owner...> link(C * component, Context * ctx)
		{
			auto c = require<T>(ctx);
			c->link(component);
			return c;
		}

		template<class T, class C, useif <based_on<T, Base>::value, can_construct<T>::value> endif>
		void unlink(C * component)
		{
			auto c = seek<T>();

			if(c != nullptr)
				c->unlink(component);
		}

		template<class T, useif <based_on<T, Base>::value> endif>
		Handle<T, Owner...> seek() const
		{
			auto i = map.find(MorphType<T>::id());

			if(i == map.end())
				return nullptr;

			return handle_cast<T>(i->second);
		}

		template<class T, useif <based_on<T, Base>::value> endif>
		void remove()
		{
			auto i = map.find(MorphType<T>::id());

			if(i == map.end())
				return;

			if(i->second != nullptr && i->second->hasLinks())
				throw LinkedComponentException(i->second);

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
		UnorderedMap<int, Base, Owner...> map;
	};
}

//---------------------------------------------------------------------------
#endif
