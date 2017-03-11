//---------------------------------------------------------------------------

#pragma once

#ifndef CONNECT_H
#define CONNECT_H

//---------------------------------------------------------------------------

#include "Channel.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Connector;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg, useif<is_message<Msg>::value>>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver);
	template<class Dst, typename Msg, class RealDst, useif<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value>>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest);

//---------------------------------------------------------------------------

	class BasicConnection : public Shared
	{
	public:
		BasicConnection(size_t id) : id(id) {}
		virtual ~BasicConnection() {}

	protected:
		size_t id;
	};

	template<class Dst, typename Msg, class RealDst = Empty>
	class Connection : public BasicConnection
	{
	public:
		Connection(size_t id, RealDst & dest) : BasicConnection(id), dest(dest) {}
		virtual ~Connection() { Channel<Dst, Msg>::disconnect(dest, id); }

	protected:
		RealDst & dest;
	};

	template<class Dst, typename Msg>
	class Connection<Dst, Msg, Empty> : public BasicConnection
	{
	public:
		Connection(size_t id) : BasicConnection(id) {}
		virtual ~Connection() { Channel<Dst, Msg>::disconnect(id); }
	};

	class Connector
	{
		template<class Dst, typename Msg, used_t>
		friend void connect(Connector &, const function<void(Handle<Msg> &, Dst &)> &);
		template<class Dst, typename Msg, class RealDst, used_t>
		friend void connect(Connector &, const function<void(Handle<Msg> &, Dst &)> &, RealDst &);

		template<class Dst, typename Msg>
		void add(size_t id)
		{
			_connections.push_back(Handle<Connection<Dst, Msg>>(id));
		}

		template<class Dst, typename Msg, class RealDst>
		void add(size_t id, RealDst & dest)
		{
			_connections.push_back(Handle<Connection<Dst, Msg, RealDst>>(id, dest));
		}

		ArrayList<BasicConnection> _connections;
	};

//---------------------------------------------------------------------------

	/**
	 *	Permanent connections (lifetime of a connection < lifetime of a receiver >= lifetime of a channel/destination)
	 */

	template<class Dst, typename Msg, typename MsgDst = message_dst_t<Dst, Msg>, useif<is_message<Msg>::value>, useif<is_same<MsgDst, Dst>::value>>
	void connect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<Dst, Msg>::connect(receiver);
	}

	template<class Dst, typename Msg, typename MsgDst = message_dst_t<Dst, Msg>, useif<is_message<Msg>::value>, skipif<is_same<MsgDst, Dst>::value>, useif<is_base_of<MsgDst, Dst>::value>>
	void connect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<MsgDst, Msg>::connect(make_function([&receiver](Handle<Msg> & msg, MsgDst & dst) {
			receiver(msg, static_cast<Dst &>(dst));
		}));
	}

	template<class Functor, useif<can_connect_functor<Functor>::value>, skipif<based_on<Functor, Connector>::value>>
	void connect(Functor & receiver)
	{
		connect(make_function(receiver));
	}

	template<class Functor, useif<can_connect_functor<Functor>::value>, skipif<based_on<Functor, Connector>::value>>
	void connect(Functor && receiver)
	{
		connect(make_function(forward<Functor>(receiver)));
	}

	template<class Rcvr, class Method, useif<is_method<Rcvr, Method>::value, can_connect_method<Method>::value>, skipif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr receiver, Method method)
	{
		connect(make_function(receiver, method));
	}

	template<class Dst, typename Msg, class RealDst, typename MsgDst = message_dst_t<Dst, Msg>, useif<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value>, useif<is_same<MsgDst, Dst>::value>>
	void connect(const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest)
	{
		Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Dst, typename Msg, class RealDst, typename MsgDst = message_dst_t<Dst, Msg>, useif<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value>, skipif<is_same<MsgDst, Dst>::value>, useif<is_base_of<MsgDst, Dst>::value>>
	void connect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<MsgDst, Msg>::connect(make_function([&receiver](Handle<Msg> & msg, MsgDst & dst) {
			receiver(msg, static_cast<Dst &>(dst));
		}));
	}

	template<class Functor, class RealDst, useif<can_connect_functor<Functor, RealDst>::value>, skipif<based_on<Functor, Connector>::value>>
	void connect(Functor & receiver, RealDst & dest)
	{
		connect(make_function(receiver), dest);
	}

	template<class Functor, class RealDst, useif<can_connect_functor<Functor, RealDst>::value>, skipif<based_on<Functor, Connector>::value>>
	void connect(Functor && receiver, RealDst & dest)
	{
		connect(make_function(forward<Functor>(receiver)), dest);
	}

	template<class Rcvr, class Method, class RealDst, useif<is_method<decay_t<Rcvr>, Method>::value, can_connect_method<Method, RealDst>::value>, skipif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr receiver, Method method, RealDst & dest)
	{
		connect(make_function(receiver, method), dest);
	}

//---------------------------------------------------------------------------

	/**
	 *	Collectable connections (lifetime of a connection < lifetime of a receiver < lifetime of a channel/destination)
	 */

	template<class Dst, typename Msg, used_t>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		c.add<Dst, Msg>(Channel<Dst, Msg>::connect(receiver));
	}

	template<class Functor, useif<can_connect_functor<Functor>::value>>
	void connect(Connector & c, Functor & receiver)
	{
		connect(c, make_function(receiver));
	}

	template<class Functor, useif<can_connect_functor<Functor>::value>>
	void connect(Connector & c, Functor && receiver)
	{
		connect(c, make_function(forward<Functor>(receiver)));
	}

	template<class Rcvr, class Method, useif<is_method<Rcvr, Method>::value, can_connect_method<Method>::value>>
	void connect(Connector & c, Rcvr receiver, Method method)
	{
		connect(c, make_function(receiver, method));
	}

	template<class Dst, typename Msg, class RealDst, used_t>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest)
	{
		c.add<Dst, Msg>(Channel<Dst, Msg>::connect(dest, receiver), dest);
	}

	template<class Functor, class RealDst, useif<can_connect_functor<Functor, RealDst>::value>>
	void connect(Connector & c, Functor & receiver, RealDst & dest)
	{
		connect(c, make_function(receiver), dest);
	}

	template<class Functor, class RealDst, useif<can_connect_functor<Functor, RealDst>::value>>
	void connect(Connector & c, Functor && receiver, RealDst & dest)
	{
		connect(c, make_function(forward<Functor>(receiver)), dest);
	}

	template<class Rcvr, class Method, class RealDst, useif<is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value>>
	void connect(Connector & c, Rcvr receiver, Method method, RealDst & dest)
	{
		connect(c, make_function(receiver, method), dest);
	}

//---------------------------------------------------------------------------

	/**
	 *	Automatic connections (lifetime of a connection = lifetime of a receiver < lifetime of a channel/destination)
	 */

	template<class Rcvr, useif<can_connect_functor<Rcvr>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr * receiver)
	{
		connect(*receiver, make_function(receiver));
	}

	template<class Rcvr, class RealDst, useif<can_connect_functor<Rcvr, RealDst>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr * receiver, RealDst & dest)
	{
		connect(*receiver, make_function(receiver), dest);
	}

	template<class Rcvr, class Method, useif<is_method<Rcvr, Method>::value, can_connect_method<Method>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr * receiver, Method method)
	{
		connect(*receiver, make_function(receiver, method));
	}

	template<class Rcvr, class Method, class RealDst, useif<is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr * receiver, Method method, RealDst & dest)
	{
		connect(*receiver, make_function(receiver, method), dest);
	}

	template<class Rcvr, useif<can_connect_functor<Rcvr>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr & receiver)
	{
		connect(receiver, make_function(receiver));
	}

	template<class Rcvr, class RealDst, useif<can_connect_functor<Rcvr, RealDst>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr & receiver, RealDst & dest)
	{
		connect(receiver, make_function(receiver), dest);
	}

	template<class Rcvr, class Method, useif<is_method<Rcvr, Method>::value, can_connect_method<Method>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr & receiver, Method method)
	{
		connect(receiver, make_function(receiver, method));
	}

	template<class Rcvr, class Method, class RealDst, useif<is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value>, useif<based_on<Rcvr, Connector>::value>>
	void connect(Rcvr & receiver, Method method, RealDst & dest)
	{
		connect(receiver, make_function(receiver, method), dest);
	}
}

//---------------------------------------------------------------------------
#endif
