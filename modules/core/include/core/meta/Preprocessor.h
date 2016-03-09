//---------------------------------------------------------------------------

#ifndef DEFINE_PREPROCESSOR_H
#define DEFINE_PREPROCESSOR_H

//---------------------------------------------------------------------------

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/remove.hpp>

#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/preprocessor/repetition/enum.hpp>

// Now it helps only when parsing a lambda. But it helps :)

#define __extract(i, x)  __ ## i x
#define __0(_x, ...) _x
#define __1(_1, _x, ...) _x
#define __2(_1, _2, _x, ...) _x

#define __expand(...) __VA_ARGS__

// BOOST_PP names are too long

#define pp_seq_elem				BOOST_PP_SEQ_ELEM
#define pp_seq_enum				BOOST_PP_SEQ_ENUM
#define pp_seq_fold(op, seq)	BOOST_PP_SEQ_FOLD_LEFT(op, , seq)
#define pp_seq_transform		BOOST_PP_SEQ_TRANSFORM
#define pp_seq_foreach			BOOST_PP_SEQ_FOR_EACH

#define pp_tuple_size			BOOST_PP_TUPLE_SIZE
#define pp_tuple_elem			BOOST_PP_TUPLE_ELEM
#define pp_tuple_enum			BOOST_PP_TUPLE_ENUM
#define pp_tuple_remove			BOOST_PP_TUPLE_REMOVE

#define pp_tuple_extract(index, tuple)	pp_tuple_elem(pp_tuple_size(tuple), index, tuple)
#define pp_tuple_head(tuple)	pp_tuple_extract(0, tuple)
#define pp_tuple_tail(tuple)	pp_tuple_remove(tuple, 0)

#define pp_cat					BOOST_PP_CAT
#define pp_string				BOOST_PP_STRINGIZE
#define pp_enum					BOOST_PP_ENUM

#define pp_enum_prepend(z, n, text) text n

#define pp_seq_expand(op, seq)	pp_seq_enum(pp_seq_transform(op, ~, seq))

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

#define tuples_sequence_0(...) ((__VA_ARGS__)) tuples_sequence_1
#define tuples_sequence_1(...) ((__VA_ARGS__)) tuples_sequence_0
#define tuples_sequence_0_end
#define tuples_sequence_1_end

#define tuples_sequence(tuples) pp_cat(tuples_sequence_0 tuples, _end)

#define template_class_tuple(tuple) pp_tuple_head(tuple)<pp_tuple_enum(pp_tuple_tail(tuple))>

//---------------------------------------------------------------------------
#endif
