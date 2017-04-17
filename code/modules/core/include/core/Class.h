//---------------------------------------------------------------------------

#pragma once

#ifndef CLASS_H
#define CLASS_H

//---------------------------------------------------------------------------

#include <functional>
#include <array>

#include <meta/Meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	using std::array;

	class Object;
	class String;

	template<class ... Parents>
	class Class {};

	using MetaClass = Class<>;

	template<>
	class Class<>
	{
	protected:
		const char * _name;

	public:
		Class(const char * name) : _name(name) {}
		virtual ~Class() {}

		const char * name() const
		{
			return _name;
		}

		bool instanceOf(const MetaClass * metaClass) const
		{
			return this == metaClass;
		}

		virtual bool kindOf(const MetaClass * metaClass) const
		{
			return this == metaClass;
		}

		virtual const MetaClass * superClass() const
		{
			return nullptr;
		}

		virtual void forEachClass(const std::function<void(const MetaClass *)> & func) const
		{
			func(this);
		}

	protected:
		virtual void addInfo(String &, const Object &) const {}
		friend api(core) void print(String &, const Object &);
	};

	template<class T>
	struct ClassInstance
	{
		typedef MetaClass Class;
	};

#define getclass(...)	   (&ClassInstance<__VA_ARGS__>::instance)
#define classname(...)	   getclass(__VA_ARGS__)->name()

#define instanceof(...)  ->instanceOf(getclass(__VA_ARGS__))
#define kindof(...)		 ->kindOf(getclass(__VA_ARGS__))
#define notinstanceof(...) instanceof(__VA_ARGS__) == false
#define notkindof(...)     kindof(__VA_ARGS__) == false

	struct KindOfClass
	{
		template<class Parent>
		static bool iterate(const MetaClass * meta)
		{
			return getclass(Parent)->kindOf(meta);
		}
	};

	struct ForEachMetaClass
	{
		template<class Parent>
		static void iterate(const std::function<void(const MetaClass *)> & func)
		{
			getclass(Parent)->forEachClass(func);
		}
	};

	template<class FirstParent, class ... OtherParents>
	class Class<FirstParent, OtherParents...> : public MetaClass
	{
		using ParentsLoop = foreach_t<FirstParent, OtherParents...>;

	public:
		Class(const char * name) : MetaClass(name) {}
		virtual ~Class() {}

		virtual const MetaClass * superClass() const override
		{
			return getclass(FirstParent);
		}

		virtual bool kindOf(const MetaClass * metaClass) const override
		{
			if(this == metaClass)
				return true;

			return ParentsLoop::template iterate<KindOfClass, false>(metaClass);
		}

		virtual void forEachClass(const std::function<void(const MetaClass *)> & func) const override
		{
			func(this);
			ParentsLoop::template iterate<ForEachMetaClass>(func);
		}
	};

#define link_class(module, cl, /* class */...)		\
	template<>										\
	struct ClassInstance<cl>						\
	{												\
		static __VA_ARGS__ api(module) instance;	\
	}

#define implement_link(...)							\
	decltype(ClassInstance<__VA_ARGS__>::instance) ClassInstance<__VA_ARGS__>::instance(#__VA_ARGS__)
}

//---------------------------------------------------------------------------
#endif
