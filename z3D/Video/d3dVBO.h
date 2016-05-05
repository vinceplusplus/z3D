
#pragma once

#include "predecl.h"
#include "VBO.h"
#include "d3dGfx.h"

namespace z3D
{
	namespace Video
	{
		class d3dVBO: public VBO, public id3dRecreatable
		{
		public:
			DECL_TYPEINFO();
		protected:
			WPtr<d3dGfx>			_gfx;
			IDirect3DDevice9*		_device;
			IDirect3DVertexBuffer9*	_handle;
			size_t					_dynamic;
		protected:
			size_t					_length;
			void*					_pointer;
		protected:
			d3dVBO();
			~d3dVBO();
		public:
			static SPtr<d3dVBO>		create(const SPtr<d3dGfx>& gfx, size_t length, bool dynamic);
		public:
			static bool		checkManaged(IDirect3DVertexBuffer9* vbo);
		public:
			bool			dynamic() const;
		public:
			bool			lock();
			bool			lock(size_t offset, size_t size, bool no_overwrite);
			void			unlock();
		public:
			void*			pointer() const;
			size_t			length() const;
		public:
			IDirect3DVertexBuffer9*		handle() const;
		protected:
			void			prepare_recreate();
			bool			recreate();
		public:
			wcs				short_desc() const;
		};

	};
};

