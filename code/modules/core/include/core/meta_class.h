//---------------------------------------------------------------------------

#pragma once

#ifndef META_CLASS_H
#define META_CLASS_H

//---------------------------------------------------------------------------

#include <functional>
#include <array>

#include <meta/meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	using std::array;
	
	class object;
	
	class String;
	
	template<class ... Parents>
	class meta_class {};
	
	using basic_class = meta_class<>;
	
	template<>
	class meta_class<>
	{
	protected:
		const char * _name;
	
	public:
		meta_class(const char * name) : _name(name) {}
		
		virtual ~meta_class() {}
		
		const char * name() const {
			return _name;
		}
		
		bool instance_of(const basic_class * meta) const {
			return this == meta;
		}
		
		virtual bool kind_of(const basic_class * meta) const {
			return this == meta;
		}
		
		virtual const basic_class * super() const {
			return nullptr;
		}
		
		virtual void foreach(const std::function<void(const basic_class *)> & f) const {
			f(this);
		}
	
	protected:
		virtual void add_info(String &, const object &) const {}
		
		friend api(core) void print(String &, const object &);
	};
	
	template<class T>
	struct class_instance
	{
		static constexpr bool linked = false;
		
		template<class U = T>
		static basic_class * instance() {
			static_assert(class_instance<U>::linked, "Class T wasn't linked");
			return nullptr;
		};
	};

#define getclass(...)       (class_instance<__VA_ARGS__>::instance())
#define classname(...)      getclass(__VA_ARGS__)->name()

#define instanceof(...)    ->instance_of(getclass(__VA_ARGS__))
#define kindof(...)         ->kind_of(getclass(__VA_ARGS__))
#define notinstanceof(...)    instanceof(__VA_ARGS__) == false
#define notkindof(...)        kindof(__VA_ARGS__) == false
	
	struct KindOfClass
	{
		template<class Parent>
		static bool iterate(const basic_class * meta) {
			return getclass(Parent)->kind_of(meta);
		}
	};
	
	struct ForEachMetaClass
	{
		template<class Parent>
		static void iterate(const std::function<void(const basic_class *)> & func) {
			getclass(Parent)->foreach(func);
		}
	};
	
	template<class FirstParent, class ... OtherParents>
	class meta_class<FirstParent, OtherParents...> : public basic_class
	{
		using ParentsLoop = foreach_t<FirstParent, OtherParents...>;
	
	public:
		meta_class(const char * name) : basic_class(name) {}
		
		virtual ~meta_class() {}
		
		virtual const basic_class * super() const override {
			return getclass(FirstParent);
		}
		
		virtual bool kind_of(const basic_class * meta) const override {
			if(this == meta) {
				return true;
			}
			
			return ParentsLoop::template iterate<KindOfClass, false>(meta);
		}
		
		virtual void foreach(const std::function<void(const basic_class *)> & f) const override {
			f(this);
			ParentsLoop::template iterate<ForEachMetaClass>(f);
		}
	};

#define _link_class_impl(cl, /* class */...)    \
    class_instance<cl>                            \
    {                                           \
        static constexpr bool linked = true;    \
                                                \
        static __VA_ARGS__ * instance() {       \
            static __VA_ARGS__ i(#cl);            \
            return &i;                            \
        };                                    \
    }

#define link_class(module)            \
    template<>                      \
    struct api(module) _link_class_impl
}

//---------------------------------------------------------------------------
#endif
