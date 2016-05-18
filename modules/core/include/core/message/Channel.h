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

	template<class Dst, class Msg>
	using ReceiversList = vector<Receiver<Dst, Msg>>;

	using ReceiversSet = TypedSet<BasicReceivers>;

	member_checker(has_receivers, receiversMap);

	template<class Dst, class Msg>
	struct Receivers : BasicReceivers
	{
		ReceiversList<Dst, Msg> contents;

		static const bool exist = !is_same<Dst, Empty>::value && has_receivers<Dst>::value;

		template<useif <exist> endif>
		static void add_to(Dst & dest, const Receiver<Dst, Msg> & rcvr)
		{
			dest.receiversMap.template require<Receivers>()->contents.push_back(rcvr);
		}

		template<useif <exist> endif>
		static void add_to(Dst & dest, Receiver<Dst, Msg> && rcvr)
		{
			dest.receiversMap.template require<Receivers>()->contents.push_back(forward<Receiver<Dst, Msg>>(rcvr));
		}

		template<useif <exist> endif>
		static ReceiversList<Dst, Msg> & require_from(Dst & dest)
		{
			return dest.receiversMap.template require<Receivers>()->contents;
		}

		template<useif <exist> endif>
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
		using ReaderDst	= reader_t<Dst, Msg>;

		using Connector   = MessageConnector<RealDst, Msg>;
		using Broadcaster = MessageBroadcaster<MsgDst, Msg>;
		using Transmitter = MessageTransmitter<ReaderDst, Msg>;

		static Handle<Msg> & transmit(Handle<Msg> & message, Dst & dest)
		{
			Broadcaster::broadcast(message, dest);

			auto & r = Connector::instance().receivers();

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
	struct MessageConnector<Dst, Msg, false> : Singleton<MessageConnector<Dst, Msg, false>>
	{
		using Base = Singleton<MessageConnector<Dst, Msg, false>>;
		using Rcvr = Receiver<Dst, Msg>;

		static size_t connect(const Rcvr & receiver)
		{
			receivers().push_back(receiver);
			return receiver.id;
		}

		static size_t connect(Rcvr && receiver)
		{
			receivers().push_back(forward<Rcvr>(receiver));
			return receiver.id;
		}

		size_t operator += (const Rcvr & receiver)
		{
			_receivers.contents.push_back(receiver);
			return receiver.id;
		}

		size_t operator += (Rcvr && receiver)
		{
			_receivers.contents.push_back(forward<Rcvr>(receiver));
			return receiver.id;
		}

		static void disconnect(const Rcvr & receiver)
		{
			auto & r = receivers();

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
			auto & r = receivers();

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
			auto & r = receivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(i->id == id)
					i = r.erase(i);
			}
		}

		template<class R>
		static void disconnect(const Handle<R> & rcvr)
		{
			auto & r = receivers();

			for(auto i = r.begin(); i != r.end(); ++i)
			{
				if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(static_cast<const R *>(rcvr))))
					i = r.erase(i);
			}
		}

		static ReceiversList<Dst, Msg> & receivers()
		{
			return Base::instance()._receivers.contents;
		}

	protected:
		Receivers<Dst, Msg> _receivers;
	};

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, true> : MessageConnector<Dst, Msg, false>
	{
		using Base = MessageConnector<Dst, Msg, false>;
		using Rcvr = Receiver<Dst, Msg>;
		using Rcvrs = Receivers<Dst, Msg>;

		struct ReceiversWrapper
		{
			ReceiversList<Dst, Msg> & receivers;

			size_t operator += (const Rcvr & receiver)
			{
				receivers.push_back(receiver);
				return receiver.id;
			}

			size_t operator += (Rcvr && receiver)
			{
				receivers.push_back(forward<Rcvr>(receiver));
				return receiver.id;
			}
		};

		using Base::connect;
		using Base::disconnect;

		static size_t connect(Dst & dest, const Rcvr & receiver)
		{
			Rcvrs::add_to(dest, receiver);
			return receiver.id;
		}

		static size_t connect(Dst & dest, Rcvr && receiver)
		{
			Rcvrs::add_to(dest, forward<Rcvr>(receiver));
			return receiver.id;
		}

		static ReceiversWrapper receivers(Dst & dest)
		{
			return {Rcvrs::require_from(dest)};
		}

		static void disconnect(Dst & dest, const Rcvr & receiver)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
				{
					if(*i == receiver)
					{
						local->erase(i);
						break;
					}
				}
		}

		static void disconnect(Dst & dest, size_t id, size_t additional)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
				{
					if(i->id == id && i->additional == additional)
					{
						local->erase(i);
						break;
					}
				}
		}

		static void disconnect(Dst & dest, size_t id)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
				{
					if(i->id == id)
						i = local->erase(i);
				}
		}

		template<class R>
		static void disconnect(Dst & dest, const Handle<R> & rcvr)
		{
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local != nullptr && local->size() > 0)
				for(auto i = local->begin(); i != local->end(); ++i)
				{
					if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(static_cast<const R *>(rcvr))))
						i = local->erase(i);
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

	template<class Rcvr, class Dst, typename Msg, useif <!is_const<Rcvr>::value> endif>
	size_t connect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg>
	size_t connect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
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

	template<class Rcvr, class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value, !is_const<Rcvr>::value
		> endif
	>
	size_t connect(RealDst & dest, const Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value
		> endif
	>
	size_t connect(RealDst & dest, const Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
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

	template<class Rcvr, class Dst, typename Msg>
	void disconnect(Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg>
	void disconnect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr>
	void disconnect(const Handle<Rcvr> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
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

	template<class Rcvr, class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class RealDst, useif <
		is_dest<RealDst, Msg>::value && based_on<RealDst, Dst>::value
		> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

//---------------------------------------------------------------------------

#define dest_connect(realDst, Dst, Msg) Channel<Dst, Msg>::receivers(realDst) += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)
#define global_connect(Dst, Msg) Channel<Dst, Msg>::instance() += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)

//---------------------------------------------------------------------------

#define declare_receivers() 						\
	template<class, typename>						\
	friend struct Rapture::Receivers;				\
	Rapture::ReceiversSet receiversMap;				\

#define set_message_dest(Dst, Msg) using pp_cat(DestOf, Msg) = Dst;
#define set_message_reader(Dst, Msg) using pp_cat(ReaderOf, Msg) = Dst;

#define bind_message(Dst, Msg)						\
	template<class, typename>						\
	friend struct Rapture::Receivers;				\
	template<class, typename>						\
	friend struct Rapture::DestGetter;				\
													\
	set_message_dest(Dst, Msg)						\

#define op_bind_message(s, Dst, Msg) set_message_dest(Dst, Msg)

#define bind_messages(Dst, ... /* Messages */)		\
	template<class, typename>						\
	friend struct Rapture::Receivers;				\
	template<class, typename>						\
	friend struct Rapture::DestGetter;				\
													\
	pp_seq_foreach(op_bind_message, Dst,			\
		pp_tuple_to_seq((__VA_ARGS__))				\
		)											\

#define create_custom_reader(Dst, Msg, msg)			\
	friend_sfinae(has_read_method);					\
													\
	template<class, typename>						\
	friend struct Rapture::MessageTransmitter;		\
	template<class, typename>						\
	friend struct Rapture::ReaderGetter;			\
													\
	set_message_reader(Dst, Msg)					\
													\
	virtual void read(Rapture::Handle<Msg> & msg)	\

#define create_reader(Dst, Msg)						\
	create_custom_reader(Msg, msg)									

#define op_create_reader(s, Dst, Msg) virtual void read(Rapture::Handle<Msg> &); set_message_reader(Dst, Msg)
#define op_create_empty_reader(s, Dst, Msg) virtual void read(Rapture::Handle<Msg> &) {} set_message_reader(Dst, Msg)

#define create_readers_func(Dst, op, seq)			\
	friend_sfinae(has_read_method);					\
													\
	template<class, typename>						\
	friend struct Rapture::MessageTransmitter;		\
	template<class, typename>						\
	friend struct Rapture::ReaderGetter;			\
													\
	pp_seq_foreach(op, Dst, seq)					\

#define create_readers(Dst, ...)					\
	create_readers_func(Dst, op_create_reader,		\
		pp_tuple_to_seq((__VA_ARGS__))				\
		)											\

#define create_empty_readers(Dst, ...)				\
	create_readers_func(Dst, op_create_empty_reader,\
		pp_tuple_to_seq((__VA_ARGS__))				\
		)											\

#define override_custom_reader(Dst, Msg, msg)		\
	template<class, typename>						\
	friend struct Rapture::ReaderGetter;			\
													\
	set_message_reader(Dst, Msg)					\
													\
	virtual void read(Rapture::Handle<Msg> & msg) override					

#define override_reader(Dst, Msg)						\
	override_custom_reader(Dst, Msg, msg)									

#define op_override_reader(s, Dst, Msg) override_reader(Dst, Msg); 

#define override_readers(Dst, ...)					\
	create_readers_func(Dst, op_override_reader,	\
		pp_tuple_to_seq((__VA_ARGS__))				\
		)											\

#define implement_custom_reader(Dst, Msg, msg)	void Dst::read(Handle<Msg> & msg)
#define implement_reader(Dst, Msg)	implement_custom_reader(Dst, Msg, msg)
}

//---------------------------------------------------------------------------
#endif
