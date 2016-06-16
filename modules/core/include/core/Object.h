//---------------------------------------------------------------------------

#ifndef OBJECT_H
#define OBJECT_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/Class.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#define setclass(...) this->_class = getclass(__VA_ARGS__)

	link_class(core, Object, MetaClass);

	class Object : public Shared
    {
    public:
		Object() : _class(getclass(Object)) {}

        const char * className() const
        {
            return _class->name();
        }

		const MetaClass * getClass() const
		{
			return _class;
		}

        bool kindOf(const MetaClass * metaClass) const
        {
            return _class->kindOf(metaClass);
        }

        bool instanceOf(const MetaClass * metaClass) const
        {
            return _class->instanceOf(metaClass);
        }

	protected:
		const MetaClass * _class;
    };
}

//---------------------------------------------------------------------------
#endif
