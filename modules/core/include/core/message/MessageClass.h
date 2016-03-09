//---------------------------------------------------------------------------

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

//---------------------------------------------------------------------------

#include <core/meta/Types.h>

//---------------------------------------------------------------------------

#define pp_msg_elem_n(...) pp_cat(n, pp_tuple_elem_n(__VA_ARGS__))

#define pp_msg_copy(z, data, ...) const pp_tuple_elem_t(__VA_ARGS__) & pp_msg_elem_n(__VA_ARGS__)
#define pp_msg_move(z, data, ...) pp_tuple_elem_t(__VA_ARGS__) && pp_msg_elem_n(__VA_ARGS__)

#define pp_msg_copy_init(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(pp_msg_elem_n(__VA_ARGS__))
#define pp_msg_move_init(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(forward<pp_tuple_elem_t(__VA_ARGS__)>(pp_msg_elem_n(__VA_ARGS__)))
#define pp_msg_copy_init_s(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(msg.pp_tuple_elem_n(__VA_ARGS__))
#define pp_msg_move_init_s(z, data, ...) pp_tuple_elem_n(__VA_ARGS__)(move(msg.pp_tuple_elem_n(__VA_ARGS__)))

#define op_msg_compare(s, state, ...) state pp_tuple_elem_n(__VA_ARGS__) == msg.pp_tuple_elem_n(__VA_ARGS__) &&
#define op_msg_assign(s, state, ...) state pp_tuple_elem_n(__VA_ARGS__) = msg.pp_tuple_elem_n(__VA_ARGS__);
#define op_msg_move(s, state, ...) state pp_tuple_elem_n(__VA_ARGS__) = move(msg.pp_tuple_elem_n(__VA_ARGS__));
#define op_msg_declare(s, state, ...) state pp_tuple_elem_d(__VA_ARGS__);

#define _message_class(Class, fields)																							\
	class Class : public Message																								\
	{																															\
	public:																														\
		Class(pp_seq_expand(pp_msg_copy, fields)) : Message(), pp_seq_expand(pp_msg_copy_init, fields) {}						\
		Class(pp_seq_expand(pp_msg_move, fields)) : Message(), pp_seq_expand(pp_msg_move_init, fields) {}						\
		Class(const Class & msg) : Message(), pp_seq_expand(pp_msg_copy_init_s, fields) {}										\
		Class(Class && msg) : Message(), pp_seq_expand(pp_msg_move_init_s, fields) {}											\
		~Class() {}																												\
																																\
		Class & operator = (const Class & msg)																					\
		{																														\
			pp_seq_fold(op_msg_assign, fields)																					\
			return *this;																										\
		}																														\
																																\
		Class & operator = (Class && msg)																						\
		{																														\
			pp_seq_fold(op_msg_move, fields)																					\
			return *this;																										\
		}																														\
																																\
		bool operator == (const Class & msg) const																				\
		{																														\
			return pp_seq_fold(op_msg_compare, fields) 1;																		\
		}																														\
																																\
		pp_seq_fold(op_msg_declare, fields)																						\
	};																															\

#define message_receivers_getter(Msg)																							\
	template<class Dst>																											\
	class MessageDispatcher<Dst, Msg>																							\
	{																															\
		template<class, typename, bool>																							\
		friend class MessageBroadcaster;																						\
																																\
		template<class, typename, bool>																							\
		friend class MessageConnector;																							\
																																\
		member_checker(has_receivers, receiversOf##Msg);																		\
																																\
		template<class RealDst,																									\
			require(																											\
				based_on(RealDst, Dst)																							\
				)>																												\
		static inline auto & receivers(RealDst & dest)																			\
		{																														\
			if(dest.receiversOf##Msg == nullptr)																				\
				dest.receiversOf##Msg.init();																					\
																																\
			return *dest.receiversOf##Msg;																						\
		}																														\
																																\
	public:																														\
		static const bool hasDestination = has_receivers<Dst>::value;															\
	};																															\
																																\

/**
 *	@brief
 *  Standart message-class creator. Accepts type of new class and
 *  sequence of parameters which are represented by pairs (type)(name).
 *  Example:
 *	message_class(
 *		NewMessage,
 *		(int,	 id)
 *		(String, type)
 *	);
 */
#define message_class(name, fields) _message_class(name, tuples_sequence(fields)) message_receivers_getter(name)
#define empty_message_class(name) class name : public Message {}; message_receivers_getter(name)

#define mkmsg(Msg, ...) Handle<Msg>(__VA_ARGS__)

//---------------------------------------------------------------------------
#endif
