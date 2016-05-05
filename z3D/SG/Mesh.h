
#pragma once

#include "predecl.h"
#include "Mesh_structs.h"
#include "MeshRenderer.h"

namespace z3D
{
	namespace SG
	{
		class LoadedMesh : public ResBase
		{
		public:
			enum
			{
				MAX_MCHANNELS				=	8,
			};
		public:
			SMem						_filemem;
		public:
			const struct _DATA
			{
				size_t						face_count;

				size_t						vert_count;
				size_t						norm_count;
				size_t						color_count;
				size_t						mvert_count;
				size_t						mface_count;

				size_t						submesh_count;

				const Vec3*					verts;
				const Vec3*					norms;
				const Vec4*					colors;
				const Vec2*					mverts;

				const vnFace*				faces;								// count: face_count
				const cFace*				cfaces;								// count: face_count
				const mFace*				mfaces;								// count: mface_count

				const mChan*				mchans[MAX_MCHANNELS];

				// for tangents, please use getTangents()

				const SubMesh*				submeshes;
			}							data;
		private:
			Vec4*						_tangents[MAX_MCHANNELS];
		private:
			struct _VBO
			{
				version						group;

				WPtr<VBO>					vert;
				WPtr<VBO>					norm;
				WPtr<VBO>					color;
				WPtr<VBO>					mverts[MAX_MCHANNELS];
				WPtr<VBO>					tangents[MAX_MCHANNELS];
			}							_vbo;
		private:
			AABB						_bound;
			ptrdiff_t					_manifold_2;
		private:
			vector<INT3>				_face_neighbor_lookup;
			vector<Plane>				_face_plane;
		private:
			vfLookup					_vf_lookup;
		private:
			ConvexGroup					_convex_group;
		private:
			static ResMgr				_res_mgr;
		protected:
			LoadedMesh(const wcs& filepath, const SMem& sm, const _DATA& data, Vec4 const * const (&tangents)[MAX_MCHANNELS]);
			~LoadedMesh();
		public:
			static SPtr<LoadedMesh>		load(const wcs& filepath);
		private:
			static void					parse(const void* byte_array, size_t bytes, _DATA& data, const Vec4* (&tangents)[MAX_MCHANNELS]);
		public:
			const AABB&					bound() const;
		public:
			SPtr<VBO>					vbo_vert();
			SPtr<VBO>					vbo_norm();
			SPtr<VBO>					vbo_color();
			SPtr<VBO>					vbo_mvert(size_t chan);
			SPtr<VBO>					vbo_tangent(size_t chan);
		public:
			const vector<INT3>&			getFaceNeighborLookup();
			const vector<Plane>&		getFacePlane();
		public:
			bool						manifold_2();
		public:
			const vfLookup&				getVertexFaceLookup();
		public:
			const ConvexGroup&			getConvexGroup();
		public:
			const Vec4*					get_tangents(size_t chan) const;			// NEW
		public:
			void						buildTangents(size_t chan);
		};

		class LoadedMtl: public ResBase
		{
		public:
			vector<Mtl>					mtls;
		private:
			static ResMgr				_res_mgr;
		protected:
			LoadedMtl(const wcs& filepath);
		public:
			static SPtr<LoadedMtl>		load(const wcs& filepath);
		private:
			class Loader
			{
			public:
				vector<Mtl>					mtls;
			private:
				Mtl							_m;
				MtlMap*						_mtlmap;
			public:
				bool					load(const wcs& filepath);
			public:
				void					mtls_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void					mtl_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void					mtl_end(SaxBase* parser, const char* name);
				void					map_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void					map_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void					animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib);
			public:
				GfxEnum::CMPFUNC		getCmpFunc(const char* s);
				GfxEnum::BLENDOPERATION	getBlendOperation(const char* s);
				GfxEnum::BLENDFACTOR	getBlendFactor(const char* s);
				GfxEnum::FILTERMODE		getFilterMode(const char* s);
				GfxEnum::WRAPMODE		getWrapMode(const char* s);
			};
		};

		class MeshHelper
		{
		public:
			static void					__build_vbo_vert(SPtr<VBO>& vbo, size_t face_count, const Vec3* verts, const vnFace* faces, bool dynamic);
			static void					__build_vbo_norm(SPtr<VBO>& vbo, size_t face_count, const Vec3* norms, const vnFace* faces, bool dynamic);
			static void					__build_vbo_mvert(SPtr<VBO>& vbo, size_t face_count, const Vec2* mverts, const mFace* faces, bool dynamic);
			static void					__build_vbo_color(SPtr<VBO>& vbo, size_t face_count, const Vec4* colors, const cFace* faces, bool dynamic);
			static void					__build_vbo_nm_tangent(SPtr<VBO>& vbo, size_t face_count, const Vec4* tangents, bool dynamic);
		public:
			static void					__calc_bound(AABB& bound, const Vec3* verts, size_t vert_count);
			static void					__buildFaceNeighborLookup(vector<INT3>& face_neighbor_lookup, const vnFace* faces, size_t face_count);
			static void					__buildFacePlane(vector<Plane>& face_plane, const Vec3* array_vert, const vnFace* faces, size_t face_count);
			static bool					__isManifold2(const vector<INT3>& face_neighbor_lookup);
			static Vec4					__calcTangent(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec2& uv0, const Vec2& uv1, const Vec2& uv2, const Vec3* normal = NULL);
			static void					__build_nm_tangents(Vec4* tangents, const Vec3* verts, const Vec3* norms, const Vec2* mverts, const vnFace* faces, size_t face_count, const mFace* mfaces, const SubMesh* submeshes, size_t submesh_count, const vfLookup& vflookup);
		};

		class Mesh : public RefBase
		{
		public:
			struct _DATA
			{
				vt<vector<Vec3>>			verts;
				vt<vector<Vec3>>			norms;
				vt<vector<Vec4>>			tangents[8];

				vt<vector<Mtl>>				mtls;

			}							data;

		private:
			SPtr<LoadedMesh>			_ref;
			SPtr<LoadedMtl>				_ref_mtl;
		private:
			version						_group;
		private:
			struct	_VBO
			{
				SPtr<VBO>					vert;
				SPtr<VBO>					norm;
				SPtr<VBO>					color;
				SPtr<VBO>					mverts[LoadedMesh::MAX_MCHANNELS];
				SPtr<VBO>					tangents[8];
			}							_vbo;
		protected:
			vt<AABB>					_bound;
		protected:
			vt<vector<INT3>>			_face_neighbor_lookup;
			vt<vector<Plane>>			_face_plane;
		protected:
			vt<vector<TriangleAux>>		_array_face_tri;
		protected:
			vt<ptrdiff_t>				_manifold_2;
			vt<vfLookup>				_vf_lookup;
			vt<ConvexGroup>				_convex_group;
		protected:
			Mesh();
			~Mesh();
		public:
			static SPtr<Mesh>			create(const SPtr<LoadedMesh>& ref, const SPtr<LoadedMtl>& ref_mtl);
			static SPtr<Mesh>			load(const wcs& mesh_filepath, const wcs& mtl_filepath);
		public:
			const SPtr<LoadedMesh>&		getRef() const;
			const SPtr<LoadedMtl>&		getRefMtl() const;
		public:
			static bool					parseMtl(const wcs& filepath, vector<Mtl>& array_mtl);
		public:
			const Vec3*					get_verts() const;
			const Vec3*					get_norms() const;
			const Vec4*					get_colors() const;
			const Vec2*					get_mverts(size_t chan) const;
			const Vec4*					get_tangents(size_t chan) const;
			const vnFace*				get_faces() const;
			const cFace*				get_cfaces() const;
			const mFace*				get_mfaces(size_t chan) const;
			mChan const * const *		get_mchans() const;
			const SubMesh*				get_submeshes() const;
		public:
			size_t						get_vert_count() const;
			size_t						get_norm_count() const;
			size_t						get_color_count() const;
			size_t						get_mvert_count(size_t chan) const;
			size_t						get_face_count() const;
			size_t						get_submesh_count() const;
		public:
			bool						has_mchan(size_t chan) const;
		public:
			size_t						getSubMeshCount() const;
			const SubMesh&				getSubMesh(size_t index) const;
		public:
			Vec3						getFaceNormal(size_t triidx) const;
		public:
			const SPtr<VBO>&			get_vbo_vert();
			const SPtr<VBO>&			get_vbo_norm();
			const SPtr<VBO>&			get_vbo_color();
			const SPtr<VBO>&			get_vbo_mvert(size_t chan);
			const SPtr<VBO>&			get_vbo_tangent(size_t chan);
		public:
			void						loadTextures();
		public:
			const AABB&					local_bound();
		public:
			const vector<INT3>&			getFaceNeighborLookup();
		public:
			const vector<Plane>&		getFacePlane();
		public:
			const vector<TriangleAux>&	getFaceTri();
		public:
			bool						manifold_2();
		public:
			const vfLookup&				getVertexFaceLookup();
			const ConvexGroup&			getConvexGroup();
		public:
			void						buildTangents(size_t chan);
		};
	};
};

