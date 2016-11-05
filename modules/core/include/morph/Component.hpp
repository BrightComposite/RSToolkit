//---------------------------------------------------------------------------

#pragma once

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

//---------------------------------------------------------------------------

#include "Morph.hpp"

//---------------------------------------------------------------------------

#define components_origin(origin) morph_origin(origin)
#define create_components_from(module, .../*base class*/) create_morph_pool(module, __VA_ARGS__)
#define create_component(module, .../*component class*/) create_morph_type(module, __VA_ARGS__)

#define bind_components(cl, origin) \
	using bound_components = ComponentSet<origin, cl>; \
	Unique<bound_components, cl> components = Unique<bound_components, cl>(this);

namespace asd
{
	type_checker(has_components, bound_components);
}

//---------------------------------------------------------------------------
#endif
