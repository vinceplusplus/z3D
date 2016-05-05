
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class IBO: public RefBase
		{
		public:
			DECL_TYPEINFO();
		public:
			version					ver;
		protected:
			size_t					_dirty;
		protected:
			IBO();
			~IBO();
		public:
			static SPtr<IBO>		create(size_t length, bool _32bit, bool dynamic);							// dynamic IBO will likely be placed in AGP memory...
		public:
			void					setDirty(bool dirty);
			bool					dirty() const;
		public:
			virtual bool			dynamic() const = 0;
			virtual bool			is32bit() const = 0;
		public:
			virtual bool			lock() = 0;													// lock entire buffer
			virtual bool			lock(size_t offset, size_t size, bool no_overwrite = false) = 0;
			virtual void			unlock() = 0;
		public:
			virtual void*			pointer() const = 0;
			template<class T> T*	data() const {return (T*)pointer();}
			virtual size_t			length() const = 0;
		};
	};
};

