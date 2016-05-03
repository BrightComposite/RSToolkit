//---------------------------------------------------------------------------

#ifndef CONTAINER_H
#define CONTAINER_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	type_checker(is_iterable, iterator);

	template<typename T, useif <is_iterable<T>::value> endif>
	typename T::iterator erase(T & container, int pos)
	{
		return container.erase(container.begin() + pos);
	}
}

//---------------------------------------------------------------------------
#endif
