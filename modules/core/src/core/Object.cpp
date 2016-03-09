//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/meta/Meta.h>

#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	int Object::compare(const Object * obj) const
	{
		return strcmp(className(), obj->className());
	}
}