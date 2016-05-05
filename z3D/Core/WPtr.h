
#pragma once

#include "predecl.h"
#include "shared_count.h"

#ifdef SPTR_THREAD_SAFE
	#include "mutex.h"
	#include "scoped_lock.h"
#endif

#include "SPtr.h"

/********************************************************************************************************************************************************************************************

The set of the following classes are thread-safe for both read and write simultaneously and hence they are safer than boost smart pointer implementation
RefBaseImpl features automatic correct construction of SPtr(e.g. SPtr<T>(this)) whenever you possess a RefBaseImpl pointer(such as this) which is similar in boost smart pointer implementation(shared from this)
However, it is much safer than the boost one, because the shared_count is stored in ResBase itself

For the best use of smart pointer classes, use SPtr everywhere as much as possible. Use WPtr to break cyclic dependency. Storing raw pointers(e.g. SPtr::get() or WPtr::get_unsafe()) should only happen in performance-critical code block
Because the following classes are much safer, the trade-off is a small performance hit in methods of SPtr and WPtr

***** To ensure thread-safety for operations of SPtr and WPtr, define SPTR_THREAD_SAFE in predecl.h, otherwise, remark it for single-threaded application *****

********************************************************************************************************************************************************************************************/

namespace z3D
{
	namespace Core
	{
		// An implementation of weak pointer
		template< class U >
		class WPtr
		{
		private:
			shared_count					_sc;
			U*								_ref;
		public:
			WPtr();
			WPtr(const WPtr& p);													// this is the real copy constructor, the next line one CANNOT, without this, memberwise copy is performed
			template<class V>				WPtr(const SPtr<V>& p);
			template<class V>				WPtr(const WPtr<V>& p);
		private:
			void							___ctor();
			template<class V> void			___ctor(const SPtr<V>& p);
			template<class V> void			___ctor(const WPtr<V>& p);
		public:
			WPtr&							operator=(const WPtr& p);				// this is the real assignment operator for same type
			template<class V> WPtr&			operator=(const WPtr<V>& p);
			template<class V> WPtr&			operator=(const SPtr<V>& p);
		public:
			void							reset();
		public:
			SPtr<U>							lock() const;
		public:
			uint32_t						use_count() const;
			bool							expired() const;
		public:
			bool							valid() const;
		public:
			U*								get_unsafe() const;
		public:
			friend bool	operator==(const WPtr& a, const WPtr& b)							{return a._sc == b._sc;}				// out of the scope of read/write thread-safety
			friend bool	operator!=(const WPtr& a, const WPtr& b)							{return a._sc != b._sc;}				// out of the scope of read/write thread-safety
			friend bool	operator<(const WPtr& a, const WPtr& b)								{return a._sc < b._sc;}					// out of the scope of read/write thread-safety
		public:
			template<class V> friend bool	operator==(const WPtr& a, const WPtr<V>& b)		{return a._sc == b._sc;}				// out of the scope of read/write thread-safety
			template<class V> friend bool	operator!=(const WPtr& a, const WPtr<V>& b)		{return a._sc != b._sc;}				// out of the scope of read/write thread-safety
			template<class V> friend bool	operator<(const WPtr& a, const WPtr<V>& b)		{return a._sc < b._sc;}					// out of the scope of read/write thread-safety
		};
	};
};

#include "WPtr_inl.h"

