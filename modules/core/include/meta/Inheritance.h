//---------------------------------------------------------------------------

#ifndef META_INHERITANCE_H
#define META_INHERITANCE_H

//---------------------------------------------------------------------------

#include "Types.h"
#include "Preprocessor.h"

//---------------------------------------------------------------------------

namespace Rapture
{
#define inherit_operator(op, descendant, ... /*ancestor*/)	\
	template<class ... A, typename = decltype(				\
		declval<__VA_ARGS__>().operator op(declval<A>()...)	\
		)>													\
	descendant & operator op (A &&... args)					\
	{														\
		__VA_ARGS__::operator op(forward<A>(args)...);		\
		return *this;										\
	}														\

#define op_inherit(r, data, elem) inherit_operator(elem, pp_tuple_elem(2, 0, data), pp_tuple_elem(2, 1, data))

#define inherit_ops(Descendant, Ancestor) pp_seq_foreach(op_inherit, (Descendant, Ancestor), (=)(+=)(-=)(*=)(/=)(|=)(^=)(&=)(%=))

#define subclass(Name, Ancestor)							\
	class Name : public	Ancestor							\
	{														\
		typedef Ancestor Base;								\
															\
	public:													\
		template<class ... A,								\
			useif <can_construct<Base, A...>::value> endif	\
		>													\
		Name(A &&... args) : Base(forward<A>(args)...) {}	\
															\
		inherit_ops(Name, Base)								\
	};														\

#define template_subclass(Name, Ancestor, .../*args*/)		\
	class Name : public	Ancestor<__VA_ARGS__>				\
	{														\
		typedef Ancestor<__VA_ARGS__> Base;					\
															\
	public:													\
		template<class ... A,								\
			useif <can_construct<Base, A...>::value> endif	\
		>													\
		Name(A &&... args) : Base(forward<A>(args)...) {}	\
															\
		inherit_ops(Name, Base)								\
	};														\

}

//---------------------------------------------------------------------------
#endif
