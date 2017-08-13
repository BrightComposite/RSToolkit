//---------------------------------------------------------------------------

#pragma once

#ifndef MESSAGE_CLASS_H
#define MESSAGE_CLASS_H

//---------------------------------------------------------------------------

#include <core/addition/contents.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class Dst, typename Msg>
	struct DestGetter
	{
		typedef Dst type;
	};

	/**
	 *	@brief
	 *  The standart message-class creator. Accepts type of a new class and a
	 *  sequence of parameters which are represented by pairs (type)(name).
	 *  Example:
	 *	message_class
	 *	(
	 *		NewMessage,
	 *		(int,		id)
	 *		(String,	type)
	 *	);
	 */
#define message_class(Msg, ... /*elements*/)											\
	struct Msg;																			\
																						\
	declare_contents(Msg, __VA_ARGS__);													\
																						\
	struct Msg : Internals::Message<Msg>												\
	{																					\
		using Internals::Message<Msg>::Message;											\
	};																					\
																						\
	type_getter(macrowrap(DestGetter<T, Msg>), T, pp_cat(DestOf, Msg), empty);

	template<class Dst, typename Msg>
	using message_dst_t = typename DestGetter<Dst, Msg>::type;
}

//---------------------------------------------------------------------------
#endif