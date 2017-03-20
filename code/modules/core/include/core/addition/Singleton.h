//---------------------------------------------------------------------------

#pragma once

#ifndef SINGLETON_H
#define SINGLETON_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct CommonThreadingModel {};
	struct ThreadLocalModel {};

	template<class T>
	struct DefaultInitializer {};

	template<class T>
	struct VariableInitializer
	{
		static T initialize() { return {}; }
	};

	template<
		class T,
		class ThreadingModel = CommonThreadingModel,
		template<class> class Initializer = DefaultInitializer
	>
	struct Singleton
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
	struct Singleton<T, ThreadLocalModel, Initializer>
	{
		static T & instance()
		{
			static thread_local T instance = Initializer<T>::initialize();
			return instance;
		}
	};

	template<class T>
	struct Singleton<T, CommonThreadingModel, DefaultInitializer>
	{
		static T & instance()
		{
			static T instance;
			return instance;
		}
	};

	template<class T>
	struct Singleton<T, ThreadLocalModel, DefaultInitializer>
	{
		static T & instance()
		{
			static thread_local T instance;
			return instance;
		}
	};

	template<class T, class ThreadingModel = CommonThreadingModel>
	using CopySingleton = Singleton<T, ThreadingModel, VariableInitializer>;
}

//---------------------------------------------------------------------------
#endif
