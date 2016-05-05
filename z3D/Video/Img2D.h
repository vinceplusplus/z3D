
#pragma once

#include "predecl.h"
#include "Tex2D.h"

namespace z3D
{
	namespace Video
	{
		// one day, i will convert all these ints to int32_ts

		class Img2D: public ResBase
		{
		protected:
			static ResMgr				_res_mgr;
		private:
			int							_dx;
			int							_dy;
			int							_horz_slots;
			int							_vert_slots;
			int							_texture_size;
			vector< SPtr<Tex2D> >		_tex;
			//SPtr<TexSampler>			_sampler;
		private:				// authoring packing
			Config*						_packed_cfg;
			hash_map<wcs,int>			_packed_img_index;
		private:				// using other Img2D packing
			SPtr<Img2D>					_packed;
			int							_offset_x;
			int							_offset_y;
			int							_clip_x;
			int							_clip_y;
			int							_clip_dx;
			int							_clip_dy;
		private:				// global Img2D lookup/directory
			static hash_map<wcs, SPtr<Img2D> >		_packed_img_lookup;
		protected:
			static bool					FORCE_16BIT;
		protected:
			static SPtr<Tex2D>			_null_tex2d;
		private:
			Img2D(const wcs& filepath);
			~Img2D();
		public:
			static void					setForce16Bit(bool force);
			static bool					getForce16Bit();
		public:
			static SPtr<Img2D>			load(const wcs& filepath, int texture_size = 256);
			static SPtr<Img2D>			load_fip(void* fipimage, int texture_size = 256);
			static SPtr<Img2D>			create(int dx, int dy, int texture_size = 256);
		public:
			int							dx() const;
			int							dy() const;
			int							horz_slots() const;
			int							vert_slots() const;
			int							texture_size() const;
			const SPtr<Tex2D>&			texture(int r,int c) const;				// use reference to avoid unnecessary copy
		public:
			void						draw(int dest_x, int dest_y, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* shade_state = NULL) const;
			void						draw(int src_x, int src_y, int dx, int dy, int dest_x, int dest_y, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* shade_state = NULL) const;
			void						drawStretch(int src_x, int src_y, int src_dx, int src_dy, int dest_x, int dest_y, int dest_dx, int dest_dy, const Vec4& color = Vec4(1, 1, 1, 1), rsShade* shade_state = NULL) const;
		public:
			//static void					begin();
			//static void					end();
		public:
			void						author_packing(const Config& cfg);
		public:
			int							offset_x() const;
			int							offset_y() const;
			int							clip_x() const;
			int							clip_y() const;
			int							clip_dx() const;
			int							clip_dy() const;
		public:
			void						getRespectingShadeState(rsShade& rs, bool reset_shade_state) const;
		};
	};
};

