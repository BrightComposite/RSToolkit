//---------------------------------------------------------------------------

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

//---------------------------------------------------------------------------

#include <meta/Types.h>

//---------------------------------------------------------------------------

namespace Rapture
{

#define pp_msg_ctor_param_op(z, data, ...) const pp_tuple_elem_t(__VA_ARGS__) & pp_tuple_elem_n(__VA_ARGS__)
#define pp_msg_move_param_op(z, data, ...) pp_tuple_elem_t(__VA_ARGS__) && pp_tuple_elem_n(__VA_ARGS__)
#define pp_msg_cp_init_param_op(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(pp_tuple_elem_n(__VA_ARGS__))
#define pp_msg_mv_init_param_op(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(forward<pp_tuple_elem_t(__VA_ARGS__)>(pp_tuple_elem_n(__VA_ARGS__)))

#define pp_msg_ctor_params(...) pp_seq_expand(pp_msg_ctor_param_op, __VA_ARGS__)
#define pp_msg_move_params(...) pp_seq_expand(pp_msg_move_param_op, __VA_ARGS__)
#define pp_msg_cp_init_params(...) pp_seq_expand(pp_msg_cp_init_param_op, __VA_ARGS__)
#define pp_msg_mv_init_params(...) pp_seq_expand(pp_msg_mv_init_param_op, __VA_ARGS__)

#define op_msg_declare(s, state, ...) state pp_tuple_elem_d(__VA_ARGS__);

#define message_nonempty0(Msg, fields)								\
	pp_seq_fold(op_msg_declare, fields)								\
																	\
	Msg(const Subject * source, pp_msg_ctor_params(fields)) :		\
		Message(source), pp_msg_cp_init_params(fields) {}			\
	Msg(const Subject * source, pp_msg_move_params(fields)) :		\
		Message(source), pp_msg_mv_init_params(fields) {}			\

#define message_nonempty(Msg, ...)									\
	message_nonempty0(Msg, tuples_sequence(__VA_ARGS__))			\

#define message_empty(Msg, ...)										\
	Msg(const Subject * source) : Message(source) {}				\

#define message_contents(Msg, ...)									\
	pp_if(pp_is_empty(__VA_ARGS__), message_empty, message_nonempty)\
		(Msg, __VA_ARGS__)											\

#define set_message_dest(Dst, Msg) using pp_cat(DestOf, Msg) = Dst;

	template<class Dst, typename Msg>
	struct DestGetter
	{
		typedef Dst type;
	};

	template<class Dst, typename Msg>
	using message_dst_t = typename DestGetter<Dst, Msg>::type;

#define make_dest_getter(Msg)											\
	type_getter(T, macrowrap(DestGetter<T, Msg>), pp_cat(DestOf, Msg), T)

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
#define message_class(Msg, .../* fields */)	\
	class Msg : public Message				\
	{										\
	public:									\
		message_contents(Msg, __VA_ARGS__)	\
	};										\
											\
	make_dest_getter(Msg)
}

//---------------------------------------------------------------------------
#endif