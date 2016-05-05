
#include "Jotter.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		Jotter::Jotter()
		{
			_buffer = NULL;

			begin(3, false, false, 0);
		}
		Jotter::~Jotter()
		{
			if(_buffer != _pre_alloc)
				delete[] (char*)_buffer;
		}
		void						Jotter::begin(uint8_t pos_comps, bool has_normal, bool has_color, uint8_t tc_sets, uint8_t tc_comps)
		{
			_pos_comps = min(max((uint8_t)2, pos_comps), (uint8_t)4);
			_has_normal = has_normal;
			_has_color = has_color;
			_tc_sets = min(max((uint8_t)0, tc_sets), (uint8_t)MAX_TEXCOORD_SETS);
			_tc_comps = min(max((uint8_t)1, tc_comps), (uint8_t)4);

			_vert_size = 0;
			_vert_size += (_pos_size = sizeof(REAL) * _pos_comps);
			_vert_size += sizeof(REAL) * 3 * _has_normal;
			_vert_size += sizeof(REAL) * 4 * _has_color;
			_vert_size += (_tc_size = sizeof(REAL) * _tc_comps) * _tc_sets;

			if(!_buffer)
			{
				_buffer = _pre_alloc;
				_buffer_size = sizeof(_pre_alloc);
			}
			_max_vertices = _buffer_size / _vert_size;
			_vertices = 0;
			_next = _buffer;
		}
		void						Jotter::vertex(const Vec2& pos)
		{
			*(Vec2*)&_vert_pos = pos;
			__commit_vertex();
		}
		void						Jotter::vertex(const Vec3& pos)
		{
			*(Vec3*)&_vert_pos = pos;
			__commit_vertex();
		}
		void						Jotter::vertex(const Vec4& pos)
		{
			_vert_pos = pos;
			__commit_vertex();
		}
		void						Jotter::vertex(REAL x, REAL y)
		{
			_vert_pos.x = x;
			_vert_pos.y = y;
			__commit_vertex();
		}
		void						Jotter::vertex(REAL x, REAL y, REAL z)
		{
			_vert_pos.x = x;
			_vert_pos.y = y;
			_vert_pos.z = z;
			__commit_vertex();
		}
		void						Jotter::vertex(REAL x, REAL y, REAL z, REAL w)
		{
			_vert_pos.x = x;
			_vert_pos.y = y;
			_vert_pos.z = z;
			_vert_pos.w = w;
			__commit_vertex();
		}
		void						Jotter::normal(const Vec3& nor)
		{
			_vert_nor = nor;
		}
		void						Jotter::normal(REAL nx, REAL ny, REAL nz)
		{
			_vert_nor.x = nx;
			_vert_nor.y = ny;
			_vert_nor.z = nz;
		}
		void						Jotter::color(const Vec4& color)
		{
			_vert_color = color;
		}
		void						Jotter::color(REAL r, REAL g, REAL b, REAL a)
		{
			_vert_color.x = r;
			_vert_color.y = g;
			_vert_color.z = b;
			_vert_color.w = a;
		}
		void						Jotter::tc(uint8_t tc_set, const REAL tc)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			_vert_tc[tc_set].x = tc;
		}
		void						Jotter::tc(uint8_t tc_set, const Vec2& tc)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			*(Vec2*)&_vert_tc[tc_set] = tc;
		}
		void						Jotter::tc(uint8_t tc_set, const Vec3& tc)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			*(Vec3*)&_vert_tc[tc_set] = tc;
		}
		void						Jotter::tc(uint8_t tc_set, const Vec4& tc)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			_vert_tc[tc_set] = tc;
		}
		void						Jotter::tc(uint8_t tc_set, REAL s, REAL t)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			REAL* p = &_vert_tc[tc_set].x;
			p[0] = s;
			p[1] = t;
		}
		void						Jotter::tc(uint8_t tc_set, REAL s, REAL t, REAL r)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			REAL* p = &_vert_tc[tc_set].x;
			p[0] = s;
			p[1] = t;
			p[2] = r;
		}
		void						Jotter::tc(uint8_t tc_set, REAL s, REAL t, REAL r, REAL q)
		{
			Z_ASSERT(tc_set < MAX_TEXCOORD_SETS);
			REAL* p = &_vert_tc[tc_set].x;
			p[0] = s;
			p[1] = t;
			p[2] = r;
			p[3] = q;
		}
		uint8_t						Jotter::pos_comps() const
		{
			return _pos_comps;
		}
		uint8_t						Jotter::has_normal() const
		{
			return _has_normal;
		}
		uint8_t						Jotter::has_color() const
		{
			return _has_color;
		}
		uint8_t						Jotter::texcoord_sets() const
		{
			return _tc_sets;
		}
		uint8_t						Jotter::texcoord_comps() const
		{
			return _tc_comps;
		}
		void*						Jotter::buffer() const
		{
			return _buffer;
		}
		uint32_t					Jotter::vertices() const
		{
			return _vertices;
		}
		uint32_t					Jotter::vertex_size() const
		{
			return _vert_size;
		}
		void						Jotter::__commit_vertex()
		{
			if(_vertices >= _max_vertices)
			{
				bool need_free = (_buffer != _pre_alloc);

				void* nb = new char[_buffer_size << 1];
				if(!nb)
					return;
				memcpy(nb, _buffer, _buffer_size);
				if(need_free)
					delete[] (char*)_buffer;
				_buffer = nb;
				_buffer_size = _buffer_size << 1;
				_max_vertices = _buffer_size / _vert_size;
				_next = (char*)_buffer + _vert_size * _vertices;
			}

			char* p = (char*)_next;
			memcpy(p, &_vert_pos, _pos_size);
			p += _pos_size;
			if(_has_normal)
			{
				memcpy(p, &_vert_nor, sizeof(Vec3));
				p += sizeof(Vec3);
			}
			if(_has_color)
			{
				memcpy(p, &_vert_color, sizeof(Vec4));
				p += sizeof(Vec4);
			}
			ptrdiff_t c = _tc_sets;
			for(ptrdiff_t i = 0; i < c; ++i)
			{
				memcpy(p, &_vert_tc[i], _tc_size);
				p += _tc_size;
			}
			_next = p;
			++_vertices;
		}
		void						Jotter::bindVertexAttribs(const SPtr<Gfx>& gfx) const
		{
			Gfx* g = gfx.get();
			size_t s = 0;
			g->bindVertexPosition((uint8_t)s, (uint8_t)_pos_comps);
			s += _pos_size;
			if(_has_normal)
			{
				g->bindVertexNormal((uint8_t)s);
				s += sizeof(Vec3);
			}
			if(_has_color)
			{
				g->bindVertexColor((uint8_t)s);
				s += sizeof(Vec4);
			}
			ptrdiff_t c = _tc_sets;
			for(ptrdiff_t i = 0; i < c; ++i)
			{
				g->bindVertexTexCoord((uint8_t)s, (uint8_t)i, (uint8_t)_tc_comps);
				s += _tc_size;
			}
		}
	}
}

