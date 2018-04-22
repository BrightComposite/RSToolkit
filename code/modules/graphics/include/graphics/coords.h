//---------------------------------------------------------------------------

#pragma once

#ifndef COORDS_H
#define COORDS_H

//---------------------------------------------------------------------------

#include <math/rect.h>

//---------------------------------------------------------------------------

namespace asd
{
    namespace gfx
    {
        using coord = float;
        using area = math::rect<coord>;
        using position = typename area::point_type;
        using size = typename area::size_type;
        using range = typename area::range_type;
    }
}

//---------------------------------------------------------------------------
#endif
