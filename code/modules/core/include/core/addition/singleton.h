//---------------------------------------------------------------------------

#pragma once

#ifndef SINGLETON_H
#define SINGLETON_H

//---------------------------------------------------------------------------

#include <meta/meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct common_threading_model {};
	struct thread_local_model {};

	template<class T>
	struct default_initializer {};

	template<class T>
	struct variable_initializer
	{
		static T initialize() { return {}; }
	};

	template<
		class T,
		class ThreadingModel = common_threading_model,
		template<class> class Initializer = default_initializer
	>
	struct singleton
	{
		static T & instance()
		{
			static T instance = Initializer<T>::initialize();
			return instance;
		}
	};

	template<
		class T,
		template<class> class Initializer
	>
	struct singleton<T, thread_local_model, Initializer>
	{
		static T & instance()
		{
			static thread_local T instance = Initializer<T>::initialize();
			return instance;
		}
	};

	template<class T>
	struct singleton<T, common_threading_model, default_initializer>
	{
		static T & instance()
		{
			static T instance;
			return instance;
		}
	};

	template<class T>
	struct singleton<T, thread_local_model, default_initializer>
	{
		static T & instance()
		{
			static thread_local T instance;
			return instance;
		}
	};

	template<class T, class ThreadingModel = common_threading_model>
	using copy_singleton = singleton<T, ThreadingModel, variable_initializer>;
}

//---------------------------------------------------------------------------
#endif
