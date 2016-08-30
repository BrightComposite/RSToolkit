//---------------------------------------------------------------------------

#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H

//---------------------------------------------------------------------------

#include <core/Morpher.h>
#include <container/ArrayList.h>
#include "Message.h"

//---------------------------------------------------------------------------

namespace Rapture
{


#define declare_receivers() 					                                                                            \
	template<class, typename>					                                                                            \
	friend struct Rapture::Receivers;			                                                                            \
												                                                                            \
	Rapture::ReceiversSet _rcvrs;				                                                                            \
//----- declare_receivers

#define set_message_dest(Dst, Msg)				                                                                            \
	using pp_cat(DestOf, Msg) = Dst;
//----- set_message_dest

#define bind_message(Dst, Msg)					                                                                            \
	template<class, typename>					                                                                            \
	friend struct Rapture::Receivers;			                                                                            \
	template<class, typename>					                                                                            \
	friend struct Rapture::DestGetter;			                                                                            \
												                                                                            \
	set_message_dest(Dst, Msg)					                                                                            \
//----- bind_message

#define op_bind_message(s, Dst, Msg) set_message_dest(Dst, Msg)

#define bind_messages(Dst, ... /* Messages */)	                                                                            \
	template<class, typename>					                                                                            \
	friend struct Rapture::Receivers;			                                                                            \
	template<class, typename>					                                                                            \
	friend struct Rapture::DestGetter;			                                                                            \
												                                                                            \
	pp_seq_foreach(op_bind_message, Dst, pp_tuple_to_seq((__VA_ARGS__)))										            \
//----- bind_messages

#define op_channel_api(s, data, Msg)			                                                                            \
	apistruct(pp_tuple_extract(0, data)) MessageConnector<pp_tuple_extract(1, data), Msg>;                                  \
	create_morph_type(pp_tuple_extract(0, data), macrowrap(Receivers<pp_tuple_extract(1, data), Msg>));						\
//----- op_channel_api

#define channels_api(module, Dst, ... /* Messages */)	                                                                    \
	pp_seq_foreach(op_channel_api, (module, Dst), pp_tuple_to_seq((__VA_ARGS__)))											\
//----- channels_api

#define subscribe_on(Dst, Msg, ... /*Real dst*/)																			\
	Channel<Dst, Msg>::receivers(__VA_ARGS__) += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)                  \
//----- subscribe_on

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	struct is_dest;

	template<class Dst, typename Msg>
	struct has_reader;

//---------------------------------------------------------------------------

	struct MessageReceiver : public Shared {};

	struct BasicReceivers : Shared
	{
		morph_base(BasicReceivers);
	};

	create_morph_pool(message, BasicReceivers);

	template<class Dst, class Msg>
	using ReceiversList = array_list<Receiver<Dst, Msg>>;

	using ReceiversSet = Morpher<BasicReceivers>;

	member_checker(has_receivers, _rcvrs);

	template<class Dst, class Msg>
	struct Receivers : BasicReceivers
	{
		ReceiversList<Dst, Msg> contents;

		static const bool exist = !is_same<Dst, Empty>::value && has_receivers<Dst>::value;

		static ReceiversList<Dst, Msg> & requireFrom(Dst & dest)
		{
			return dest._rcvrs.template require<Receivers>()->contents;
		}

		static ReceiversList<Dst, Msg> * seek(Dst & dest)
		{
			auto r = dest._rcvrs.template seek<Receivers>();
			return r != nullptr ? &r->contents : nullptr;
		}

		static size_t count(Dst & dest)
		{
			return acquire(dest._rcvrs.template seek<Receivers>(), [](Receivers * r) { return r->contents.size(); }) + Channel<Dst, Msg>::count();
		}

		static size_t addTo(Dst & dest, Receiver<Dst, Msg> && rcvr)
		{
			size_t id = rcvr.id;
			requireFrom(dest).push_back(forward<Receiver<Dst, Msg>>(rcvr));
			return id;
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct Channel;

	template<class MsgDst, typename Msg, bool = is_dest<MsgDst, Msg>::value>
	struct MessageConnector {};

	template<class MsgDst, typename Msg>
	struct MessageBroadcaster;

	template<class Rdr, typename Msg>
	struct MessageTransmitter;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct Channel : MessageConnector<message_dst_t<Dst, Msg>, Msg>
	{
		using MsgDst    = message_dst_t<Dst, Msg>;
		using RealDst   = conditional_t<is_same<MsgDst, Empty>::value, Dst, MsgDst>;
		using ReaderDst = conditional_t<has_reader<Dst, Msg>::value, Dst, Empty>;

		using Connector   = MessageConnector<RealDst, Msg>;
		using Broadcaster = MessageBroadcaster<MsgDst, Msg>;
		using Transmitter = MessageTransmitter<ReaderDst, Msg>;

		static Handle<Msg> & transmit(Handle<Msg> & message, Dst & dest)
		{
			Broadcaster::broadcast(message, dest);

			auto & r = Connector::globalReceivers();

			if(r.size() > 0)
			{
				for(auto & receiver : r)
					receiver.callback(message, dest);
			}

			Transmitter::transmit(message, dest);

			return message;
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct ReceiversWrapper
	{
		ReceiversList<Dst, Msg> & receivers;

		void operator += (msg_callback<Dst, Msg> && receiver)
		{
			receivers.push_back(Receiver<Dst, Msg>(move(receiver)));
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, false>
	{
		using Rcvr = Receiver<Dst, Msg>;

		static size_t connect(Rcvr && receiver)
		{
			size_t id = receiver.id;
			globalReceivers().push_back(forward<Rcvr>(receiver));
			return id;
		}

		static void disconnect(size_t id)
		{
			auto & r = globalReceivers();

			for(auto i = r.begin(); i != r.end(); ++i)
				if(i->id == id)
				{
					r.erase(i);
					break;
				}
		}

		static ReceiversWrapper<Dst, Msg> receivers()
		{
			return {globalReceivers()};
		}

		static size_t count()
		{
			return globalReceivers().size();
		}

		static ReceiversList<Dst, Msg> & globalReceivers()
		{
			static ReceiversList<Dst, Msg> r;
			return r;
		}
	};

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, true> : MessageConnector<Dst, Msg, false>
	{
		using Base = MessageConnector<Dst, Msg, false>;
		using Rcvr = Receiver<Dst, Msg>;
		using Rcvrs = Receivers<Dst, Msg>;

		using Base::connect;
		using Base::disconnect;
		using Base::receivers;

		static size_t connect(Dst & dest, Rcvr && receiver)
		{
			return Rcvrs::addTo(dest, forward<Rcvr>(receiver));
		}

		static ReceiversWrapper<Dst, Msg> receivers(Dst & dest)
		{
			return {Rcvrs::requireFrom(dest)};
		}

		static void disconnect(Dst & dest, size_t id)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
					if(i->id == id)
					{
						local->erase(i);
						break;
					}
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageBroadcaster
	{
		static void broadcast(Handle<Msg> & message, Dst & dest)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto & receiver : *local)
					receiver.callback(message, dest);
		}
	};

	template<typename Msg>
	struct MessageBroadcaster<Empty, Msg>
	{
		template<class Dst>
		static void broadcast(Handle<Msg> &, Dst &) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageTransmitter
	{
		static void transmit(Handle<Msg> & message, Dst & dest)
		{
			dest.read(message);
		}
	};

	template<typename Msg>
	struct MessageTransmitter<Empty, Msg>
	{
		template<class Dst>
		static void transmit(Handle<Msg> &, Dst &) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	struct is_dest
	{
		static const bool value = Receivers<message_dst_t<Dst, Msg>, Msg>::exist;
	};

	method_checker(has_read_method, read);

	template<class Dst, typename Msg>
	struct has_reader : has_read_method<Dst, Handle<Msg> &> {};
}

//---------------------------------------------------------------------------
#endif
