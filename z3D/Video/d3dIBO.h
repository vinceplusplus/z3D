
#pragma once

#include "predecl.h"
#include "IBO.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		class d3dIBO: public IBO, public id3dRecreatable
		{
		public:
			DECL_TYPEINFO();
		protected:
			WPtr<d3dGfx>			_gfx;
			IDirect3DDevice9*		_device;
			IDirect3DIndexBuffer9*	_handle;
			size_t					_32bit;
			size_t					_dynamic;
		protected:
			size_t					_length;
			void*					_pointer;
		protected:
			d3dIBO();
			~d3dIBO();
		public:
			static SPtr<d3dIBO>		create(const SPtr<d3dGfx>& gfx, size_t length, bool _32bit, bool dynamic);
		public:
			static bool		checkManaged(IDirect3DIndexBuffer9* IBO);
		public:
			bool			dynamic() const;
			bool			is32bit() const;
		public:
			bool			lock();
			bool			lock(size_t offset, size_t size, bool no_overwrite);
			void			unlock();
		public:
			void*			pointer() const;
			size_t			length() const;
		public:
			IDirect3DIndexBuffer9*		handle() const;
		protected:
			void			prepare_recreate();
			bool			recreate();
		public:
			wcs				short_desc() const;
		};

	};
};

