//---------------------------------------------------------------------------

#pragma once

#ifndef NAMED_H
#define NAMED_H

//---------------------------------------------------------------------------

#include <core/String.h>

//---------------------------------------------------------------------------

namespace asd {

	class named;

	link_class(core) (named, basic_class);

    class named
    {
	public:
		named() : _name("unnamed") {}
		named(const String & name) : _name(name) {}
		named(String && name) : _name(forward<String>(name)) {}
		named(const named & obj) : _name(obj._name) {}
		named(named && obj) : _name(move(obj._name)) {}

		virtual ~named() {}

        named & operator = (const named & obj)
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

	private:
		String _name;
    };
}

//---------------------------------------------------------------------------
#endif
