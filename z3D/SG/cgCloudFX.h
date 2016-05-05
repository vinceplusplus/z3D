
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		// should we use the name "cgCloudFX"?
		class cgCloudFX: public SNode
		{
			DECL_TYPEINFO();
		private:
			AABB				_local_bound;
		private:
			SPtr<Tex2D>			_array_octave[4];
		private:
			SPtr<VBO>			_vbo;
			SPtr<IBO>			_ibo;
		private:
			SPtr<Shader>		_vs;
			SPtr<Shader>		_ps;
		private:
			Counter				_counter;
		protected:
			cgCloudFX();
			~cgCloudFX();
		private:
			template<size_t M, size_t N>
			static void					catmull_rom_rescale(const REAL (&src)[M][M], REAL (&dst)[N][N]);
		private:
			bool						init();
		public:
			static SPtr<cgCloudFX>		create();
		private:
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


