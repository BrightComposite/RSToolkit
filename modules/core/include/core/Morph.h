//---------------------------------------------------------------------------

#ifndef MORPH_H
#define MORPH_H

//---------------------------------------------------------------------------

#include <core/container/Map.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	link_class(Morph, Class<Object>);

	class Morph : public Object
	{
		Map<String, Object> properties;

	public:
		Morph() : Object(), properties()
		{
			setclass(Morph);
		}

		virtual ~Morph() {}

		template<class T>
		Handle<T> property(const String & key) const
		{
			return handle_cast<T>(properties[key]);
		}

		void setProperty(const String & key, const Handle<Object> & value)
		{
			properties.insert_or_assign(key, value);
		}

		void clearProperty(const String & key)
		{
			properties.erase(key);
		}
	};

};

//---------------------------------------------------------------------------
#endif
