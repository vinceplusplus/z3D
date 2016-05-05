
#pragma once

#include "predecl.h"

#pragma warning(push)
#pragma warning(disable:4251)

// use this macro to implement the singleton current instance
#define IMPLEMENT_SINGLETON(T)		T*		z3D::Core::singleton<T>::_current = 0;

namespace z3D
{
	namespace Core
	{
		// actually the following are not singleton classes

		class singleton_base
		{
		protected:
			static vector<singleton_base*>		_array_singleton;
		public:
			singleton_base();
			virtual ~singleton_base();
		public:
			static void							shutdown();								// must be called before program exit to ensure deleting all singletons
		};

		template<class T>
		class singleton: public singleton_base
		{
		protected:
			static T*							_current;
		public:
			virtual ~singleton();
		public:
			static T*							current();
			template<class U>
			static U*							current();
		public:
			void								makeCurrent();
			void								makeNotCurrent();
			bool								isCurrent() const;
		};

		template<class T>
		singleton<T>::~singleton()
		{
			makeNotCurrent();
		}

		template<class T>
		T*											singleton<T>::current() {return _current;}

		template<class T>template<class U>
		U*											singleton<T>::current() {return (U*)_current;}

		template<class T>
		void										singleton<T>::makeCurrent()
		{
			_current = (T*)this;
		}

		template<class T>
		void										singleton<T>::makeNotCurrent()
		{
			if(_current == (T*)this)
				_current = 0;
		}

		template<class T>
		bool										singleton<T>::isCurrent() const
		{
			return (_current == this);
		}
	};
};

#pragma warning(pop)

