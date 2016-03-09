//---------------------------------------------------------------------------

#ifndef OBJECT_H
#define OBJECT_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/MetaClass.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#define setclass(...) this->_metaClass = getclass(__VA_ARGS__)

	link_class(Object, MetaBase)

	class Object : public Shareable<Object>
    {
    public:
		Object() : _metaClass(getclass(Object)) {}
		Object(const Object & obj) : _metaClass(getclass(Object)) {}

		virtual ~Object() {}

        Object & operator = (const Object & obj)
        {
            return *this;
        }

        virtual int compare(const Object * obj) const;

        bool equals(const Object * obj) const
        {
            return compare(obj) == 0;
        }

		bool operator == (const Object & obj) const
		{
			return this == &obj;
		}

		bool operator > (const Object & obj) const
		{
			return compare(&obj) > 0;
		}

		bool operator < (const Object & obj) const
		{
			return compare(&obj) < 0;
		}

		bool operator >= (const Object & obj) const
		{
			return compare(&obj) >= 0;
		}

		bool operator <= (const Object & obj) const
		{
			return compare(&obj) <= 0;
		}

        const char * className() const
        {
            return _metaClass->name();
        }

		const MetaBase * getClass() const
		{
			return _metaClass;
		}

        bool kindOf(const MetaBase * metaClass) const
        {
            return _metaClass->kindOf(metaClass);
        }

        bool instanceOf(const MetaBase * metaClass) const
        {
            return _metaClass->instanceOf(metaClass);
        }

	protected:
		const MetaBase * _metaClass;
    };
}

//---------------------------------------------------------------------------
#endif
