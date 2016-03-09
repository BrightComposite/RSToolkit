//---------------------------------------------------------------------------

#ifndef SINGLETON_H
#define SINGLETON_H

//---------------------------------------------------------------------------

#include <core/meta/Meta.h>

//---------------------------------------------------------------------------

namespace Rapture
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
	class Singleton
	{
	public:
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
	class Singleton<T, ThreadLocalModel, Initializer>
	{
	public:
		static T & instance()
		{
			static thread_local T instance = Initializer<T>::initialize();
			return instance;
		}
	};

	template<class T>
	class Singleton<T, CommonThreadingModel, DefaultInitializer>
	{
	public:
		static T & instance()
		{
			static T instance;
			return instance;
		}
	};

	template<class T>
	class Singleton<T, ThreadLocalModel, DefaultInitializer>
	{
	public:
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
