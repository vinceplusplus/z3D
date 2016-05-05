
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class cgBillBoardFX: public SNode
		{
			DECL_TYPEINFO();
		public:
			class Image: public RefBase
			{
			public:
				enum
				{
					MAX_FRAMES	=	64,											// i don't think u need more than 64 frames
				};
			private:
				SPtr<Tex2D>					_tex;
				size_t						_frames;
				struct						FRAME
				{
					REAL						lt[2];
					REAL						rb[2];
				}							_array_frame[MAX_FRAMES];
			protected:
				Image();
				~Image();
			public:
				friend class cgBillBoardFX;
			};
		public:
			class Item: public RefBase
			{
			public:
				typedef enum _ALIGN : size_t
				{
					ALIGN_SCREEN = 0,
					ALIGN_WORLD,
					//ALIGN_AXIS,
				} ALIGN;
				typedef enum _BLEND : size_t
				{
					BLEND_ADDITIVE = 0,
					BLEND_DECAL,
					BLEND_MAX,
				} BLEND;
			public:
				Vec3						pos;
				Vec4						color;
				Vec2						extent;
			public:
				SPtr<Image>					image;
				REAL						anim;
			public:
				ALIGN						align;
				BLEND						blend;
			private:
				REAL						_temp;
			public:
				Item();
			protected:
				~Item();
			public:
				friend class cgBillBoardFX;
			};
		private:
			AABB						_local_bound;
		private:
			SPtr<Shader>				_vs_screen;
			SPtr<Shader>				_vs_world;
			SPtr<Shader>				_ps;
			SPtr<Shader>				_ps_max;
		private:
			vector<WPtr<Item>>			_array_item;
		public:
			cgBillBoardFX(const AABB& local_bound);
		protected:
			~cgBillBoardFX();
		public:
			void						addBillBoard(const SPtr<Item>& item);
			SPtr<cgBillBoardFX::Image>	createImage(const wcs& filepath, size_t rows, size_t cols, size_t frames);
		public:
			bool						loadShaders();
		public:
			const AABB&					local_bound();
		public:
			bool						renderable() const;
		public:
			void						preRender(SRenderInfo* ri);
		public:
			void						render(intptr_t parameter, SRenderInfo* ri);
		};
	};
};


