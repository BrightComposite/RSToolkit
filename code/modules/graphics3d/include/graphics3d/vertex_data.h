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
        using vertex_index_t = uint16_t;
        using vertex_indices = array_list<vertex_index_t>;
    }
}

//---------------------------------------------------------------------------
#endif
