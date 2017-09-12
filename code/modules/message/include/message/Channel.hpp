//---------------------------------------------------------------------------

#pragma once

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

//---------------------------------------------------------------------------

#include "message.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class, typename>
	struct Receivers;

	template<class, typename>
	struct Channel;

//---------------------------------------------------------------------------

#define declare_receivers() 					                                                                            \
	template<class, typename>					                                                                            \
	friend struct asd::Receivers;			                                                                                \
												                                                                            \
	asd::ReceiversSet _rcvrs;				                                                                                \
//----- declare_receivers

#define set_message_dest(Dst, Msg)				                                                                            \
	using pp_cat(DestOf, Msg) = Dst;
	//----- set_message_dest

#define bind_message(Dst, Msg)					                                                                            \
	template<class, typename>					                                                                            \
	friend struct asd::Receivers;			                                                                                \
	template<class, typename>					                                                                            \
	friend struct asd::DestGetter;			                                                                                \
												                                                                            \
	set_message_dest(Dst, Msg)					                                                                            \
//----- bind_message

#define op_bind_message(s, Dst, Msg) set_message_dest(Dst, Msg)

#define bind_messages(Dst, ... /* Messages */)	                                                                            \
	template<class, typename>					                                                                            \
	friend struct asd::Receivers;			                                                                                \
	template<class, typename>					                                                                            \
	friend struct asd::DestGetter;			                                                                                \
												                                                                            \
	pp_seq_foreach(op_bind_message, Dst, pp_tuple_to_seq((__VA_ARGS__)))										            \
//----- bind_messages

#define op_channel_api(s, data, Msg)			                                                                            \
	apistruct(pp_tuple_extract(0, data)) MessageConnector<pp_tuple_extract(1, data), Msg>;                                  \
	create_morph_type(pp_tuple_extract(0, data), macrowrap(asd::Receivers<pp_tuple_extract(1, data), Msg>));				\
//----- op_channel_api

#define channels_api(module, Dst, ... /* Messages */)	                                                                    \
	pp_seq_foreach(op_channel_api, (module, Dst), pp_tuple_to_seq((__VA_ARGS__)))											\
//----- channels_api

#define subscription(dst)																									\
	int pp_cat(__I, __LINE__) = 0; for(auto & __DST__ = dst; pp_cat(__I, __LINE__) < 1; ++pp_cat(__I, __LINE__))
//----- subscription

#define onmessage(Msg)																										\
	using pp_cat(__TMP_TYPE_, __LINE__) = decay_t<decltype(__DST__)>;                                                       \
	asd::Channel<pp_cat(__TMP_TYPE_, __LINE__), Msg>::receivers(__DST__) +=                                                 \
		[](asd::handle<Msg> & msg, pp_cat(__TMP_TYPE_, __LINE__) & dest)
//----- onmessage
}
//---------------------------------------------------------------------------
#endif
