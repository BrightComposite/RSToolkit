//---------------------------------------------------------------------------

#pragma once

#ifndef SUBJECT_H
#define SUBJECT_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <core/Hash.h>

#include <core/addition/named.h>
#include <container/Set.h>
#include "Connect.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Subject;
	
	/**
	 *	@brief
	 *	Receiver must be initialized after this message
	 */
	message_class(
		InitializeMessage,
		(int, type)
	);
	
	class Subject : public shareable, public named, public AutoIdentifier
	{
	public:
		Subject() {}
		
		Subject(const String & name) : named(name) {}
		
		Subject(String && name) : named(forward<String>(name)) {}
		
		Subject(const Subject & subject) : named(subject) {}
		
		Subject(Subject && subject) : named(forward<Subject>(subject)) {}
		
		virtual ~Subject() {}
		
		virtual void enable() {
			_enabled = true;
		}
		
		virtual void disable() {
			_enabled = false;
		}
		
		bool isEnabled() const {
			return _enabled;
		}
		
		bool isDisabled() const {
			return !_enabled;
		}
		
		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const Subject *, F ...>::value>>
		handle<Msg> send(Dst & dest, F && ... fields) const {
			handle<Msg> msg(this, forward<F>(fields)...);
			return Channel<Dst, Msg>::transmit(msg, dest);
		}
		
		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const Subject *, F ...>::value>>
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
		
		static const Subject * universe() {
			static Subject u("Universe");
			return &u;
		}
	
	protected:
		bool _enabled = true;
	
	private:
		declare_receivers();
	};
	
	template<class Msg, class Dst, class ... F, useif<can_construct<Msg, Subject *, F ...>::value>>
	handle<Msg> send(Dst & dest, F && ... fields) {
		handle<Msg> msg(Subject::universe(), forward<F>(fields)...);
		return Channel<Dst, Msg>::transmit(msg, dest);
	}
	
	template<class Dst, class Msg>
	handle<Msg> & resend(handle<Msg> & message, Dst & dest) {
		auto * src = message->source;
		message->source = Subject::universe();
		Channel<Dst, Msg>::transmit(message, dest);
		message->source = src;
		
		return message;
	}
	
	template<>
	struct hash<Subject> : hash<AutoIdentifier> {};
}

//---------------------------------------------------------------------------
#endif
