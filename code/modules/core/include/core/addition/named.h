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
		named(const string & name) : _name(name) {}
		named(string && name) : _name(forward<string>(name)) {}
		named(const named & obj) : _name(obj._name) {}
		named(named && obj) : _name(move(obj._name)) {}

		virtual ~named() {}

        named & operator = (const named & obj)
        {
            return *this;
        }

		const string & name() const
		{
			return _name;
		}

		virtual void setName(const string & name)
		{
			_name = name;
		}

	private:
		string _name;
    };
}

//---------------------------------------------------------------------------
#endif
