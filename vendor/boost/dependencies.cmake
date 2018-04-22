#--------------------------------------------------------
#	dependencies file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

set(DEPENDENCIES_align core static_assert throw_exception)
set(DEPENDENCIES_assert config)
set(DEPENDENCIES_algorithm exception iterator numeric/conversion range regex unordered)
set(DEPENDENCIES_asio date_time)
set(DEPENDENCIES_core assert)
set(DEPENDENCIES_conversion smart_ptr typeof)
set(DEPENDENCIES_container intrusive)
set(DEPENDENCIES_date_time lexical_cast tokenizer)
set(DEPENDENCIES_filesystem algorithm io iterator system)
set(DEPENDENCIES_function bind integer move mpl type_index typeof)
set(DEPENDENCIES_function_types core detail mpl type_traits)
set(DEPENDENCIES_functional iterator)
set(DEPENDENCIES_fusion functional)
set(DEPENDENCIES_iterator concept_check conversion function function_types fusion optional tuple)
set(DEPENDENCIES_lexical_cast array container math)
set(DEPENDENCIES_mpl predef preprocessor throw_exception type_traits utility)
set(DEPENDENCIES_regex container_hash iterator)
set(DEPENDENCIES_smart_ptr align type_traits)
set(DEPENDENCIES_static_assert config)
set(DEPENDENCIES_system core predef)
set(DEPENDENCIES_throw_exception assert)
set(DEPENDENCIES_tuple type_traits)
set(DEPENDENCIES_type_index mpl container_hash)
set(DEPENDENCIES_type_traits core static_assert)
set(DEPENDENCIES_utility mpl)

set(HEADER_array array.hpp)
set(HEADER_assert assert.hpp)
set(HEADER_conversion implicit_cast.hpp)
set(HEADER_lexical_cast lexical_cast.hpp)
set(HEADER_static_assert static_assert.hpp)
set(HEADER_throw_exception throw_exception.hpp)
set(HEADER_tokenizer tokenizer.hpp)

if (WIN32)
    set(DEPENDENCIES_system ${DEPENDENCIES_system} winapi)
endif()