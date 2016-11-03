//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

//---------------------------------------------------------------------------

#include <component/Morpher.hpp>

//---------------------------------------------------------------------------

#define component_base(Base) morph_base(Base)
#define create_component_pool(module, .../*base class*/) create_morph_pool(module, __VA_ARGS__)
#define create_component(module, .../*component class*/) create_morph_type(module, __VA_ARGS__)

namespace Rapture
{
	method_checker(can_clone, clone);
}

//---------------------------------------------------------------------------
#endif
