//---------------------------------------------------------------------------

#ifndef META_CLASS_H
#define META_CLASS_H

//---------------------------------------------------------------------------

#include <functional>
#include <array>

#include <core/meta/Meta.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::array;

	class Object;
	class String;

	template<class ... Parents>
	class MetaClass {};

	typedef MetaClass<> MetaBase;

	template<>
	class MetaClass<>
	{
	protected:
		const char * _name;

	public:
		MetaClass(const char * name) : _name(name) {}
		virtual ~MetaClass() {}

		const char * name() const
		{
			return _name;
		}

		bool instanceOf(const MetaBase * metaClass) const
		{
			return this == metaClass;
		}

		virtual bool kindOf(const MetaBase * metaClass) const
		{
			return instanceOf(metaClass);
		}

		virtual const MetaBase * superClass() const
		{
			return nullptr;
		}

		virtual void forEachClass(const std::function<void(const MetaBase *)> & func) const
		{
			func(this);
		}

	protected:
		virtual void addInfo(String &, const Object &) const {}
		friend void print(String &, const Object &);
	};

	template<class T>
	struct ClassInstance
	{
		typedef MetaBase Class;
	};

#define getclass(...)	ClassInstance<__VA_ARGS__>::instance()
#define classname(...)	getclass(__VA_ARGS__)->name()

#define instanceof(...) ->instanceOf(getclass(__VA_ARGS__))
#define kindof(...)     ->kindOf(getclass(__VA_ARGS__))
#define notinstanceof(...) instanceof(__VA_ARGS__) == false
#define notkindof(...)     kindof(__VA_ARGS__) == false

	template<class Parent>
	class MetaClass<Parent> : public MetaBase
	{
	protected:
		const MetaBase * _parent;

	public:
		MetaClass(const char * name) : MetaBase(name), _parent(getclass(Parent)) {}
		virtual ~MetaClass() {}

		virtual const MetaBase * superClass() const override
		{
			return _parent;
		}

		virtual bool kindOf(const MetaBase * metaClass) const override
		{
			if(instanceOf(metaClass))
				return true;

			return _parent != nullptr ? _parent->kindOf(metaClass) : false;
		}

		virtual void forEachClass(const std::function<void(const MetaBase *)> & func) const override
		{
			func(this);
			_parent->forEachClass(func);
		}
	};

	template<class Class>
	inline const MetaBase * getMetaClass()
	{
		return ClassInstance<Class>::instance();
	}

	template<class FirstParent, class SecondParent, class ... OtherParents>
	class MetaClass<FirstParent, SecondParent, OtherParents...> : public MetaBase
	{
	public:
		static const int parentsCount = 2 + sizeof...(OtherParents);

		MetaClass(const char * name) : MetaBase(name), _parents
		{
			getMetaClass<FirstParent>(),
			getMetaClass<SecondParent>(),
			getMetaClass<OtherParents>()...
		} {}

		virtual ~MetaClass() {}

		virtual const MetaBase * superClass() const override
		{
			return _parents[0];
		}

		virtual bool kindOf(const MetaBase * metaClass) const override
		{
			if(instanceOf(metaClass))
				return true;

			for(auto & parent : _parents)
			{
				if(parent->kindOf(metaClass))
					return true;
			}

			return false;
		}

		virtual void forEachClass(const std::function<void(const MetaBase *)> & func) const override
		{
			func(this);

			for(auto & parent : _parents)
				parent->forEachClass(func);
		}

	protected:
		array<const MetaBase *, parentsCount> _parents;
	};

#define link_class(cl, /* metaclass */...)	\
	class cl;								\
											\
	template<>								\
	struct ClassInstance<cl>				\
	{										\
		typedef __VA_ARGS__ Class;			\
		static const MetaBase * instance()	\
		{									\
			static const Class meta(#cl);	\
			return &meta;					\
		}									\
	};

#define expand_template(type, /* args */...) type<__VA_ARGS__>
#define template_type(type, /* args */...) type

#define link_template_class(cl, /* metaclass */...)		\
	template<>											\
	struct Rapture::ClassInstance<expand_template cl>	\
	{													\
		typedef __VA_ARGS__ Class;						\
		static const MetaBase * instance()				\
		{												\
			static const Class meta(					\
				pp_string(template_type cl)				\
			);											\
														\
			return &meta;								\
		}												\
	};
}

//---------------------------------------------------------------------------
#endif