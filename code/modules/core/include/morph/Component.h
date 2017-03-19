//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

//---------------------------------------------------------------------------

#include "Morph.h"
#include <core/Exception.h>
#include <container/ArrayList.h>

//---------------------------------------------------------------------------

#include "Component.hpp"

//---------------------------------------------------------------------------

namespace asd
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

	template<class Origin, class Context>
	class ComponentSet : public Morph<Origin, Context>
	{
		using Base = Morph<Origin, Context>;

	public:
		static_assert(can_clone<Origin, Context *>::value, "A Origin class must accept a context");
		
		ComponentSet(Context * ctx) : Base(ctx) {}

		ComponentSet(Context * ctx, const ComponentSet & set) : Base(ctx)
		{
			for(auto & entry : set.map)
				this->map.insert({entry.first, entry.second->clone(ctx)});
		}

		using Base::set;

		void set(const ComponentSet & set)
		{
			this->map.clear();

			for(auto & entry : set.map)
				this->map.insert({entry.first, entry.second->clone(this->ctx)});
		}

		template<class T, class C, useif<based_on<T, Origin>::value, can_construct<T, Context *>::value>>
		Handle<T> link(C * component)
		{
			auto c = this->template require<T>();
			c->link(component);
			return c;
		}

		template<class T, class C, useif<based_on<T, Origin>::value, can_construct<T, Context *>::value>>
		void unlink(C * component)
		{
			auto c = this->template seek<T>();

			if(c != nullptr)
				c->unlink(component);
		}

		template<class T, useif<based_on<T, Origin>::value>>
		void remove()
		{
			auto i = this->map.find(morphid(T));

			if(i == this->map.end())
				return;

			auto & h = valueof(i);

			if(h != nullptr && h->hasLinks())
				throw LinkedComponentException(h);
			
			this->map.erase(i);
		}
	};

	namespace cast
	{
		template<class T, class Y, selectif(0)<!is_pointer<Y>::value>, useif<has_components<Y>::value>>
		inline Handle<T> as(Y && x)
		{
			return forward<Y>(x).components->template require<T>();
		}

		template<class T, class Y, selectif(1)<is_pointer<Y>::value>, useif<has_components<std::remove_pointer_t<Y>>::value>>
		inline Handle<T> as(Y x)
		{
			return x->components->template require<T>();
		}
	}
}

//---------------------------------------------------------------------------
#endif
