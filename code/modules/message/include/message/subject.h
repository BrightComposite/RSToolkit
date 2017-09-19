//---------------------------------------------------------------------------

#pragma once

#ifndef SUBJECT_H
#define SUBJECT_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <core/Hash.h>

#include <core/addition/named.h>
#include <container/set.h>
#include "Connect.h"

//---------------------------------------------------------------------------

namespace asd
{
	class subject;
	
	/**
	 *	@brief
	 *	Receiver must be initialized after this message
	 */
	message_class(
		InitializeMessage,
		(int, type)
	);
	
	class subject : public shareable<subject>, public named, public auto_id
	{
	public:
		subject() {}
		
		subject(const string & name) : named(name) {}
		
		subject(string && name) : named(forward<string>(name)) {}
		
		subject(const subject & subj) : named(subj) {}
		
		subject(subject && subj) : named(forward<subject>(subj)) {}
		
		virtual ~subject() {}
		
		virtual void enable() {
			_enabled = true;
		}
		
		virtual void disable() {
			_enabled = false;
		}
		
		bool enabled() const {
			return _enabled;
		}
		
		bool disabled() const {
			return !_enabled;
		}
		
		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const subject *, F ...>::value>>
		handle<Msg> send(Dst & dest, F && ... fields) const {
			handle<Msg> msg(this, forward<F>(fields)...);
			return Channel<Dst, Msg>::transmit(msg, dest);
		}
		
		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const subject *, F ...>::value>>
		handle<Msg> send(Dst * dest, F && ... fields) const {
			if(dest == nullptr) {
				throw Exception("Destination should be not null!");
			}
			
			handle<Msg> msg(this, forward<F>(fields)...);
			return Channel<Dst, Msg>::transmit(msg, *dest);
		}
		
		template<class Dst, class Msg>
		handle<Msg> & resend(handle<Msg> & message, Dst & dest) const {
			auto * src = message->source;
			message->source = this;
			Channel<Dst, Msg>::transmit(message, dest);
			message->source = src;
			
			return message;
		}
		
		template<class Dst, class Msg>
		handle<Msg> & resend(handle<Msg> & message, Dst * dest) const {
			if(dest == nullptr) {
				throw Exception("Destination should be not null!");
			}
			
			auto * src = message->source;
			message->source = this;
			Channel<Dst, Msg>::transmit(message, *dest);
			message->source = src;
			
			return message;
		}
		
		static const subject * universe() {
			static subject u("Universe");
			return &u;
		}
	
	protected:
		bool _enabled = true;
	
	private:
		declare_receivers();
	};
	
	template<class Msg, class Dst, class ... F, useif<can_construct<Msg, subject *, F ...>::value>>
	handle<Msg> send(Dst & dest, F && ... fields) {
		handle<Msg> msg(subject::universe(), forward<F>(fields)...);
		return Channel<Dst, Msg>::transmit(msg, dest);
	}
	
	template<class Dst, class Msg>
	handle<Msg> & resend(handle<Msg> & message, Dst & dest) {
		auto * src = message->source;
		message->source = subject::universe();
		Channel<Dst, Msg>::transmit(message, dest);
		message->source = src;
		
		return message;
	}
	
	template<>
	struct hash<subject> : hash<auto_id> {};
}

//---------------------------------------------------------------------------
#endif
