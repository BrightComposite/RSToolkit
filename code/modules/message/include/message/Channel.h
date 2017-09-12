//---------------------------------------------------------------------------

#pragma once

#ifndef CHANNEL_H
#define CHANNEL_H

//---------------------------------------------------------------------------

#include <morph/morph.h>

#include "MessageConnector.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class Dst, class Msg>
	struct is_dest;

	template<class Dst, typename Msg>
	struct has_reader;

//---------------------------------------------------------------------------

	struct MessageReceiver : public shareable {};

	struct BasicReceivers : shareable
	{
		morph_origin(BasicReceivers);
	};

	create_morph_pool(message, BasicReceivers);

	using ReceiversSet = morph<BasicReceivers>;

	member_checker(has_receivers, _rcvrs);

	template<class Dst, class Msg>
	struct Receivers : BasicReceivers
	{
		ReceiversList<Dst, Msg> contents;

		static const bool exist = !is_same<Dst, empty>::value && has_receivers<Dst>::value;

		static ReceiversList<Dst, Msg> & requireFrom(Dst & dest) {
			return dest._rcvrs.template require<Receivers>()->contents;
		}

		static ReceiversList<Dst, Msg> * seek(Dst & dest) {
			auto r = dest._rcvrs.template seek<Receivers>();
			return r ? &r->contents : nullptr;
		}

		static size_t count(Dst & dest) {
			auto r = dest._rcvrs.template seek<Receivers>();
			return (r ? r->contents.size() : 0) + Channel<Dst, Msg>::count();
		}

		static size_t addTo(Dst & dest, Receiver<Dst, Msg> && rcvr) {
			size_t id = rcvr.id;
			requireFrom(dest).push_back(forward<Receiver<Dst, Msg>>(rcvr));
			return id;
		}
	};

//---------------------------------------------------------------------------

	template<class MsgDst, typename Msg>
	struct MessageBroadcaster;

	template<class Rdr, typename Msg>
	struct MessageTransmitter;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct Channel : MessageConnector<message_dst_t<Dst, Msg>, Msg>
	{
		using MsgDst = message_dst_t<Dst, Msg>;
		using RealDst = conditional_t<is_same<MsgDst, empty>::value, Dst, MsgDst>;
		using ReaderDst = conditional_t<has_reader<Dst, Msg>::value, Dst, empty>;

		using Connector = MessageConnector<RealDst, Msg>;
		using Broadcaster = MessageBroadcaster<MsgDst, Msg>;
		using Transmitter = MessageTransmitter<ReaderDst, Msg>;

		static forceinline handle<Msg> & transmit(handle<Msg> & message, Dst & dest) {
			Broadcaster::broadcast(message, dest);
			
			auto & r = Connector::globalReceivers();

			for(auto & receiver : r) {
				receiver.callback(message, dest);
			}
			
			Transmitter::transmit(message, dest);

			return message;
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageBroadcaster
	{
		static forceinline void broadcast(handle<Msg> & message, Dst & dest) {
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr) {
				for(auto & receiver : *local) {
					receiver.callback(message, dest);
				}
			}
		}
	};

	template<typename Msg>
	struct MessageBroadcaster<empty, Msg>
	{
		template<class Dst>
		static void broadcast(handle<Msg> &, Dst &) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageTransmitter
	{
		static forceinline void transmit(handle<Msg> & message, Dst & dest) {
			dest.read(message);
		}
	};

	template<typename Msg>
	struct MessageTransmitter<empty, Msg>
	{
		template<class Dst>
		static void transmit(handle<Msg> &, Dst &) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	struct is_dest
	{
		static const bool value = Receivers<message_dst_t<Dst, Msg>, Msg>::exist;
	};

	method_checker(has_read_method, read);

	template<class Dst, typename Msg>
	struct has_reader : has_read_method<Dst, handle<Msg> &> {};
}

//---------------------------------------------------------------------------
#endif
