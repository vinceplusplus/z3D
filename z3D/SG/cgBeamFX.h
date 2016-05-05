
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class cgBeamFX: public SNode
		{
			DECL_TYPEINFO();
		public:
			class Item: public RefBase
			{
			public:
				Vec3					from;
				Vec3					to;
				Vec4					color;
				REAL					radius;
			};
		private:
			AABB					_local_bound;
		private:
			SPtr<Tex2D>				_tex_beam;
			SPtr<Tex2D>				_tex_beam_core;
		private:
			SPtr<Shader>			_vs;
			SPtr<Shader>			_ps;
		private:
			vector<WPtr<Item>>		_array_item;
		public:
			cgBeamFX(const AABB& local_bound);
		protected:
			~cgBeamFX();
		public:
			void					setBeamTextures(const SPtr<Tex2D>& beam, const SPtr<Tex2D>& beam_core);
			bool					loadShaders();
		public:
			SPtr<cgBeamFX::Item>	createBeam(const Vec3& from, const Vec3& to, const Vec4& color, REAL radius);
		public:
			const AABB&				local_bound();
		public:
			bool					renderable() const;
		public:
			void					preRender(SRenderInfo* ri);
		public:
			void					render(intptr_t parameter, SRenderInfo* ri);
		};
	};
};


