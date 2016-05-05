
#pragma once

#include "predecl.h"

#include "fast_string.h"

#include "functor.h"

#include "SPtr.h"
#include "WPtr.h"

namespace z3D
{
	namespace Core
	{
		// DO NOT directly derive from this class, derive from RefBase or ResBase instead
		// if objects of classes derived from RefBase or ResBase are not created using new then you will want to override the way how to destroy such objects, you can use setDestroyFunc to override. otherwise, delete is used to destroy those objects
		class RefBaseImpl: public noncopyable
		{
#ifdef SPTR_THREAD_SAFE
		private:
			static mutex		_mx_sptr;											// can be more fine tuned to use mutex_fast only, but bulky code...
#endif
		private:
			shared_count					_sc;
		private:
			struct UNIQUE_TRIGGER
			{
				functor1<void, void*>			func;
				void*							ref;
			}								_unique_trigger;
		private:
			functor1<void, RefBaseImpl*>	_destroy_func;
		protected:
			RefBaseImpl();
		protected:
			virtual ~RefBaseImpl();
		protected:
			virtual void					dispose();								// called by SPtr, override if necessary, default implementation: delete this; must have protected modifier to avoid accidentally called
		public:								// normally, SPtr<T>((T*)this) is sufficient for either rtti(language feature) on or off. the problem is that c style cast cannot cast a virtually inherited base class to its derived class, rtti on and dynamic_cast can do the job, but rtti is usually off in most projects and doing such dynamic_cast(from virtual base to derived ones) is slow
			template<class T>	SPtr<T>		SPtr_from_this_dynamic() const;			// using dynamic_cast, if runtime type information is not enabled, DO NOT use this method. 
		public:
			bool							checkHeldBySPtr() const;				// call to make sure this refbase instance is held by a SPtr
		public:								// use unique trigger with care, need to analyze the whole program, current users: stock
			void							setUniqueTrigger(const functor1<void, void*>& func, void* ref);
			void							getUniqueTrigger(functor1<void, void*>& func, void*& ref) const;
		public:
			void							setDestroyFunc(const functor1<void, RefBaseImpl*>& destroy_func);
			const functor1<void, RefBaseImpl*>&
											getDestroyFunc() const;
		public:
			template<class T> friend class	SPtr;
			template<class T> friend class	WPtr;
		};

		// Reference counted base class, usually use in conjunction with SPtr/WPtr
		// IMPORTANT NOTICE: all derived classes from RefBase or ResBase must have protected or private destructors, public destructors are not allowed because sometimes developers will accidentally write code to directly delete the objects
		class RefBase: virtual public RefBaseImpl
		{
		protected:
			RefBase();
			virtual ~RefBase();
		};

		// Resource base class, used for eliminating unnecessary loading of objects already existed in memory
		class ResBase: public RefBase
		{
		private:
			wcs								_filepath;
		private:
			ResMgr*							_res_mgr;
		protected:
			explicit ResBase(const wcs& filepath);
		protected:
			virtual ~ResBase();
		public:
			void							setResMgr(ResMgr* mgr);
			ResMgr*							getResMgr();
		public:
			const wcs&						filepath() const;
		protected:
			void							set_filepath(const wcs& filepath);
		};

		class SPtrLocker
		{
		private:
			static vector<SPtr<RefBase>>	_sptrs;
		private:
			SPtrLocker();
			~SPtrLocker();
		public:
			static void						add(const SPtr<RefBase>& sptr)		// add when inevitably use SPtr inside constructor
			{
				_sptrs.push_back(sptr);
			}
			static void						clear()								// when you are sure you are in a very outer scope, call this to free up those SPtrs, good example is GameApp::update(REAL dt)
			{
				_sptrs.clear();
			}
		};

		template<class T>					SPtr<T>			RefBaseImpl::SPtr_from_this_dynamic() const			{return SPtr<T>(dynamic_cast<T*>(this));}
	};
};

