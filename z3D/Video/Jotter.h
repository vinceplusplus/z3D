
#pragma once

#include "predecl.h"
#include "GfxEnum.h"

namespace z3D
{
	namespace Video
	{
		class Jotter
		{
		private:
			enum
			{
				PRE_ALLOC_SIZE = 2048,
				MAX_TEXCOORD_SETS = 16,
			};
		private:
			uint8_t						_pre_alloc[PRE_ALLOC_SIZE];
		private:
			union
			{
				uint32_t																				__UNNUSED_00;
				struct
				{
					uint8_t						_pos_comps : 3;
					uint8_t						_has_normal : 1;
					uint8_t						_has_color : 1;
					uint8_t						_tc_sets : 4;
					uint8_t						_tc_comps : 3;
				};
			};
			uint32_t					_vert_size;
			uint32_t					_pos_size;
			uint32_t					_tc_size;
		private:
			void*						_buffer;
			uint32_t					_buffer_size;
			uint32_t					_max_vertices;
			uint32_t					_vertices;
			void*						_next;
		private:
			Vec4						_vert_pos;
			Vec3						_vert_nor;
			Vec4						_vert_color;
			Vec4						_vert_tc[MAX_TEXCOORD_SETS];
		public:
			Jotter();
			~Jotter();
		public:
			void						begin(uint8_t pos_comps, bool has_normal, bool has_color, uint8_t tc_sets, uint8_t tc_comps = 2);
		public:
			void						vertex(const Vec2& pos);
			void						vertex(const Vec3& pos);
			void						vertex(const Vec4& pos);
			void						vertex(REAL x, REAL y);
			void						vertex(REAL x, REAL y, REAL z);
			void						vertex(REAL x, REAL y, REAL z, REAL w);
		public:
			void						normal(const Vec3& nor);
			void						normal(REAL nx, REAL ny, REAL nz);
		public:
			void						color(const Vec4& color);
			void						color(REAL r, REAL g, REAL b, REAL a);
		public:
			void						tc(uint8_t tc_set, const REAL tc);
			void						tc(uint8_t tc_set, const Vec2& tc);
			void						tc(uint8_t tc_set, const Vec3& tc);
			void						tc(uint8_t tc_set, const Vec4& tc);
			void						tc(uint8_t tc_set, REAL s, REAL t);
			void						tc(uint8_t tc_set, REAL s, REAL t, REAL r);
			void						tc(uint8_t tc_set, REAL s, REAL t, REAL r, REAL q);
		public:
			uint8_t						pos_comps() const;
			uint8_t						has_normal() const;
			uint8_t						has_color() const;
			uint8_t						texcoord_sets() const;
			uint8_t						texcoord_comps() const;
		public:
			void*						buffer() const;
			uint32_t					vertices() const;
			uint32_t					vertex_size() const;
		private:
			void						__commit_vertex();
		public:
			void						bindVertexAttribs(const SPtr<Gfx>& gfx) const;
		};
	};
};

