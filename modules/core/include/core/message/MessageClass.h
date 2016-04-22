//---------------------------------------------------------------------------

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

//---------------------------------------------------------------------------

#include <core/meta/Types.h>

//---------------------------------------------------------------------------

#define pp_msg_ctor_param_op(z, data, ...) const pp_tuple_elem_t(__VA_ARGS__) & pp_tuple_elem_n(__VA_ARGS__)
#define pp_msg_move_param_op(z, data, ...) pp_tuple_elem_t(__VA_ARGS__) && pp_tuple_elem_n(__VA_ARGS__)
#define pp_msg_cp_init_param_op(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(pp_tuple_elem_n(__VA_ARGS__))
#define pp_msg_mv_init_param_op(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(forward<pp_tuple_elem_t(__VA_ARGS__)>(pp_tuple_elem_n(__VA_ARGS__)))

#define pp_msg_ctor_params(...) pp_seq_expand(pp_msg_ctor_param_op, __VA_ARGS__)
#define pp_msg_move_params(...) pp_seq_expand(pp_msg_move_param_op, __VA_ARGS__)
#define pp_msg_cp_init_params(...) pp_seq_expand(pp_msg_cp_init_param_op, __VA_ARGS__)
#define pp_msg_mv_init_params(...) pp_seq_expand(pp_msg_mv_init_param_op, __VA_ARGS__)

#define op_msg_declare(s, state, ...) state pp_tuple_elem_d(__VA_ARGS__);

#define message_nonempty0(Class, fields)								\
	pp_seq_fold(op_msg_declare, fields)									\
																		\
	Class(const Subject * source, pp_msg_ctor_params(fields)) :			\
		Message(source), pp_msg_cp_init_params(fields) {}				\
	Class(const Subject * source, pp_msg_move_params(fields)) :			\
		Message(source), pp_msg_mv_init_params(fields) {}				\

#define message_nonempty(Class, ...)									\
	message_nonempty0(Class, tuples_sequence(__VA_ARGS__))				\

#define message_empty(Class, ...)										\
	Class(const Subject * source) : Message(source) {}					\

#define message_contents(Class, ...)									\
	pp_if(pp_is_empty(__VA_ARGS__), message_empty, message_nonempty)	\
		(Class, __VA_ARGS__)											\

#define message_class0(Class, ...)										\
	class Class : public Message										\
	{																	\
	public:																\
		message_contents(Class, __VA_ARGS__)							\
		Class(const Class &) = delete;									\
	};																	\

#define dest_getter(Msg)												\
	template<class _Ty>													\
	struct DestGetter<_Ty, Msg>											\
		_GET_TYPE_OR_DEFAULT(DestOf##Msg, _Ty);							\

/**
 *	@brief
 *  The standart message-class creator. Accepts type of a new class and a
 *  sequence of parameters which are represented by pairs (type)(name).
 *  Example:
 *	message_class(NewMessage,
 *		(int,	 id)
 *		(String, type)
 *		);
 */
#define message_class(Class, .../*fields*/) message_class0(Class, __VA_ARGS__) dest_getter(Class)

//---------------------------------------------------------------------------
#endif
