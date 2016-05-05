
#include "cgCloudFX.h"
#include "RenderInfo.h"

namespace z3D
{
	namespace SG
	{
		cgCloudFX::cgCloudFX()
		{
		}
		cgCloudFX::~cgCloudFX()
		{
		}
		template<size_t M, size_t N>
		void						cgCloudFX::catmull_rom_rescale(const REAL (&src)[M][M], REAL (&dst)[N][N])
		{
			Z_STATIC_ASSERT( M > 1, ASSUMPTION_WRONG );
			Z_STATIC_ASSERT( M <= 64, SRC_TOO_LARGE );

			REAL HERMITE[4][4] = {{2, -2, 1, 1}, {-3, 3, -2, -1}, {0, 0, 1, 0}, {1, 0, 0, 0}};
			REAL HERMITE_T[4][4] = {{2, -3, 0, 1}, {-2, 3, 0, 0}, {1, -2, 1, 0}, {1, -1, 0, 0}};

			REAL array_g[M][M][4][4];
			size_t array_init_g[M][M];

			memset(array_init_g, 0, sizeof(array_init_g));

			for(size_t j = 0; j < N; ++j)
				for(size_t i = 0; i < N; ++i)
				{
					REAL u = (REAL)i / N * M;
					REAL v = (REAL)j / N * M;
					size_t c = (size_t)u;
					size_t r = (size_t)v;
					u -= c;
					v -= r;

					REAL (&g)[4][4] = array_g[r][c];
					if(!array_init_g[r][c])
					{
						size_t r0 = (r + M - 1) % M;
						size_t c0 = (c + M - 1) % M;
						size_t r1 = r;
						size_t c1 = c;
						size_t r2 = (r + 1) % M;
						size_t c2 = (c + 1) % M;
						size_t r3 = (r + 2) % M;
						size_t c3 = (c + 2) % M;

						REAL tmp[4][4] = {
							{src[r1][c1], src[r2][c1], (src[r2][c1] - src[r0][c1]) * 0.5f, (src[r3][c1] - src[r1][c1]) * 0.5f},
							{src[r1][c2], src[r2][c2], (src[r2][c2] - src[r0][c2]) * 0.5f, (src[r3][c2] - src[r1][c2]) * 0.5f},
							{(src[r1][c2] - src[r1][c0]) * 0.5f, (src[r2][c2] - src[r2][c0]) * 0.5f, (src[r1][c2] - src[r1][c1]) - (src[r1][c1] - src[r1][c0]), (src[r2][c2] - src[r2][c1]) - (src[r2][c1] - src[r2][c0])},
							{(src[r1][c3] - src[r1][c1]) * 0.5f, (src[r2][c3] - src[r2][c1]) * 0.5f, (src[r1][c3] - src[r1][c2]) - (src[r1][c2] - src[r1][c1]), (src[r2][c3] - src[r2][c2]) - (src[r2][c2] - src[r2][c1])}
						};

						memcpy(g, tmp, sizeof(g));
						array_init_g[r][c] = true;
					}

					Mat4 m = (*(Mat4*)HERMITE) * (*(Mat4*)g) * (*(Mat4*)HERMITE_T);
					Vec4 s = Vec4(u * u * u, u * u, u, 1) * m;
					REAL val = s[0] * v * v * v + s[1] * v * v + s[2] * v + s[3];

					Z_ASSERT( FINITE(val) );

					dst[j][i] = val;
				}
		}
		bool						cgCloudFX::init()
		{
			REAL r = 1 / SQRT(2.0f);
			_local_bound = AABB(Vec3(-r, -r, -r), Vec3(r, 1 - r, r));

			struct _VERTEX
			{
				Vec3				pos;
				Vec2				uv;
			}					array_vert[16][16];

			struct _INDEX
			{
				uint16_t			idx[6];
			}					array_idx[15][15];

			Z_STATIC_ASSERT( sizeof(_VERTEX) == sizeof(Vec3) + sizeof(Vec2), ASSUMPTION_WRONG1 );
			Z_STATIC_ASSERT( sizeof(_INDEX) == sizeof(uint16_t) * 6, ASSUMPTION_WRONG2 );

			Z_STATIC_ASSERT( _countof(array_vert) - 1 == _countof(array_idx), ASSUMPTION_WRONG3 );
			Z_STATIC_ASSERT( _countof(array_vert[0]) - 1 == _countof(array_idx[0]), ASSUMPTION_WRONG4 );

			for(size_t j = 0; j < _countof(array_vert); ++j)
				for(size_t i = 0; i < _countof(array_vert[0]); ++i)
				{
					REAL u = i / ((_countof(array_vert[0]) - 1) * 0.5f) - 1;
					REAL v = j / ((_countof(array_vert) - 1) * 0.5f) - 1;

					u *= r;
					v *= r;

					REAL w = SQRT(max(0.0f, 1 - (u * u + v * v))) - r - (1 - r) * 0.5f;

					array_vert[j][i].pos = Vec3(u, w, v);
					array_vert[j][i].uv = Vec2((REAL)i / (_countof(array_vert[0]) - 1), (REAL)j / (_countof(array_vert) - 1));
				}
			for(size_t j = 0; j < _countof(array_idx); ++j)
				for(size_t i = 0; i < _countof(array_idx[0]); ++i)
				{
					array_idx[j][i].idx[0] = (uint16_t)((j + 1) * _countof(array_vert[0]) + i);
					array_idx[j][i].idx[1] = (uint16_t)(j * _countof(array_vert[0]) + i);
					array_idx[j][i].idx[2] = (uint16_t)(j * _countof(array_vert[0]) + i + 1);
					array_idx[j][i].idx[3] = (uint16_t)((j + 1) * _countof(array_vert[0]) + i);
					array_idx[j][i].idx[4] = (uint16_t)(j * _countof(array_vert[0]) + i + 1);
					array_idx[j][i].idx[5] = (uint16_t)((j + 1) * _countof(array_vert[0]) + i + 1);
				}

			_vbo = VBO::create(sizeof(array_vert), false);
			if(!_vbo)
				return false;

			_vbo->lock();
			memcpy(_vbo->pointer(), array_vert, sizeof(array_vert));
			_vbo->unlock();

			_ibo = IBO::create(sizeof(array_idx), false, false);
			if(!_ibo)
				return false;

			_ibo->lock();
			memcpy(_ibo->pointer(), array_idx, sizeof(array_idx));
			_ibo->unlock();

			if(true)
			{
				//lrng r(GetTickCount());
				mt r;
				r.init_genrand(0);
				//r.init_genrand(0);

				REAL ROUGHNESS = 1.0f;

				for(size_t i = 0; i < 4; ++i)
				{
					if(_array_octave[i])
						continue;

					REAL tmp[32][32];
					const size_t DIM = 256;
					REAL *buffer = new REAL[DIM * DIM];
					//REAL tmp2[256][256];
					REAL (&tmp2)[DIM][DIM] = (REAL (&)[DIM][DIM])*buffer;

					REAL scale = POW(POW(2.0f, -ROUGHNESS), (REAL)i);

					for(size_t j = 0; j < _countof(tmp); ++j)
						for(size_t k = 0; k < _countof(tmp[0]); ++k)
						{
							//REAL rr = (REAL)r.nextDouble();
							REAL rr = (REAL)r.genrand_real2();

							rr = rr * 2 - 1;
							tmp[j][k] = rr * scale;
						}

					catmull_rom_rescale(tmp, tmp2);

					_array_octave[i] = Tex2D::create(_countof(tmp2[0]), _countof(tmp2), 1, GfxEnum::PIXELFORMAT_L8, false);
					if(!_array_octave[i])
						return false;

					uint8_t* pixel;
					size_t pitch;
					_array_octave[i]->lockMipLevel(0, (void*&)pixel, pitch);

					for(size_t j = 0; j < _countof(tmp2); ++j)
					{
						for(size_t k = 0; k < _countof(tmp2[0]); ++k)
							pixel[k] = (uint8_t)min(max((ptrdiff_t)0, (ptrdiff_t)((tmp2[j][k] * 0.5f + 0.5f) * 256)), (ptrdiff_t)255);
						pixel += pitch;
					}

					_array_octave[i]->unlockMipLevel(0);

					delete buffer;
				}
			}

			return loadShaders();
		}
		SPtr<cgCloudFX>			cgCloudFX::create()
		{
			SPtr<cgCloudFX> n = new cgCloudFX();
			if(!n->init())
				return NULL;
			return n;
		}
		bool						cgCloudFX::loadShaders()
		{
			_vs = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_VERTEX, 
"\
\n\
float4x4 wvp;\n\
float4x4 xform_tc_0;\n\
float4x4 xform_tc_1;\n\
float4x4 xform_tc_2;\n\
float4x4 xform_tc_3;\n\
\n\
void main(\n\
	float4 in_pos: POSITION,\n\
	out float4 out_pos: POSITION,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	out float4 out_tc_0: TEXCOORD0,\n\
	out float4 out_tc_1: TEXCOORD1,\n\
	out float4 out_tc_2: TEXCOORD2,\n\
	out float4 out_tc_3: TEXCOORD3,\n\
	out float3 out_opos: TEXCOORD4,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	out_pos = mul(in_pos, wvp);\n\
	float r = sqrt(2.0) * 0.5;\n\
	out_opos = in_pos.xyz + float3(0, r + (1 - r) * 0.5, 0);\n\
	out_tc_0 = mul(in_tc_0, xform_tc_0);\n\
	out_tc_1 = mul(in_tc_0, xform_tc_1);\n\
	out_tc_2 = mul(in_tc_0, xform_tc_2);\n\
	out_tc_3 = mul(in_tc_0, xform_tc_3);\n\
}\n\
\
");

			_ps = Gfx::current()->createShaderFromSource(GfxEnum::SHADERTYPE_PIXEL, 
"\
\n\
sampler2D sampler_0: register(s0);\n\
sampler2D sampler_1: register(s1);\n\
sampler2D sampler_2: register(s2);\n\
sampler2D sampler_3: register(s3);\n\
\n\
void main(\n\
	out float4 out_color: COLOR,\n\
	float4 in_tc_0: TEXCOORD0,\n\
	float4 in_tc_1: TEXCOORD1,\n\
	float4 in_tc_2: TEXCOORD2,\n\
	float4 in_tc_3: TEXCOORD3,\n\
	float3 in_opos: TEXCOORD4,\n\
	uniform float4 END_PARAMS = 0\n\
	)\n\
{\n\
	float4 tf_0_0 = tex2D(sampler_0, in_tc_0);\n\
	float4 tf_1_1 = tex2D(sampler_1, in_tc_1);\n\
	float4 tf_2_2 = tex2D(sampler_2, in_tc_2);\n\
	float4 tf_3_3 = tex2D(sampler_3, in_tc_3);\n\
\n\
\n\
	float v = (((tf_0_0.x * 2 - 1) + (tf_1_1.x * 2 - 1) + (tf_2_2.x * 2 - 1) + (tf_3_3.x * 2 - 1)) * 0.5 + 0.5);\n\
	float v2 = v;\n\
	//v = 1 - pow(0.25, max(0, v * 4 - 2));\n\
	v = pow(saturate(v - 0.5), 0.666666);\n\
	//v = saturate(v - 0.5) * 2;\n\
	//v = smoothstep(0, 1, v);\n\
	//v = smoothstep(0, 1, v);\n\
	//v = saturate(v - 0.25);\n\
	float3 c = lerp(float3(0.5, 0.5, 1), float3(1, 1, 1), v);\n\
	//float3 c = lerp(float3(0.5, 0.5, 1), smoothstep(float3(2, 2, 2), float3(0.25, 0.25, 0.25), v2), v);\n\
	out_color = float4(lerp(c, float3(16.0 / 25, 16.0 / 25, 16.0 / 25), smoothstep(radians(22.5), radians(45.0 * 36 / 49), acos(saturate(dot(in_opos, float3(0, 1, 0)))))), 1);\n\
	//out_color = tf_0_0;\n\
\n\
}\n\
\
");
			return (_vs && _ps);
		}
		const AABB&					cgCloudFX::local_bound()
		{
			return _local_bound;
		}
		bool						cgCloudFX::renderable() const
		{
			return true;
		}
		void						cgCloudFX::preRender(SRenderInfo* ri)
		{
			ri->regSolidItem(this, 0);
		}
		void						cgCloudFX::render(intptr_t parameter, SRenderInfo* ri)
		{
			_counter.update();

			Gfx* gfx = Gfx::current();

			rsShade rs(false);

			SRenderParamQuery param(ri);
			param.preSetupShadeState(rs);

			Z_ASSERT( _vs && _ps );

			gfx->useProgrammablePipeline(_vs, _ps);

			ShaderUniforms2 unis(_vs, _ps);

			//unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", world_trans() * gfx->getViewTransform() * gfx->adjustProjectionTransform(gfx->getUnadjustedProjectionTransform()));
			unis.set(GfxEnum::SHADERTYPE_VERTEX, "wvp", world_trans() * gfx->getAdjustedViewProjectionTransform());
			for(size_t i = 0; i < 4; ++i)
			{
				rs.setSampler(i);
				REAL t = -(_counter.elapsedTime() - ((int)_counter.elapsedTime()) / 16 * 16) * (1.0f / 8);
				unis.set(GfxEnum::SHADERTYPE_VERTEX, FORMATA("xform_tc_%d", i).c_str(), Mat4::translation(t / (1 << i), t / (1 << i), 0) * Mat4::scaling((REAL)(1 << i), (REAL)(1 << i), (REAL)(1 << i)));
			}

			gfx->setShaderUniforms(unis);

			param.postSetupShadeState(rs);

			gfx->setShadeState(rs);

			for(size_t i = 0; i < 4; ++i)
				gfx->bindTexture(i, _array_octave[i]);

			gfx->bindVBO(_vbo, sizeof(Vec3) + sizeof(Vec2), 0);
			gfx->bindVertexPosition(0, 3);
			gfx->bindVertexTexCoord((uint8_t)sizeof(Vec3), 0, 2);

			gfx->drawIndexedPrimitives(GfxEnum::PRIMTYPE_TRIANGLES, _ibo, 0, _ibo->length() / sizeof(uint16_t) / 3);
		}
	};
};


