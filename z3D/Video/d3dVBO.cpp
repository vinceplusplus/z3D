
#include "predecl.h"

#if defined(Z3D_ENABLE_VIDEO_D3D)

#include "d3dVBO.h"

namespace z3D
{
	namespace Video
	{
		d3dVBO::d3dVBO() {_pointer = 0; _device = 0;}
		d3dVBO::~d3dVBO()
		{
			if(_gfx.valid())
				_gfx.get_unsafe()->removeVBORef(_handle);
			if(_handle)
				_handle->Release();
			if(_device)
				_device->Release();
		}
		SPtr<d3dVBO>		d3dVBO::create(const SPtr<d3dGfx>& gfx, size_t length, bool dynamic)
		{
			IDirect3DVertexBuffer9* handle = NULL;
			gfx->device()->CreateVertexBuffer((UINT)length, dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : D3DUSAGE_WRITEONLY, 0, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &handle, NULL);
			if(!handle)
			{
				if(dynamic)
				{
					dynamic = false;
					gfx->device()->CreateVertexBuffer((UINT)length, dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) : D3DUSAGE_WRITEONLY, 0, dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &handle, NULL);
					if(!handle)
						return NULL;
				}
				else
					return NULL;
			}
			SPtr<d3dVBO> vbo = new d3dVBO();
			vbo->_gfx = gfx;
			vbo->_handle = handle;
			vbo->_device = gfx->device();
			vbo->_device->AddRef();
			vbo->_dynamic = dynamic;
			vbo->_length = length;

			if(!checkManaged(handle))
				Gfx::current<d3dGfx>()->addRecreatable(vbo.get());

			return vbo;
		}
		bool					d3dVBO::checkManaged(IDirect3DVertexBuffer9* vbo)
		{
			D3DVERTEXBUFFER_DESC desc;
			vbo->GetDesc(&desc);
			if(desc.Pool != D3DPOOL_MANAGED)
				return false;
			return true;
		}
		bool			d3dVBO::dynamic() const
		{
			return !!_dynamic;
		}
		bool			d3dVBO::lock()
		{
			return lock(0, _length, false);
		}
		bool			d3dVBO::lock(size_t offset, size_t size, bool no_overwrite)
		{
			Z_ASSERT(offset >= 0 && size > 0 && offset + size <= _length);

			bool ret = (_handle->Lock((UINT)offset, (UINT)size, &_pointer, dynamic() ? (no_overwrite ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD) : 0) == D3D_OK);
			Z_ASSERT( ret );
			return ret;
		}
		void			d3dVBO::unlock()
		{
			bool ret = (_handle->Unlock() == D3D_OK);
			_pointer = NULL;

			Z_ASSERT( ret);
		}
		void*			d3dVBO::pointer() const {return _pointer;}
		size_t			d3dVBO::length() const {return _length;}
		IDirect3DVertexBuffer9*		d3dVBO::handle() const {return _handle;}
		void			d3dVBO::prepare_recreate()
		{
			if(!_handle)
				return;
			_handle->Release();
			_handle = NULL;
		}
		bool			d3dVBO::recreate()
		{
			HRESULT ret = ((d3dGfx*)Gfx::current())->device()->CreateVertexBuffer((UINT)_length, _dynamic ? (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY): 0, 0, _dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &_handle, NULL);
			Z_ASSERT( !FAILED(ret) );
			if(!_handle)
				return false;
			ver.reset();
			return true;
		}
		wcs				d3dVBO::short_desc() const
		{
			wcs desc = FORMATW(L"VBO: %d", _length);
			if(_dynamic)
				desc += L", dynamic";
			return desc;
		}
	};
};

#endif