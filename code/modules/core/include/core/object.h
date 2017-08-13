//---------------------------------------------------------------------------

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

//---------------------------------------------------------------------------

#include <core/handle.h>
#include <core/meta_class.h>

//---------------------------------------------------------------------------

namespace asd
{
#define setclass(...) this->_class = getclass(__VA_ARGS__)
	
	link_class(core) (object, basic_class);
	
	class object : public shareable
	{
	public:
		object() : _class(getclass(object)) {}
		
		const char * class_name() const {
			return _class->name();
		}
		
		const basic_class * get_class() const {
			return _class;
		}
		
		bool kind_of(const basic_class * meta) const {
			return _class->kind_of(meta);
		}
		
		bool instance_of(const basic_class * meta) const {
			return _class->instance_of(meta);
		}
	
	protected:
		const basic_class * _class;
	};
	
	template<class T>
	using is_object = based_on<T, object>;
}

//---------------------------------------------------------------------------
#endif
