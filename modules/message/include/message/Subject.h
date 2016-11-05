//---------------------------------------------------------------------------

#pragma once

#ifndef SUBJECT_H
#define SUBJECT_H

//---------------------------------------------------------------------------

#include <core/Exception.h>
#include <core/Hash.h>

#include <core/addition/Named.h>
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

	link_class(message, Subject, Class<Object, Named>);

	class Subject : public Object, public Named, public AutoIdentifier
	{
	public:
		Subject() : Object(), Named()
		{
			setclass(Subject);
		}

		Subject(const String & name) : Object(), Named(name)
		{
			setclass(Subject);
		}

		Subject(String && name) : Object(), Named(forward<String>(name))
		{
			setclass(Subject);
		}

		Subject(const Subject & subject) : Object(), Named(subject)
		{
			setclass(Subject);
		}

		Subject(Subject && subject) : Object(), Named(forward<Subject>(subject))
		{
			setclass(Subject);
		}

		virtual ~Subject() {}

		void enable()
		{
			_enabled = true;
		}

		void disable()
		{
			_enabled = false;
		}

		bool isEnabled() const
		{
			return _enabled;
		}

		bool isDisabled() const
		{
			return !_enabled;
		}

		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const Subject *, F ...>::value>>
		Handle<Msg> send(Dst & dest, F &&... fields) const
		{
			Handle<Msg> msg(this, forward<F>(fields)...);
			return Channel<Dst, Msg>::transmit(msg, dest);
		}

		template<class Msg, class Dst, class ... F, useif<can_construct<Msg, const Subject *, F ...>::value>>
		Handle<Msg> send(Dst * dest, F &&... fields) const
		{
			if(dest == nullptr)
				throw Exception("Destination should be not null!");

			Handle<Msg> msg(this, forward<F>(fields)...);
			return Channel<Dst, Msg>::transmit(msg, *dest);
		}

		template<class Dst, class Msg>
		Handle<Msg> & resend(Handle<Msg> & message, Dst & dest) const
		{
			auto * src = message->source;
			message->source = this;
			Channel<Dst, Msg>::transmit(message, dest);
			message->source = src;

			return message;
		}

		template<class Dst, class Msg>
		Handle<Msg> & resend(Handle<Msg> & message, Dst * dest) const
		{
			if(dest == nullptr)
				throw Exception("Destination should be not null!");

			auto * src = message->source;
			message->source = this;
			Channel<Dst, Msg>::transmit(message, *dest);
			message->source = src;

			return message;
		}

		static const Subject * universe()
		{
			static Subject u("Universe");
			return &u;
		}

	protected:
		bool _enabled = true;

	private:
		declare_receivers();
	};

	template<class Msg, class Dst, class ... F, useif<can_construct<Msg, Subject *, F ...>::value>>
	Handle<Msg> send(Dst & dest, F &&... fields)
	{
		Handle<Msg> msg(Subject::universe(), forward<F>(fields)...);
		return Channel<Dst, Msg>::transmit(msg, dest);
	}

	template<class Dst, class Msg>
	Handle<Msg> & resend(Handle<Msg> & message, Dst & dest)
	{
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
