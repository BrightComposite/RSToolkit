//---------------------------------------------------------------------------

#ifndef NAMED_H
#define NAMED_H

//---------------------------------------------------------------------------

#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture {

	class NamedClass;

	link_class(Named, MetaBase);

    class Named
    {
	public:
		Named() : name("unnamed") {}
		Named(const String & name) : name(name) {}
		Named(String && name) : name(forward<String>(name)) {}
		Named(const Named & obj) : name(obj.name) {}
		Named(Named && obj) : name(move(obj.name)) {}

		virtual ~Named() {}

        Named & operator = (const Named & obj)
        {
            return *this;
        }

		String name;
    };

	class NamedClass : public MetaClass<>
	{
	public:
		NamedClass(const char * name) : MetaClass<>(name) {}
		NamedClass(const NamedClass & meta) : MetaClass<>(meta) {}

	protected:
		virtual void addInfo(String & target, const Object & obj) const override
		{
			target << '\"' << dynamic_cast<const Named &>(obj).name << '\"' << ' ';
		}
	};
}

//---------------------------------------------------------------------------
#endif
