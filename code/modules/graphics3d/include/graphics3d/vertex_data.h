//---------------------------------------------------------------------------

#pragma once

#ifndef VERTEX_DATA_H
#define VERTEX_DATA_H

//---------------------------------------------------------------------------

#include <container/array_list.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace gfx3d
	{
		using vertex_data = array_list<float>;
		using vertex_indices = array_list<uint>;
	}
}

//---------------------------------------------------------------------------
#endif
