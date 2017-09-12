//---------------------------------------------------------------------------

#pragma once

#ifndef PROPERTY_HPP
#define PROPERTY_HPP

//---------------------------------------------------------------------------

#include "morph.hpp"

//---------------------------------------------------------------------------


#define properties_origin(origin) morph_origin(origin)
#define create_properties_from(module, .../*base class*/) create_morph_pool(module, __VA_ARGS__)
#define create_property(module, .../*component class*/) create_morph_type(module, __VA_ARGS__)

#define bind_properties(clazz, origin)								\
	using bound_properties = PropertiesSet<origin>;				\
	unique<bound_properties> properties = {_};	\


//---------------------------------------------------------------------------
#endif
