
#pragma once

#include "predecl.h"
#include "MeshRenderer.h"

namespace z3D
{
	namespace SG
	{
#pragma pack(push)
#pragma pack(1)

		// face for storing vertex & normal indices
		class vnFace
		{
		public:
			INT3		v;
			INT3		n;
		};

		// face for storing mapping vertex indices
		class mFace
		{
		public:
			INT3		m;
		};

		// face for storing vertex color indices
		class cFace
		{
		public:
			INT3		c;
		};

		class vfLookup : noncopyable
		{
		private:
			void*					_buffer;
		private:
			size_t					_vert_count;
		public:
			typedef struct _ENTRY
			{
				uint32_t			face_count;
				uint32_t			face_verts[1];					// actually face_index * 3 + which_corner
			} ENTRY;
		public:
			vfLookup();
			~vfLookup();
		public:
			void					build(const Vec3* verts, size_t vert_count, const vnFace* faces, size_t face_count);
		public:
			size_t					vert_count() const;
			ENTRY const * const *	lookup() const;
		};

		class ConvexGroup
		{
		private:
			uint32_t			_group_count;
			vector<int32_t>		_face_group_mapping;
		public:
			ConvexGroup();
		public:
			void				reset();
		public:
			bool				valid() const;
		public:
			void				build(const Vec3* verts, size_t vert_count, const vnFace* faces, size_t face_count, const INT3* faces_neighbors);
		public:
			int32_t				operator[](ptrdiff_t triidx) const;
		public:
			// utility classes

			// predicate for sorting
			class pred
			{
			public:
				const ConvexGroup&			cg;
			public:
				pred(const ConvexGroup& cg_);
			public:
				bool						operator () (const int32_t& e0, const int32_t& e1) const;
			};

			// get the no. of consecutive elements of the same group in a
			class group_length
			{
			public:
				const ConvexGroup&			cg;
			public:
				group_length(const ConvexGroup& cg_);
			public:
				size_t						get(const int32_t* a, size_t len);
			};
		};


		class mChan
		{
		public:
			int32_t						voffset;
			int32_t						vcount;
			int32_t						foffset;
		};

		class SubMesh
		{
		public:
			SubMesh();
		public:
			int32_t						face_count;
			int32_t						face_start;
			int32_t						mtl_index;
		};

#pragma pack(pop)

		class MtlMap
		{
		public:
			wcs						filepath;
		public:
			int32_t					channel;					// mapping channel, -1 - not applicable		// NEW
		public:
			GfxEnum::FILTERMODE		filter_min;
			GfxEnum::FILTERMODE		filter_mag;
			GfxEnum::FILTERMODE		filter_mip;

			GfxEnum::WRAPMODE		wrap_u;
			GfxEnum::WRAPMODE		wrap_v;
			GfxEnum::WRAPMODE		wrap_w;
		public:
			struct _UVGEN
			{
				REAL					offset_u;
				REAL					offset_v;
				REAL					tiling_u;
				REAL					tiling_v;
				REAL					angle_u;
				REAL					angle_v;
				REAL					angle_w;
			}						uvgen;
		public:
			SPtr<TexBase>			tex;
		public:
			struct _ANIMDATA
			{
				SPtr< AnimData<REAL> >	offset_u;
				SPtr< AnimData<REAL> >	offset_v;
				SPtr< AnimData<REAL> >	tiling_u;
				SPtr< AnimData<REAL> >	tiling_v;
				SPtr< AnimData<REAL> >	angle_u;
				SPtr< AnimData<REAL> >	angle_v;
				SPtr< AnimData<REAL> >	angle_w;

				bool				any() const	{	return offset_u || offset_v || tiling_u || tiling_v || angle_u || angle_v || angle_w;	}
			}					animdata;
		private:
			Mat4				_matrix;
			Mat4				_matrix_2d;
			unsigned long		_hash_code;
			unsigned long		_hash_code_2d;
		private:
			static MtlMap		_no_transform;
		public:
			MtlMap();
		public:
			bool				needTransform() const;
			const Mat4&			computeTransform();
			const Mat4&			computeTransform2D();
		public:
			void				loadTexture();
		private:
			unsigned long		_hash();
		};

		// not very much information, original combined with instanced
		class Mtl
		{
		public:
			static const Vec4	OPAQUE_WHITE;
		public:
			typedef enum _TYPE
			{
				TYPE_SOLID = 0,
				TYPE_TRANSPARENT,
			} TYPE;

			typedef enum _METHOD
			{
				METHOD_STANDARD = 0,
				METHOD_TOON,
			} METHOD;
		public:
			TYPE						type;
			METHOD						method;
		public:
			Vec3						ambient;
			Vec3						diffuse;
			Vec3						specular;
			REAL						specular_level;
			REAL						glossiness;
			REAL						opacity;
			uint32_t					lighting;
			uint32_t					two_sided;

			struct ALPHA_TEST
			{
				uint32_t					enabled;							// 0 - disabled, 1 - enabled, 2 - auto
				GfxEnum::CMPFUNC			func;
				REAL						ref;
			}							alpha_test;

			struct BLEND
			{
				uint32_t					enabled;
				GfxEnum::BLENDOPERATION		op;
				GfxEnum::BLENDFACTOR		sbf;
				GfxEnum::BLENDFACTOR		dbf;
			}							blend;

			REAL						reflect;
			int32_t						layer_index;							// full range of the signed integer is used, it is used to non-trivially solve the undeterministic draw order of transparent objects, think its no need to floating point to store that index, am i right?

			uint32_t					sep_opacity_mapping;					// NEW

			MtlMap						map_diffuse;
			MtlMap						map_opacity;							// NEW
			MtlMap						map_normal;
			MtlMap						map_selfill;
			MtlMap						map_specular;
			MtlMap						map_env;

			struct TOON_SHADE
			{
				wcs							map_src;
				REAL						outline_width;
				Vec3						outline_color;
			}							toon_shade;

		public:						// animation data
			struct _ANIMDATA
			{
				SPtr< AnimData<Vec3> >	ambient;
				SPtr< AnimData<Vec3> >	diffuse;
				SPtr< AnimData<Vec3> >	specular;
				SPtr< AnimData<REAL> >	specular_level;
				SPtr< AnimData<REAL> >	glossiness;
				SPtr< AnimData<REAL> >	opacity;
			}						animdata;
		public:						// instanced members
			SPtr<Tex2D>				toon_map;
		//public:
		//	SPtr<mrBase>			mesh_renderer;
		public:
			Mtl();
		public:
			void					releaseTextures();
		public:
			//void					setRenderMethod(RENDERMETHOD method,const SPtr<mrBase>& mesh_renderer_ = 0);
		public:
			//Vec3					computeAmbient() const;
			//Vec3					computeDiffuse() const;
			Vec3					computeSpecular() const;
			REAL					computeShininess() const;
		};
	};
};
