
#include "predecl.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)

#include "d3dIBO.h"

namespace z3D
{
	namespace Video
	{
		d3dIBO::d3dIBO() {_pointer = 0; _device = 0;}
		d3dIBO::~d3dIBO()
		{
			if(_gfx.valid())
				_gfx.get_unsafe()->removeIBORef(_handle);

			if(_handle)
				_handle->Release();
			if(_device)
				_device->Release();
		}
		SPtr<d3dIBO>		d3dIBO::create(const SPtr<d3dGfx>& gfx, size_t length, bool _32bit, bool dynamic)
		{
			D3DFORMAT fmt = (_32bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16);
			IDirect3DIndexBuffer9* handle = NULL;
			gfx->device()->CreateIndexBuffer((UINT)length, dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : 0, fmt, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &handle, NULL);
			if(!handle)
			{
				if(dynamic)
				{
					dynamic = false;
					gfx->device()->CreateIndexBuffer((UINT)length, dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : 0, fmt, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &handle, NULL);
					if(!handle)
						return NULL;
				}
				else
					return NULL;
			}
			SPtr<d3dIBO> ibo = new d3dIBO();
			ibo->_gfx = gfx;
			ibo->_handle = handle;
			ibo->_device = gfx->device();
			ibo->_device->AddRef();
			ibo->_32bit = _32bit;
			ibo->_dynamic = dynamic;
			ibo->_length = length;

			if(!checkManaged(handle))
				Gfx::current<d3dGfx>()->addRecreatable(ibo.get());

			return ibo;
		}
		bool					d3dIBO::checkManaged(IDirect3DIndexBuffer9* IBO)
		{
			D3DINDEXBUFFER_DESC desc;
			IBO->GetDesc(&desc);
			if(desc.Pool != D3DPOOL_MANAGED)
				return false;
			return true;
		}
		bool			d3dIBO::dynamic() const
		{
			return !!_dynamic;
		}
		bool			d3dIBO::is32bit() const
		{
			return !!_32bit;
		}
		bool			d3dIBO::lock()
		{
			return lock(0, _length, false);
		}
		bool			d3dIBO::lock(size_t offset, size_t size, bool no_overwrite)
		{
			Z_ASSERT(offset >= 0 && size > 0 && offset + size <= _length);

			bool ret = (_handle->Lock((UINT)offset, (UINT)size, &_pointer, dynamic() ? (no_overwrite ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD) : 0) == D3D_OK);
			Z_ASSERT( ret );
			return ret;
		}
		void			d3dIBO::unlock()
		{
			bool ret = (_handle->Unlock() == D3D_OK);
			_pointer = NULL;

			Z_ASSERT( ret);
		}
		void*			d3dIBO::pointer() const {return _pointer;}
		size_t			d3dIBO::length() const {return _length;}
		IDirect3DIndexBuffer9*		d3dIBO::handle() const {return _handle;}
		void			d3dIBO::prepare_recreate()
		{
			if(!_handle)
				return;
			_handle->Release();
			_handle = NULL;
		}
		bool			d3dIBO::recreate()
		{
			HRESULT ret = ((d3dGfx*)Gfx::current())->device()->CreateIndexBuffer((UINT)_length, _dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY): 0, _32bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16, _dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &_handle, NULL);
			Z_ASSERT( !FAILED(ret) );
			if(!_handle)
				return false;
			ver.reset();
			return true;
		}
		wcs				d3dIBO::short_desc() const
		{
			wcs desc = FORMATW(L"IBO: %d", _length);
			if(_dynamic)
				desc += L", dynamic";
			return desc;
		}
	};
};

#endif