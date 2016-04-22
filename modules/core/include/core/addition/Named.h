//---------------------------------------------------------------------------

#ifndef NAMED_H
#define NAMED_H

//---------------------------------------------------------------------------

#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture {

	class NamedClass;

	link_class(Named, MetaClass);

    class Named
    {
	public:
		Named() : _name("unnamed") {}
		Named(const String & name) : _name(name) {}
		Named(String && name) : _name(forward<String>(name)) {}
		Named(const Named & obj) : _name(obj._name) {}
		Named(Named && obj) : _name(move(obj._name)) {}

		virtual ~Named() {}

        Named & operator = (const Named & obj)
        {
            return *this;
        }

		const String & name() const
		{
			return _name;
		}

		virtual void setName(const String & name)
		{
			_name = name;
		}

	protected:
		String _name;
    };

	class NamedClass : public Class<>
	{
	public:
		NamedClass(const char * name) : Class<>(name) {}
		NamedClass(const NamedClass & meta) : Class<>(meta) {}

	protected:
		virtual void addInfo(String & target, const Object & obj) const override
		{
			target << '\"' << dynamic_cast<const Named &>(obj).name() << '\"' << ' ';
		}
	};
}

//---------------------------------------------------------------------------
#endif
