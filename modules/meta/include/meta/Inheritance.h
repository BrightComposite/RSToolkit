//---------------------------------------------------------------------------

#pragma once

#ifndef META_INHERITANCE_H
#define META_INHERITANCE_H

//---------------------------------------------------------------------------

#include "Meta.h"

//---------------------------------------------------------------------------

namespace Rapture
{

#define OPS_ASSIGN(getter)     getter(assign,      =)
#define OPS_ADD_ASSIGN(getter) getter(add_assign, +=)
#define OPS_SUB_ASSIGN(getter) getter(sub_assign, -=)
#define OPS_MUL_ASSIGN(getter) getter(mul_assign, *=)
#define OPS_DIV_ASSIGN(getter) getter(div_assign, /=)
#define OPS_OR_ASSIGN(getter)  getter(or_assign,  |=)
#define OPS_XOR_ASSIGN(getter) getter(xor_assign, ^=)
#define OPS_AND_ASSIGN(getter) getter(and_assign, &=)
#define OPS_MOD_ASSIGN(getter) getter(mod_assign, %=)

#define OPS_ALL \
	(OPS_ASSIGN)(OPS_ADD_ASSIGN)(OPS_SUB_ASSIGN)(OPS_MUL_ASSIGN)(OPS_DIV_ASSIGN) \
	(OPS_OR_ASSIGN)(OPS_XOR_ASSIGN)(OPS_AND_ASSIGN)(OPS_MOD_ASSIGN)

#define pp_op_getter(name, _) has_##name##_op
#define pp_op_sign(_, sign) sign

#define operator_getter(op)	method_checker(op(pp_op_getter), operator op(pp_op_sign));
#define op_operator_getter(r, data, elem) operator_getter(elem)

	pp_seq_foreach(op_operator_getter, ~, OPS_ALL)

#define inherit_operator(op, types)										\
	template<class ... A, class B = pp_tuple_elem(1, types), useif<	\
		op(pp_op_getter)<B>::value										\
		>															\
	>																	\
	pp_tuple_elem(0, types) & operator op(pp_op_sign) (A &&... args)	\
	{																	\
		B::operator op(pp_op_sign)(forward<A>(args)...);				\
		return *this;													\
	}																	\

#define op_inherit_operator(r, types, op) inherit_operator(op, types)

#define inherit_ops(descendant_and_ancestor, ops) pp_seq_foreach(op_inherit_operator, descendant_and_ancestor, ops)

#define subclass(Name, ... /*Ancestor*/)					\
	class Name : public	__VA_ARGS__							\
	{														\
		typedef __VA_ARGS__ Base;							\
															\
	public:													\
		template<class ... A,								\
			useif<can_construct<Base, A...>::value>	\
		>													\
		Name(A &&... a) : Base(std::forward<A>(a)...) {}	\
															\
		inherit_ops((Name, Base), OPS_ALL)					\
	}
}

//---------------------------------------------------------------------------
#endif
