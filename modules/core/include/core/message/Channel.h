//---------------------------------------------------------------------------

#ifndef CHANNEL_H
#define CHANNEL_H

//---------------------------------------------------------------------------

#include <core/container/TypedMap.h>
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

	class MessageReceiver : public Shared {};

	struct BasicReceivers : Shared {};

	template<class Dst, class Msg>
	using ReceiversList = vector<Receiver<Dst, Msg>>;

	template<class Dst, class Msg>
	struct Receivers : BasicReceivers
	{
		ReceiversList<Dst, Msg> contents;

		static const bool exist = same_type<decltype(declval<Dst>().receiversMap), ReceiversMap>::value && not_same_type<message_dst_t<Dst, Msg>, Empty>::value;

		template<useif <exist> endif>
		static ReceiversList<Dst, Msg> & get(Dst & dest)
		{
			return dest.receiversMap.request<Receivers<Dst, Msg>>()->contents;
		}
	};

	using ReceiversMap = TypedMap<BasicReceivers>;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg, bool isDest = is_dest<Dst, Msg>::value>
	class MessageConnector {};

	template<class Dst, typename Msg, bool isDest = is_dest<Dst, Msg>::value>
	class MessageBroadcaster {};

	template<class Dst, typename Msg, bool hasReader = has_reader<Dst, Msg>::value>
	class MessageTransmitter {};

//---------------------------------------------------------------------------

	class BasicChannel {};

	template<class Dst, typename Msg>
	class Channel : public MessageConnector<Dst, Msg>
	{
	public:
		typedef MessageConnector<Dst, Msg>   Connector;
		typedef MessageBroadcaster<Dst, Msg> Broadcaster;
		typedef MessageTransmitter<Dst, Msg> Transmitter;

		friend class MessageConnector<Dst, Msg>;
		friend class MessageBroadcaster<Dst, Msg>;

		static void transmit(Handle<Msg> & message, Dst & dest)
		{
			Broadcaster::broadcast(message, dest);

			auto & r = instance().receivers();

			if(r.size() > 0)
			{
				for(auto & receiver : r)
					receiver.callback(message, dest);
			}

			Transmitter::transmit(message, dest);
		}
	};

	template<class Dst, typename Msg>
	using channel_t = Channel<message_dst_t<Dst, Msg>, Msg>;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageConnector<Dst, Msg, false> : public Singleton<MessageConnector<Dst, Msg, false>>
	{
		friend Channel<Dst, Msg>;

	public:
		typedef Receiver<Dst, Msg> Rcvr;

		static size_t connect(const Rcvr & receiver)
		{
			receivers().push_back(receiver);
			return receiver.id;
		}

		size_t operator += (const Rcvr & receiver)
		{
			_receivers.contents.push_back(receiver);
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

	protected:
		static ReceiversList<Dst, Msg> & receivers()
		{
			return instance()._receivers.contents;
		}

		Receivers<Dst, Msg> _receivers;
	};

	template<class Dst, typename Msg>
	class MessageConnector<Dst, Msg, true> : public MessageConnector<Dst, Msg, false>
	{
		typedef MessageConnector<Dst, Msg, false> Base;

	public:
		typedef Receiver<Dst, Msg> Rcvr;

		struct ReceiversWrapper
		{
			ReceiversList<Dst, Msg> & receivers;

			size_t operator += (const Rcvr & receiver)
			{
				receivers.push_back(receiver);
				return receiver.id;
			}
		};

		using Base::connect;
		using Base::disconnect;

		static size_t connect(Dst & dest, const Rcvr & receiver)
		{
			Receivers<Dst, Msg>::get(dest).push_back(receiver);
			return receiver.id;
		}

		static ReceiversWrapper receivers(Dst & dest)
		{
			return {Receivers<Dst, Msg>::get(dest)};
		}

		static void disconnect(Dst & dest, const Rcvr & receiver)
		{
			auto & receivers = Receivers<Dst, Msg>::get(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(*i == receiver)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		static void disconnect(Dst & dest, size_t id, size_t additional)
		{
			auto & receivers = Receivers<Dst, Msg>::get(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id && i->additional == additional)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		static void disconnect(Dst & dest, size_t id)
		{
			auto & receivers = Receivers<Dst, Msg>::get(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id)
					i = receivers.erase(i);
			}
		}

		template<class R>
		static void disconnect(Dst & dest, const Handle<R> & rcvr)
		{
			auto & receivers = Receivers<Dst, Msg>::get(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(static_cast<const R *>(rcvr))))
					i = receivers.erase(i);
			}
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageBroadcaster<Dst, Msg, true>
	{
		friend Channel<Dst, Msg>;

		static void broadcast(Handle<Msg> & message, Dst & dest)
		{
			auto & local = Receivers<Dst, Msg>::get(dest);

			if(local.size() > 0)
			{
				for(auto & receiver : local)
					receiver.callback(message, dest);
			}
		}
	};

	template<class Dst, typename Msg>
	class MessageBroadcaster<Dst, Msg, false>
	{
		friend Channel<Dst, Msg>;

		static void broadcast(Handle<Msg> & message, Dst & dest) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageTransmitter<Dst, Msg, true>
	{
		friend Channel<Dst, Msg>;

		static void transmit(Handle<Msg> & message, Dst & dest)
		{
			dest.read(message);
		}
	};

	template<class Dst, typename Msg>
	class MessageTransmitter<Dst, Msg, false>
	{
		friend Channel<Dst, Msg>;

		static void transmit(Handle<Msg> & message, Dst & dest) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	struct is_dest
	{
		static const bool value = Receivers<Dst, Msg>::exist;
	};

	method_checker(has_read_function, read);

	template<class Dst, typename Msg>
	struct has_reader : has_read_function<Dst, Handle<Msg> &> {};

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

	template<class Dst, typename Msg, class Functor,
		useif <std::is_convertible<Functor, msg_callback<Dst, Msg>>::value> endif
	>
	size_t connect(Functor receiver)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg>
	size_t connect(Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg>
	size_t connect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(Receiver<Dst, Msg>(receiver, callback, key));
	}

//---------------------------------------------------------------------------

#define can_be_receiver(Functor) can_construct<MessageCallbackFunction<Dst, Msg>, Functor>::value


	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	size_t connect(RealDst & dest, const Receiver<Dst, Msg> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	size_t connect(RealDst & dest, void receiver(Handle<Msg> &, Dst &))
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	size_t connect(RealDst & dest, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class Functor, class RealDst,
		useif <
			is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value &&
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value
		> endif
	>
	size_t connect(RealDst & dest, Functor receiver)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	size_t connect(RealDst & dest, Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
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

	template<class Dst, typename Msg, class Functor,
		useif<
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

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, const Receiver<Dst, Msg> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, size_t id)
	{
		Channel<Dst, Msg>::disconnect(dest, id);
	}

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, void receiver(Handle<Msg> &, Dst &))
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class Functor, class RealDst,
		useif<
			is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value &&
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value &&
			!is_handle<Functor>::value
		> endif
	>
	void disconnect(RealDst & dest, Functor receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, Receiver<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr, class RealDst,
		useif <is_dest<Dst, Msg>::value && based_on<RealDst, Dst>::value> endif
	>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

//---------------------------------------------------------------------------

#define dest_connect(realDst, Dst, Msg) Channel<Dst, Msg>::receivers(realDst) += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)
#define global_connect(Dst, Msg) Channel<Dst, Msg>::instance() += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest)

//---------------------------------------------------------------------------

#define declare_receivers() 								\
	template<class, typename>								\
	friend struct Rapture::Receivers;						\
	Rapture::ReceiversMap receiversMap;						\

#define bind_message(Dst, Msg)								\
	template<class, typename>								\
	friend struct Rapture::Receivers;						\
	template<class, typename>								\
	friend struct Rapture::DestGetter;						\
															\
	set_message_dest(Dst, Msg)								\

#define op_bind_message(s, Dst, Msg) set_message_dest(Dst, Msg)

#define bind_messages(Dst, ... /* Messages */)				\
	template<class, typename>								\
	friend struct Rapture::Receivers;						\
	template<class, typename>								\
	friend struct Rapture::DestGetter;						\
															\
	pp_seq_foreach(op_bind_message, Dst,					\
		pp_tuple_to_seq((__VA_ARGS__))						\
		)													\

#define create_custom_reader(Msg, msg)						\
	template<class, typename ...>							\
	friend struct Rapture::has_read_function;				\
	template<class, typename, bool>							\
	friend class Rapture::MessageTransmitter;				\
															\
	virtual void read(Rapture::Handle<Msg> & msg)			\

#define create_reader(Msg)									\
	create_custom_reader(Msg, msg)									

#define op_create_reader(s, state, Msg) state virtual void read(Rapture::Handle<Msg> &);

#define create_readers(...)									\
	template<class, typename ...>							\
	friend struct Rapture::has_read_function;				\
	template<class, typename, bool>							\
	friend class Rapture::MessageTransmitter;				\
															\
	pp_seq_fold(op_create_reader,							\
		pp_tuple_to_seq((__VA_ARGS__))						\
		)													\

#define override_custom_reader(Msg, msg)					\
	create_custom_reader(Msg, msg) override									

#define override_reader(Msg)								\
	override_custom_reader(Msg, msg)									

#define implement_custom_reader(Dst, Msg, msg)	void Dst::read(Handle<Msg> & msg)
#define implement_reader(Dst, Msg)	implement_custom_reader(Dst, Msg, msg)
}

//---------------------------------------------------------------------------
#endif