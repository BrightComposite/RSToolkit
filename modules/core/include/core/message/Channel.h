//---------------------------------------------------------------------------

#ifndef CHANNEL_H
#define CHANNEL_H

//---------------------------------------------------------------------------

#include <core/container/TypedSet.h>
#include <core/container/Array.h>
#include "Message.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class Dst, class Msg>
	struct is_dest;

	template<class Dst, typename Msg>
	struct has_reader;

//---------------------------------------------------------------------------

	struct MessageReceiver : public Shared {};

	struct BasicReceivers : Shared {};

	typebase_api(core, BasicReceivers);

	template<class Dst, class Msg>
	using ReceiversList = array_list<Receiver<Dst, Msg>>;

	using ReceiversSet = TypedSet<BasicReceivers>;

	member_checker(has_receivers, receiversMap);

	template<class Dst, class Msg>
	struct Receivers : BasicReceivers
	{
		ReceiversList<Dst, Msg> contents;

		static const bool exist = !is_same<Dst, Empty>::value && has_receivers<Dst>::value;

		static void addTo(Dst & dest, const Receiver<Dst, Msg> & rcvr)
		{
			dest.receiversMap.template require<Receivers>()->contents.push_back(rcvr);
		}

		static void addTo(Dst & dest, Receiver<Dst, Msg> && rcvr)
		{
			dest.receiversMap.template require<Receivers>()->contents.push_back(forward<Receiver<Dst, Msg>>(rcvr));
		}

		static ReceiversList<Dst, Msg> & requireFrom(Dst & dest)
		{
			return dest.receiversMap.template require<Receivers>()->contents;
		}

		static ReceiversList<Dst, Msg> * seek(Dst & dest)
		{
			auto r = dest.receiversMap.template seek<Receivers>();

			if(r == nullptr)
				return nullptr;

			return &r->contents;
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

		size_t operator += (const Receiver<Dst, Msg> & receiver)
		{
			receivers.push_back(receiver);
			return receiver.id;
		}

		size_t operator += (Receiver<Dst, Msg> && receiver)
		{
			receivers.push_back(forward<Receiver<Dst, Msg>>(receiver));
			return receiver.id;
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, false>
	{
		using Rcvr = Receiver<Dst, Msg>;

		static size_t connect(const Rcvr & receiver)
		{
			globalReceivers().push_back(receiver);
			return receiver.id;
		}

		static size_t connect(Rcvr && receiver)
		{
			globalReceivers().push_back(forward<Rcvr>(receiver));
			return receiver.id;
		}

		static void disconnect(const Rcvr & receiver)
		{
			auto & r = globalReceivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(*i == receiver)
				{
					r.erase(i);
					break;
				}
			}
		}

		static void disconnect(size_t id, size_t additional)
		{
			auto & r = globalReceivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(i->id == id && i->additional == additional)
				{
					r.erase(i);
					break;
				}
			}
		}

		static void disconnect(size_t id)
		{
			auto & r = globalReceivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(i->id == id)
					i = r.erase(i);
			}
		}

		template<class R>
		static void disconnect(const R * rcvr)
		{
			auto & r = globalReceivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(rcvr)))
					i = r.erase(i);
			}
		}

		static ReceiversWrapper<Dst, Msg> receivers()
		{
			return {globalReceivers()};
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

		static size_t connect(Dst & dest, const Rcvr & receiver)
		{
			Rcvrs::addTo(dest, receiver);
			return receiver.id;
		}

		static size_t connect(Dst & dest, Rcvr && receiver)
		{
			Rcvrs::addTo(dest, forward<Rcvr>(receiver));
			return receiver.id;
		}

		static ReceiversWrapper<Dst, Msg> receivers(Dst & dest)
		{
			return {Rcvrs::requireFrom(dest)};
		}

		static void disconnect(Dst & dest, const Rcvr & receiver)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
					if(*i == receiver)
					{
						local->erase(i);
						break;
					}
		}

		static void disconnect(Dst & dest, size_t id, size_t additional)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
					if(i->id == id && i->additional == additional)
					{
						local->erase(i);
						break;
					}
		}

		static void disconnect(Dst & dest, size_t id)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
					if(i->id == id)
						i = local->erase(i);
		}

		template<class R>
		static void disconnect(Dst & dest, const R * rcvr)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
					if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(rcvr)))
						i = local->erase(i);
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

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	size_t connect(const Receiver<Dst, Msg> & receiver)
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg>
	size_t connect(void receiver(Handle<Msg> &, Dst &))
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg>
	size_t connect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg, class Functor, useif <
		can_receive<Functor, Dst, Msg>::value
		> endif
	>
	size_t connect(const Functor & receiver)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class Functor, useif <
		can_receive<Functor, Dst, Msg>::value
		> endif
	>
	size_t connect(Functor && receiver)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(forward<Functor>(receiver), std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner, useif <!is_const<Rcvr>::value> endif>
	size_t connect(Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner>
	size_t connect(const Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner, useif <!is_const<Rcvr>::value> endif>
	size_t connect(const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner>
	size_t connect(const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

//---------------------------------------------------------------------------

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, const Receiver<Dst, Msg> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, void receiver(Handle<Msg> &, Dst &))
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class Functor, class RealDst, useif <
		is_dest<RealDst, Msg>::value,
		based_on<RealDst, Dst>::value,
		can_receive<Functor, Dst, Msg>::value
		> endif
	>
	size_t connect(RealDst & dest, const Functor & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class Functor, class RealDst, useif <
		is_dest<RealDst, Msg>::value,
		based_on<RealDst, Dst>::value,
		can_receive<Functor, Dst, Msg>::value
		> endif
	>
	size_t connect(RealDst & dest, Functor && receiver)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(forward<Functor>(receiver), std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value, !is_const<Rcvr>::value
		> endif
	>
	size_t connect(RealDst & dest, Rcvr * receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, const Rcvr * receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, class ... RcvrOwner, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value, !is_const<Rcvr>::value
		> endif
	>
	size_t connect(RealDst & dest, const Handle<Rcvr, RcvrOwner...> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, class ... RcvrOwner, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, const Handle<Rcvr, RcvrOwner...> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	void disconnect(const Receiver<Dst, Msg> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg>
	void disconnect(size_t id)
	{
		Channel<Dst, Msg>::disconnect(id);
	}

	template<class Dst, typename Msg>
	void disconnect(void receiver(Handle<Msg> &, Dst &))
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg>
	void disconnect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg, class Functor, useif<
		std::is_convertible<Functor, msg_callback<Dst, Msg>>::value &&
		!is_handle<Functor>::value
		> endif
	>
	void disconnect(Functor receiver)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class Rcvr>
	void disconnect(Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr>
	void disconnect(const Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr>
	void disconnect(const Rcvr * receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner>
	void disconnect(const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner>
	void disconnect(const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class ... RcvrOwner>
	void disconnect(const Handle<Rcvr, RcvrOwner...> & receiver)
	{
		Channel<Dst, Msg>::disconnect(static_cast<const Rcvr *>(receiver));
	}

	//---------------------------------------------------------------------------

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Receiver<Dst, Msg> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, size_t id)
	{
		Channel<Dst, Msg>::disconnect(dest, id);
	}

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, void receiver(Handle<Msg> &, Dst &))
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class Functor, class RealDst, useif<
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value &&
		std::is_convertible<Functor, msg_callback<Dst, Msg>>::value &&
		!is_handle<Functor>::value
		> endif
	>
	void disconnect(RealDst & dest, Functor receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
	> endif
	>
	void disconnect(RealDst & dest, Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
	> endif
	>
	void disconnect(RealDst & dest, const Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Rcvr * receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, class ... RcvrOwner, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, class ... RcvrOwner, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
	> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr, RcvrOwner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr, class ... RcvrOwner, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr, RcvrOwner...> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, static_cast<const Rcvr *>(receiver));
	}

//---------------------------------------------------------------------------

#define subscribe_on(Dst, Msg, ... /*Real dst*/) Channel<Dst, Msg>::receivers(__VA_ARGS__) += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)

//---------------------------------------------------------------------------

#define declare_receivers() 					\
	template<class, typename>					\
	friend struct Rapture::Receivers;			\
	Rapture::ReceiversSet receiversMap;			\

#define set_message_dest(Dst, Msg) using pp_cat(DestOf, Msg) = Dst;

#define bind_message(Dst, Msg)					\
	template<class, typename>					\
	friend struct Rapture::Receivers;			\
	template<class, typename>					\
	friend struct Rapture::DestGetter;			\
												\
	set_message_dest(Dst, Msg)					\

#define op_bind_message(s, Dst, Msg) set_message_dest(Dst, Msg)

#define bind_messages(Dst, ... /* Messages */)	\
	template<class, typename>					\
	friend struct Rapture::Receivers;			\
	template<class, typename>					\
	friend struct Rapture::DestGetter;			\
												\
	pp_seq_foreach(op_bind_message, Dst,		\
		pp_tuple_to_seq((__VA_ARGS__))			\
		)										\

#define op_channel_api(s, data, Msg)			\
	template struct pp_tuple_extract(0, data) MessageConnector<pp_tuple_extract(1, data), Msg>; \
	template struct pp_tuple_extract(0, data) TypeId<Receivers<pp_tuple_extract(1, data), Msg>, BasicReceivers>;

#define channels_api(module, Dst, ... /* Messages */)	\
	pp_seq_foreach(op_channel_api, (api(module), Dst),	\
		pp_tuple_to_seq((__VA_ARGS__))					\
		)												\

}

//---------------------------------------------------------------------------
#endif
