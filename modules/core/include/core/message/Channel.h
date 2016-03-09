//---------------------------------------------------------------------------

#ifndef CHANNEL_H
#define CHANNEL_H

//---------------------------------------------------------------------------

#include <core/container/Array.h>
#include <core/Exception.h>
#include "Message.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	method_checker(has_read_function, read);

	template<class Dst, typename Msg>
	struct has_reader : has_read_function<Dst, Handle<Msg> &, const Subject *> {};

	template<class Dst, typename Msg>
	struct has_local_receivers
	{
		static const bool value = MessageDispatcher<Dst, Msg>::hasDestination;
	};

	template<class Dst, typename Msg>
	struct has_linked_channel
	{
		static const bool value = has_reader<Dst, Msg>::value && has_local_receivers<Dst, Msg>::value;
	};

//---------------------------------------------------------------------------

	class MessageReceiver : public Shareable<MessageReceiver> {};

	template<class Dst, typename Msg>
	using MessageCallbackList = vector<MessageCallback<Dst, Msg>>;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg, bool hasLocalReceivers = has_local_receivers<Dst, Msg>::value>
	class MessageBroadcaster {};

	template<class Dst, typename Msg, bool hasReader = has_reader<Dst, Msg>::value>
	class MessageTransmitter {};

	template<class Dst, typename Msg, bool hasLocalReceivers = has_local_receivers<Dst, Msg>::value>
	class MessageConnector {};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class Channel : public MessageConnector<Dst, Msg>
	{
	public:
		typedef MessageBroadcaster<Dst, Msg> Broadcaster;
		typedef MessageTransmitter<Dst, Msg> Transmitter;

		static void transmit(Handle<Msg> & message, Dst & dest, const Subject * source)
		{
			try
			{
				Broadcaster::broadcast(message, dest, source);

				auto & r = instance().receivers;

				if(r.size() > 0)
				{
					for(auto & receiver : r)
						receiver.callback(message, dest, source);
				}

				Transmitter::transmit(message, dest, source);
			}
			catch(const Interruption &)
			{}
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageBroadcaster<Dst, Msg, true>
	{
		friend Channel<Dst, Msg>;
		typedef MessageDispatcher<Dst, Msg> Dispatcher;

		static void broadcast(Handle<Msg> & message, Dst & dest, const Subject * source)
		{
			auto & local = Dispatcher::receivers(dest);

			if(local.size() > 0)
			{
				for(auto & receiver : local)
					receiver.callback(message, dest, source);
			}
		}
	};

	template<class Dst, typename Msg>
	class MessageBroadcaster<Dst, Msg, false>
	{
		friend Channel<Dst, Msg>;

		static void broadcast(Handle<Msg> & message, Dst & dest, const Subject * source) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageTransmitter<Dst, Msg, true>
	{
		friend Channel<Dst, Msg>;

		static void transmit(Handle<Msg> & message, Dst & dest, const Subject * source)
		{
			dest.read(message, source);
		}
	};

	template<class Dst, typename Msg>
	class MessageTransmitter<Dst, Msg, false>
	{
		friend Channel<Dst, Msg>;

		static void transmit(Handle<Msg> & message, Dst & dest, const Subject * source) {}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	class MessageConnector<Dst, Msg, false> : public Singleton<MessageConnector<Dst, Msg, false>>
	{
		friend Channel<Dst, Msg>;

	public:
		typedef MessageCallback<Dst, Msg> Receiver;

		static size_t connect(const Receiver & receiver)
		{
			instance().receivers.push_back(receiver);
			return receiver.id;
		}

		size_t operator += (const Receiver & receiver)
		{
			receivers.push_back(receiver);
			return receiver.id;
		}

		static void disconnect(const Receiver & receiver)
		{
			auto & receivers = instance().receivers;

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(*i == receiver)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		static void disconnect(size_t id, size_t additional)
		{
			auto & receivers = instance().receivers;

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id && i->additional == additional)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		static void disconnect(size_t id)
		{
			auto & receivers = instance().receivers;

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id)
					i = receivers.erase(i);
			}
		}

		template<class Rcvr>
		static void disconnect(const Handle<Rcvr> & rcvr)
		{
			auto & receivers = instance().receivers;

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(static_cast<const Rcvr *>(rcvr))))
					i = receivers.erase(i);
			}
		}

	protected:
		MessageCallbackList<Dst, Msg> receivers;
	};

	template<class Dst, typename Msg>
	class MessageConnector<Dst, Msg, true> : public MessageConnector<Dst, Msg, false>
	{
		typedef MessageConnector<Dst, Msg, false> Base;

	public:
		typedef MessageCallback<Dst, Msg> Receiver;
		typedef MessageDispatcher<Dst, Msg> Dispatcher;

		struct Receivers
		{
			MessageCallbackList<Dst, Msg> & receivers;

			size_t operator += (const Receiver & receiver)
			{
				receivers.push_back(receiver);
				return receiver.id;
			}
		};

		using Base::connect;
		using Base::disconnect;

		template<class RealDst,
			require(
				based_on(RealDst, Dst)
				)>
		static size_t connect(RealDst & dest, const Receiver & receiver)
		{
			Dispatcher::receivers(dest).push_back(receiver);
			return receiver.id;
		}

		template<class RealDst,
			require(
				based_on(RealDst, Dst)
				)>
		static Receivers receivers(RealDst & dest)
		{
			return {Dispatcher::receivers(dest)};
		}

		template<class RealDst,
			require(
				based_on(RealDst, Dst)
				)>
		static void disconnect(RealDst & dest, const Receiver & receiver)
		{
			auto & receivers = Dispatcher::receivers(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(*i == receiver)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		template<class RealDst,
			require(
				based_on(RealDst, Dst)
				)>
		static void disconnect(RealDst & dest, size_t id, size_t additional)
		{
			auto & receivers = Dispatcher::receivers(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id && i->additional == additional)
				{
					receivers.erase(i);
					break;
				}
			}
		}

		template<class RealDst,
			require(
				based_on(RealDst, Dst)
				)>
		static void disconnect(RealDst & dest, size_t id)
		{
			auto & receivers = Dispatcher::receivers(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == id)
					i = receivers.erase(i);
			}
		}

		template<class RealDst, class Rcvr,
			require(
				based_on(RealDst, Dst)
				)>
		static void disconnect(RealDst & dest, const Handle<Rcvr> & rcvr)
		{
			auto & receivers = Dispatcher::receivers(dest);

			for(auto i = receivers.begin(); i != receivers.end(); ++i)
			{
				if(i->id == reinterpret_cast<size_t>(static_cast<const void *>(static_cast<const Rcvr *>(rcvr))))
					i = receivers.erase(i);
			}
		}
	};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	size_t connect(const MessageCallback<Dst, Msg> & receiver)
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg>
	size_t connect(void receiver(Handle<Msg> &, Dst &, const Subject *))
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg>
	size_t connect(const function<void(Handle<Msg> &, Dst &, const Subject *)> & receiver)
	{
		return Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg, class Functor,
		require(
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value
			)>
	size_t connect(Functor receiver)
	{
		return Channel<Dst, Msg>::connect(MessageCallback<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg>
	size_t connect(Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg>
	size_t connect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(MessageCallback<Dst, Msg>(receiver, callback, key));
	}

//---------------------------------------------------------------------------

#define can_be_receiver(Functor) can_construct(MessageCallbackFunction<Dst, Msg>, Functor)

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	size_t connect(RealDst & dest, const MessageCallback<Dst, Msg> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	size_t connect(RealDst & dest, void receiver(Handle<Msg> &, Dst &, const Subject *))
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	size_t connect(RealDst & dest, const function<void(Handle<Msg> &, Dst &, const Subject *)> & receiver)
	{
		return Channel<Dst, Msg>::connect(dest, receiver);
	}

	template < class Dst, typename Msg, class RealDst, class Functor,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value &&
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value
			)>
	size_t connect(RealDst & dest, Functor receiver)
	{
		return Channel<Dst, Msg>::connect(dest, MessageCallback<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	size_t connect(RealDst & dest, Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *), size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	size_t connect(RealDst & dest, const Handle<Rcvr> & receiver, void (__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *) const, size_t key = 0)
	{
		return Channel<Dst, Msg>::connect(dest, MessageCallback<Dst, Msg>(receiver, callback, key));
	}

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	void disconnect(const MessageCallback<Dst, Msg> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg>
	void disconnect(size_t id)
	{
		Channel<Dst, Msg>::disconnect(id);
	}

	template<class Dst, typename Msg>
	void disconnect(void receiver(Handle<Msg> &, Dst &, const Subject *))
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg>
	void disconnect(const function<void(Handle<Msg> &, Dst &, const Subject *)> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	template<class Dst, typename Msg, class Functor,
		require(
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value &&
			!is_handle<Functor>::value
			)>
	void disconnect(Functor receiver)
	{
		Channel<Dst, Msg>::disconnect(MessageCallback<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg>
	void disconnect(Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg>
	void disconnect(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class Rcvr>
	void disconnect(const Handle<Rcvr> & receiver)
	{
		Channel<Dst, Msg>::disconnect(receiver);
	}

	//---------------------------------------------------------------------------

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, const MessageCallback<Dst, Msg> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, size_t id)
	{
		Channel<Dst, Msg>::disconnect(dest, id);
	}

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, void receiver(Handle<Msg> &, Dst &, const Subject *))
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, const function<void(Handle<Msg> &, Dst &, const Subject *)> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

	template < class Dst, typename Msg, class RealDst, class Functor,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value &&
			std::is_convertible<Functor, msg_callback<Dst, Msg>>::value &&
			!is_handle<Functor>::value
		)>
	void disconnect(RealDst & dest, Functor receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, MessageCallback<Dst, Msg>(receiver, std::addressof(receiver)));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *), size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Rcvr, class Dst, typename Msg, class RealDst,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *) const, size_t key = 0)
	{
		Channel<Dst, Msg>::disconnect(dest, MessageCallback<Dst, Msg>(receiver, callback, key));
	}

	template<class Dst, typename Msg, class RealDst, class Rcvr,
		require(
			based_on(RealDst, Dst) &&
			has_local_receivers<Dst, Msg>::value
			)>
	void disconnect(RealDst & dest, const Handle<Rcvr> & receiver)
	{
		Channel<Dst, Msg>::disconnect(dest, receiver);
	}

//---------------------------------------------------------------------------

#define dest_connect(realDst, Dst, Msg) Channel<Dst, Msg>::receivers(realDst) += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest, const Subject * src)
#define global_connect(Dst, Msg) Channel<Dst, Msg>::instance() += (msg_callback<Dst, Msg>)[](Handle<Msg> & msg, Dst & dest, const Subject * src)

//---------------------------------------------------------------------------

#define msg_set_dest(Dst, Msg)											\
	template<class Dst, typename Msg>									\
	friend class Rapture::MessageDispatcher;							\
	Rapture::UniqueHandle<Rapture::MessageCallbackList<Dst, Msg>>		\
		receiversOf##Msg;						

#define msg_create_custom_reader(Dst, Msg, msg, source)					\
	template<class, typename ...>										\
	friend struct Rapture::has_read_function;							\
	template<class, typename, bool>										\
	friend class Rapture::MessageTransmitter;							\
	virtual void read(													\
		Rapture::Handle<Msg> & msg,										\
		const Rapture::Subject * source									\
		)															

#define msg_create_reader(Dst, Msg)										\
	msg_create_custom_reader(Dst, Msg, msg, source)									

#define msg_create_custom_link(Dst, Msg, msg, source)					\
	msg_set_dest(Dst, Msg)												\
	msg_create_custom_reader(Dst, Msg, msg, source)											

#define msg_create_link(Dst, Msg)										\
	msg_create_custom_link(Dst, Msg, msg, source)			

#define msg_override_custom_reader(Dst, Msg, msg, source)				\
	msg_create_custom_reader(Dst, Msg, msg, source) override									

#define msg_override_reader(Dst, Msg)									\
	msg_override_custom_reader(Dst, Msg, msg, source)									

#define msg_implement_custom_reader(Dst, Msg, msg, source)	void Dst::read(Handle<Msg> & msg, const Subject * source)
#define msg_implement_reader(Dst, Msg)	msg_implement_custom_reader(Dst, Msg, msg, source)
}

//---------------------------------------------------------------------------
#endif