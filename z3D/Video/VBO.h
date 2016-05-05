
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Video
	{
		class VBO: public RefBase
		{
		public:
			DECL_TYPEINFO();
		public:
			version					ver;
		protected:
			VBO();
			~VBO();
		public:
			static SPtr<VBO>		create(size_t length, bool dynamic);							// dynamic VBO will likely be placed in AGP memory...
		public:
			virtual bool			dynamic() const = 0;
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

