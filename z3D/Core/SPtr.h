
#pragma once

#include "predecl.h"
#include "shared_count.h"

#ifdef SPTR_THREAD_SAFE
	#include "mutex.h"
	#include "scoped_lock.h"
#endif

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
		// An implementation of shared pointer, strong pointer, or whatever you like to call it
		template< class U >
		class SPtr
		{
		private:
			shared_count					_sc;
			U*								_ref;
		public:
			SPtr(U* ref = 0);
			SPtr(const SPtr& p);													// this is the real copy constructor, the next line one CANNOT, without this, memberwise copy is performed
			template<class V>				SPtr(const SPtr<V>& p);
		public:
			~SPtr();
		private:
			void							___ctor(U* ref);						// do not call the actual class constructor, because if there are instance objects, there will be reinitialized, so for safety, add a separate constructor, and so direct calling of any actual constructor/destructor is prohibited, except when using custom memory manager
			template<class V> void			___ctor(const SPtr<V>& p);
		private:
			void							___dtor(bool renew_sc);
		public:
			SPtr&							operator=(const SPtr& p);				// this is the real assignment operator for same type
			template<class V> SPtr&			operator=(const SPtr<V>& p);
		public:
			void							reset();
		public:
			U&								operator*() const;						// out of the scope of read/write thread-safety
			U*								operator->() const;						// out of the scope of read/write thread-safety
		public:
			U*								get() const;							// out of the scope of read/write thread-safety
			U*								get_unsafe() const;						// out of the scope of read/write thread-safety
		public:
			bool							unique() const;
			uint32_t						use_count() const;
		public:
			template<class V> SPtr<V>		cast() const;							// out of the scope of read/write thread-safety
			template<class V> SPtr<V>		cast_dynamic() const;					// out of the scope of read/write thread-safety // using dynamic_cast, if runtime type information is not enabled, DO NOT use this method. 
		public:
			operator						bool() const;							// out of the scope of read/write thread-safety
		public:
			friend bool	operator==(const SPtr& a, const SPtr& b)							{return a._sc == b._sc;}				// out of the scope of read/write thread-safety
			friend bool	operator!=(const SPtr& a, const SPtr& b)							{return a._sc != b._sc;}				// out of the scope of read/write thread-safety
			friend bool	operator<(const SPtr& a, const SPtr& b)								{return a._sc < b._sc;}					// out of the scope of read/write thread-safety
		public:
			template<class V> friend bool	operator==(const SPtr& a, const SPtr<V>& b)		{return a._sc == b._sc;}				// out of the scope of read/write thread-safety
			template<class V> friend bool	operator!=(const SPtr& a, const SPtr<V>& b)		{return a._sc != b._sc;}				// out of the scope of read/write thread-safety
			template<class V> friend bool	operator<(const SPtr& a, const SPtr<V>& b)		{return a._sc < b._sc;}					// out of the scope of read/write thread-safety
		};
	};
};

#include "SPtr_inl.h"

