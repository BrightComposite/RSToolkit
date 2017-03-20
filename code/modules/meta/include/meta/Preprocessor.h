//---------------------------------------------------------------------------

#pragma once

#ifndef META_PREPROCESSOR_H
#define META_PREPROCESSOR_H

//---------------------------------------------------------------------------

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/remove.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>

#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/preprocessor/repetition/enum.hpp>

// BOOST_PP names are too long

#define pp_if					BOOST_PP_IF
#define pp_empty				BOOST_PP_EMPTY

#define pp_is_empty				BOOST_PP_IS_EMPTY

#define pp_seq_size				BOOST_PP_SEQ_SIZE
#define pp_seq_elem				BOOST_PP_SEQ_ELEM
#define pp_seq_enum				BOOST_PP_SEQ_ENUM
#define pp_seq_fold(op, seq)	BOOST_PP_SEQ_FOLD_LEFT(op, , seq)
#define pp_seq_transform		BOOST_PP_SEQ_TRANSFORM
#define pp_seq_foreach			BOOST_PP_SEQ_FOR_EACH
#define pp_seq_to_tuple			BOOST_PP_SEQ_TO_TUPLE

#define pp_tuple_size			BOOST_PP_TUPLE_SIZE
#define pp_tuple_elem			BOOST_PP_TUPLE_ELEM
#define pp_tuple_enum			BOOST_PP_TUPLE_ENUM
#define pp_tuple_remove			BOOST_PP_TUPLE_REMOVE
#define pp_tuple_to_seq(tuple)	BOOST_PP_TUPLE_TO_SEQ(pp_tuple_size(tuple), tuple)

#define pp_tuple_extract(index, tuple)	pp_tuple_elem(pp_tuple_size(tuple), index, tuple)
#define pp_tuple_head(tuple)	pp_tuple_extract(0, tuple)
#define pp_tuple_tail(tuple)	pp_tuple_remove(tuple, 0)

#define pp_cat					BOOST_PP_CAT
#define pp_string				BOOST_PP_STRINGIZE
#define pp_enum					BOOST_PP_ENUM

#define pp_expand(...)			__VA_ARGS__
#define pp_eat(...)
#define pp_zero(...)			0

#define pp_enum_prepend(z, n, text) text n

#define pp_seq_expand(op, seq)	pp_seq_enum(pp_seq_transform(op, ~, seq))

#define pack_sequence0(...) ((__VA_ARGS__)) pack_sequence1
#define pack_sequence1(...) ((__VA_ARGS__)) pack_sequence0
#define pack_sequence0_end
#define pack_sequence1_end

#define pp_seq_pack(seq) pp_cat(pack_sequence0 seq, _end)

#define pp_seq_elem_t(seq)		pp_seq_elem(0, seq)
#define pp_seq_elem_n(seq)		pp_seq_elem(1, seq)
#define pp_seq_elem_d(seq)		pp_seq_elem_t(seq) pp_seq_elem_n(seq)

#define pp_tuple_elem_t(...)		pp_tuple_elem(2, 0, __VA_ARGS__)
#define pp_tuple_elem_n(...)		pp_tuple_elem(2, 1, __VA_ARGS__)
#define pp_tuple_elem_d(...)		pp_tuple_elem_t(__VA_ARGS__) pp_tuple_elem_n(__VA_ARGS__)

#define op_seq_elem_t(z, data, seq)	pp_seq_elem_t(seq)
#define op_seq_elem_n(z, data, seq)	pp_seq_elem_n(seq)
#define op_seq_elem_d(z, data, seq)	pp_seq_elem_d(seq)

#define op_arr_elem_t(z, data, tpl)	pp_tuple_elem_t(tpl)
#define op_arr_elem_n(z, data, tpl)	pp_tuple_elem_n(tpl)
#define op_arr_elem_d(z, data, tpl)	pp_tuple_elem_d(tpl)

#define op_fields(z, data, ...) pp_tuple_elem_d(__VA_ARGS__);
#define op_fields_types(z, data, ...) pp_tuple_elem_t(__VA_ARGS__)

#define pp_seq_fields0(fields)			pp_seq_foreach(op_fields, ~, pp_seq_pack(fields))
#define pp_seq_fields_types0(fields)	pp_seq_expand(op_fields_types, pp_seq_pack(fields))
#define pp_seq_fields_count0(fields)	pp_seq_size(pp_seq_pack(fields))

#define pp_seq_fields(... /*fields*/)		pp_if(pp_is_empty(__VA_ARGS__), pp_eat, pp_seq_fields0) (__VA_ARGS__)
#define pp_seq_fields_types(... /*fields*/)	pp_if(pp_is_empty(__VA_ARGS__), pp_eat, pp_seq_fields_types0) (__VA_ARGS__)
#define pp_seq_fields_count(... /*fields*/)	pp_if(pp_is_empty(__VA_ARGS__), pp_zero, pp_seq_fields_count0) (__VA_ARGS__)

//---------------------------------------------------------------------------
#endif
