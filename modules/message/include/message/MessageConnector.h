//---------------------------------------------------------------------------

#pragma once

#ifndef MESSAGE_CONNECTOR_H
#define MESSAGE_CONNECTOR_H

//---------------------------------------------------------------------------

#include <container/ArrayList.h>
#include <forward_list>

#include "Message.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class Dst, class Msg>
	struct is_dest;

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	using ReceiversList = array_list<Receiver<Dst, Msg>>;

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct ReceiversWrapper
	{
		ReceiversList<Dst, Msg> & receivers;

		template<class F, useif<can_connect_functor<F>::value>>
		auto operator += (F & f) {
			return connect(f, &F::operator());
		}

		template<class F, useif<can_connect_functor<F>::value>>
		auto operator += (F && f) {
			return connect(forward<F>(f), &F::operator());
		}

	private:
		template<class F, class C>
		void connect(F & f, void (C::*mf) (Handle<Msg> &, Dst &)) {
			receivers.emplace_back(f);
		}

		template<class F, class C>
		void connect(F && f, void (C::*mf) (Handle<Msg> &, Dst &)) {
			receivers.emplace_back(forward<F>(f));
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F & f, void (C::*mf) (Handle<Msg> &, D &)) {
			receivers.emplace_back([&f](Handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F && f, void (C::*mf) (Handle<Msg> &, D &)) {
			receivers.emplace_back([f](Handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C>
		void connect(F & f, void (C::*mf) (Handle<Msg> &, Dst &) const) {
			receivers.emplace_back(f);
		}

		template<class F, class C>
		void connect(F && f, void (C::*mf) (Handle<Msg> &, Dst &) const) {
			receivers.emplace_back(forward<F>(f));
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F & f, void (C::*mf) (Handle<Msg> &, D &) const) {
			receivers.emplace_back([&f](Handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F && f, void (C::*mf) (Handle<Msg> &, D &) const) {
			receivers.emplace_back([f](Handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}
	};

//---------------------------------------------------------------------------

	template<class MsgDst, typename Msg, bool = is_dest<MsgDst, Msg>::value>
	struct MessageConnector {};

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, false>
	{
		using Rcvr = Receiver<Dst, Msg>;

		static size_t connect(Rcvr && receiver) {
			size_t id = receiver.id;
			globalReceivers().push_back(forward<Rcvr>(receiver));
			return id;
		}

		static void disconnect(size_t id) {
			auto & r = globalReceivers();

			r.erase(std::remove_if(r.begin(), r.end(), [id](Rcvr & rcvr) {
				return rcvr.id == id;
			}), r.end());
		}

		static forceinline ReceiversWrapper<Dst, Msg> receivers() {
			return {globalReceivers()};
		}

		static size_t count() {
			return globalReceivers().size();
		}

		static forceinline ReceiversList<Dst, Msg> & globalReceivers() {
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

		static size_t connect(Dst & dest, Rcvr && receiver) {
			return Rcvrs::addTo(dest, forward<Rcvr>(receiver));
		}

		static forceinline ReceiversWrapper<Dst, Msg> receivers(Dst & dest) {
			return {Rcvrs::requireFrom(dest)};
		}

		static void disconnect(Dst & dest, size_t id) {
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local == nullptr || local->empty()) {
				return;
			}

			local->erase(std::remove_if(local->begin(), local->end(), [id](Rcvr & rcvr) {
				return rcvr.id == id;
			}), local->end());
		}
	};
}

//---------------------------------------------------------------------------
#endif
