//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H

//---------------------------------------------------------------------------

#include "morph.h"
#include <core/Exception.h>
#include <container/array_list.h>

//---------------------------------------------------------------------------

#include "component.hpp"

//---------------------------------------------------------------------------

namespace asd
{
	class component : public object
	{
		template<class, class>
		friend class component_set;
		
	public:
		bool hasLinks() const
		{
			return !_links.empty();
		}

		string printLinks() const
		{
			std::stringstream ss;

			for(auto i = _links.begin(); i != _links.end(); ++i)
			{
				if(i != _links.begin())
					ss << ", ";

				ss << valueof(i)->class_name();
			}

			return ss.str();
		}

		template<class C, useif<based_on<C, component>::value>>
		void link(C * c)
		{
			_links[morph_type<C>::id()] = c;
		}

		template<class C, useif<based_on<C, component>::value>>
		void unlink(C *)
		{
			_links.erase(morph_type<C>::id());
		}

	protected:
		dictionary<int, component *> _links;
	};

	class LinkedComponentException : public Exception
	{
	public:
		LinkedComponentException(component * c) :
			Exception("Can't remove component of the ", c->class_name(), " class! Linked components: ", c->printLinks()) {}

		virtual ~LinkedComponentException() {}
	};

	template<class Origin, class Context>
	class component_set : public morph<Origin, Context>
	{
		using base = morph<Origin, Context>;

	public:
		static_assert(can_clone<Origin, Context *>::value, "A Origin class must accept a context");
		
		component_set(Context * ctx) : base(ctx) {}

		component_set(Context * ctx, const component_set & set) : base(ctx)
		{
			for(auto & entry : set._map)
				this->_map.insert({entry.first, entry.second->clone(ctx)});
		}

		using base::set;

		void set(const component_set & set)
		{
			this->_map.clear();

			for(auto & entry : set._map)
				this->_map.insert({entry.first, entry.second->clone(this->_ctx)});
		}

		template<class T, class C, useif<based_on<T, Origin>::value, can_construct<T, Context *>::value>>
		handle<T> link(C * component)
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
			auto i = this->_map.find(morph_id(T));

			if(i == this->_map.end())
				return;

			auto & h = valueof(i);

			if(h != nullptr && h->hasLinks())
				throw LinkedComponentException(h);
			
			this->_map.erase(i);
		}
	};

	namespace cast
	{
		template<class T, class Y, skipif<is_pointer<Y>::value>, useif<has_components<Y>::value>>
		inline handle<T> as(Y && x)
		{
			return forward<Y>(x).components->template require<T>();
		}

		template<class T, class Y, useif<has_components<Y>::value>>
		inline handle<T> as(Y * x)
		{
			return x->components->template require<T>();
		}
	}
}

//---------------------------------------------------------------------------
#endif
