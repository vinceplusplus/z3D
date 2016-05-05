
#ifdef _OPENMP
#include <omp.h>
#endif

#include "Mesh.h"

#include "../Core/allocators_inl.h"

namespace z3D
{
	namespace SG
	{
		SubMesh::SubMesh()
		{
			face_start = 0;
			face_count = 0;
			mtl_index = 0;
		}

		vfLookup::vfLookup()
		{
			_vert_count = 0;
			_buffer = NULL;
		}
		vfLookup::~vfLookup()
		{
			if(_buffer)
				delete _buffer;
		}
		void					vfLookup::build(const Vec3* verts, size_t vert_count, const vnFace* faces, size_t face_count)
		{
			vector<vector<int32_t>> lookup(vert_count);				// use int32_t instead of uint32_t because vc will get confused when mixing use of size_t and unsigned int (or uint32_t)
			for(size_t i = 0; i < face_count; ++i)
			{
				const vnFace& f = faces[i];
				lookup[f.v[0]].push_back((uint32_t)(i * 3 + 0));
				lookup[f.v[1]].push_back((uint32_t)(i * 3 + 1));
				lookup[f.v[2]].push_back((uint32_t)(i * 3 + 2));
			}
			size_t s = sizeof(intptr_t) * vert_count + sizeof(uint32_t) * vert_count + sizeof(uint32_t) * face_count * 3;
			if(_buffer)
				delete _buffer;
			_buffer = new char[s];
			ENTRY* e = (ENTRY*)((char*)_buffer + sizeof(intptr_t) * vert_count);
			ENTRY** array_index = (ENTRY**)_buffer;
			for(size_t i = 0; i < vert_count; ++i)
			{
				const vector<int32_t>& a = lookup[i];
				e->face_count = (uint32_t)a.size();
				for(size_t j = 0; j < a.size(); ++j)
					e->face_verts[j] = a[j];
				array_index[i] = e;
				e = (ENTRY*)((char*)e + sizeof(uint32_t) * (1 + a.size()));
			}
		}
		size_t					vfLookup::vert_count() const
		{
			return _vert_count;
		}
		vfLookup::ENTRY const * const *		vfLookup::lookup() const
		{
			Z_ASSERT( _buffer );
			return (ENTRY**)_buffer;
		}

		ConvexGroup::ConvexGroup()
		{
			reset();
		}
		void				ConvexGroup::reset()
		{
			_group_count = 0;
			_face_group_mapping.clear();
		}
		bool				ConvexGroup::valid() const
		{
			return !!_group_count;
		}
		void				ConvexGroup::build(const Vec3* verts, size_t vert_count, const vnFace* faces, size_t face_count, const INT3* faces_neighbors)
		{
			reset();

			Z_ASSERT( vert_count );
			Z_ASSERT( face_count );

			_face_group_mapping.resize(face_count);

			vector<bool> proc_faces;
			proc_faces.resize(face_count, false);

			vector<uint32_t> faces_to_search;

			for(size_t i = 0; i < face_count; ++i)
			{
				if(proc_faces[i])
					continue;

				proc_faces[i] = true;

				faces_to_search.clear();

				faces_to_search.push_back(i);

				for(size_t j = 0; j < faces_to_search.size(); ++j)
				{
					uint32_t fi = (uint32_t)faces_to_search[j];

					_face_group_mapping[fi] = _group_count;

					const vnFace& f = faces[fi];
					Vec3 fn = ((verts[f.v[1]] - verts[f.v[0]]) ^ (verts[f.v[2]] - verts[f.v[0]])).normal_safe();
					Plane fp(verts[f.v[0]], fn);

					const INT3& neighbors = faces_neighbors[fi];
					for(size_t k = 0; k < 3; ++k)
					{
						uint32_t fi2 = neighbors[k];
						if(fi2 == -1)
							continue;
						if(proc_faces[fi2])
							continue;

						uint32_t ev0 = f.v[k];
						uint32_t ev1 = f.v[(k + 1) % 3];
						const vnFace& f2 = faces[fi2];

						bool add_face = false;

						for(size_t l = 0; l < 3; ++l)
						{
							if(f2.v[l] == ev0 || f2.v[l] == ev1)
								continue;

							if(fp.sub(verts[f2.v[l]]) < 0.0f)
							{
								add_face = true;
								break;
							}
							REAL angle = ACOS(fn * (verts[f2.v[l]] - verts[ev0]).normal_safe());
							if(angle >= 89.95f)
							{
								add_face = true;
								break;
							}

							break;
						}

						if(add_face)
						{
							faces_to_search.push_back(fi2);

							proc_faces[fi2] = true;
						}
					}
				}

				++_group_count;
			}
		}
		int32_t				ConvexGroup::operator[](ptrdiff_t triidx) const
		{
			Z_ASSERT( triidx >= 0 && (size_t)triidx < _face_group_mapping.size() );

			return _face_group_mapping[triidx];
		}

		ConvexGroup::pred::pred(const ConvexGroup& cg_)
			: cg(cg_)
		{}
		bool						ConvexGroup::pred::operator () (const int32_t& e0, const int32_t& e1) const {return cg[e0] < cg[e1];}

		ConvexGroup::group_length::group_length(const ConvexGroup& cg_)
			: cg(cg_)
		{}
		size_t						ConvexGroup::group_length::get(const int32_t* a, size_t len)
		{
			const int32_t& g = cg[a[0]];

			const int32_t* e = a + len;
			for(const int32_t* p = a + 1; p != e; ++p)
				if(cg[*p] != g)
					return p - a;
			return len;
		}


		MtlMap::MtlMap()
		{
			channel = 0;

			filter_min = GfxEnum::FILTERMODE_LINEAR;
			filter_mag = GfxEnum::FILTERMODE_LINEAR;
			filter_mip = GfxEnum::FILTERMODE_LINEAR;

			wrap_u = GfxEnum::WRAPMODE_REPEAT;
			wrap_v = GfxEnum::WRAPMODE_REPEAT;
			wrap_w = GfxEnum::WRAPMODE_REPEAT;

			uvgen.offset_u = 0;
			uvgen.offset_v = 0;
			uvgen.tiling_u = 1;
			uvgen.tiling_v = 1;
			uvgen.angle_u = 0;
			uvgen.angle_v = 0;
			uvgen.angle_w = 0;

			_matrix = Mat4::identity;
			_hash_code = 0;
			_hash_code_2d = 0;
		}
		bool				MtlMap::needTransform() const
		{
			Z_ASSERT(sizeof(int) == 4);
			Z_ASSERT(sizeof(REAL) == 4);

			return !!memcmp(&uvgen, &_no_transform.uvgen, sizeof(uvgen));
		}
		const Mat4&			MtlMap::computeTransform()
		{
			unsigned long hash_code = _hash();
			if(_hash_code != hash_code)
			{
				_matrix = Mat4::translation(Vec3(-uvgen.offset_u, -uvgen.offset_v, 0)) * Mat4::translation(-0.5f, -0.5f, 0) * Mat4::scaling(uvgen.tiling_u, uvgen.tiling_v, 1) * Mat4::translation(0.5f, 0.5f, 0) * Mat4::rotationX(uvgen.angle_u) * Mat4::rotationY(uvgen.angle_v) * Mat4::rotationZ(uvgen.angle_w);

				_hash_code = hash_code;
			}
			return _matrix;
		}
		const Mat4&			MtlMap::computeTransform2D()
		{
			unsigned long hash_code = _hash();
			if(_hash_code_2d != hash_code)
			{
				Mat3 trans = Mat3::identity;
				trans[2][0] = -0.5f;trans[2][1] = -0.5f;
				Mat3 invtrans = Mat3::identity;
				invtrans[2][0] = 0.5f;invtrans[2][1] = 0.5f;

				_matrix_2d = (Mat4)(Mat3::rotationZ(uvgen.angle_w) * Mat3(Vec3::xAxis, Vec3::yAxis, Vec3(-uvgen.offset_u, -uvgen.offset_v, 0)) * trans * Mat3::scaling(uvgen.tiling_u, uvgen.tiling_v, 1) * invtrans);

				_hash_code_2d = hash_code;
			}
			return _matrix_2d;
		}
		void				MtlMap::loadTexture()
		{
			if((!tex && filepath.length()) || (tex && tex->filepath() != filepath))					// no case handling??
			{
				size_t p = filepath.rfind('.');
				if(p != filepath.npos)
				{
					if(_wcsicmp(filepath.substr(p + 1).c_str(), L"dds") == 0)
						tex = TexCube::load(filepath, true, GfxEnum::PIXELFORMAT_A8R8G8B8, false);
				}
				if(!tex)
				{
					bool non_pow2 = false;
					if(Gfx::current())
						non_pow2 = Gfx::current()->supportsNonPower2Tex();
					tex = Tex2D::load(filepath, true, GfxEnum::PIXELFORMAT_A8R8G8B8, non_pow2);
				}
			}
		}
		unsigned long		MtlMap::_hash()
		{
			return z3D::Core::gen_crc(&uvgen, sizeof(uvgen));

			//unsigned long code=0;
			//code ^= *(unsigned long*)&offset_u;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&offset_v;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&tiling_u;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&tiling_v;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&offset_u;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&angle_u;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&angle_v;
			//code = (code<<31) | (code>>1);
			//code ^= *(unsigned long*)&angle_w;
			//code = (code<<31) | (code>>1);

			//return code;
		}


		Mtl::Mtl()
		{
			type = TYPE_SOLID;
			method = METHOD_STANDARD;

			ambient = Vec3(1, 1, 1);
			diffuse = Vec3(1, 1, 1);
			specular = Vec3(1, 1, 1);
			specular_level = 0.0f;
			glossiness = 0.1f;
			opacity = 1.0f;
			lighting = false;
			two_sided = false;

			alpha_test.enabled = 2;						// AUTO
			alpha_test.func = GfxEnum::CMPFUNC_GREATER;
			alpha_test.ref = 0.0f;

			blend.enabled = false;
			blend.op = GfxEnum::BLENDOPERATION_ADD;
			blend.sbf = GfxEnum::BLENDFACTOR_ONE;
			blend.dbf = GfxEnum::BLENDFACTOR_ZERO;

			reflect = 0.0f;
			layer_index = 0;

			sep_opacity_mapping = false;

			toon_shade.outline_width = 0.0f;
			toon_shade.outline_color = Vec3(0, 0, 0);
		}
		void					Mtl::releaseTextures()
		{
			map_diffuse.tex = NULL;
			map_normal.tex = NULL;
			map_selfill.tex = NULL;
			map_specular.tex = NULL;
			map_env.tex = NULL;
			toon_map.reset();
		}
		//void					Mtl::setRenderMethod(RENDERMETHOD method,const SPtr<mrBase>& mesh_renderer_)
		//{
		//	render_method = method;
		//	mesh_renderer = mesh_renderer_;
		//}
		//Vec4					Mtl::computeAmbient() const
		//{
		//	return ambient;
		//}
		//Vec4					Mtl::computeDiffuse() const
		//{
		//	return Vec4(diffuse.x,diffuse.y,diffuse.z,opacity);
		//}
		Vec3					Mtl::computeSpecular() const
		{
			return specular * specular_level;
		}
		REAL					Mtl::computeShininess() const
		{
			return min(max(0.0f, 128.0f * glossiness), 128.0f);
		}

		LoadedMesh::LoadedMesh(const wcs& filepath, const SMem& sm, const _DATA& data_, Vec4 const * const (&tangents)[MAX_MCHANNELS]): ResBase(filepath), _filemem(sm), data(data_)
		{
			memcpy(_tangents, tangents, sizeof(_tangents));
			MeshHelper::__calc_bound(_bound, data.verts, data.vert_count);
		}
		LoadedMesh::~LoadedMesh()
		{
			for(size_t i = 0; i < _countof(_tangents); ++i)
				if(!_tangents[i])
					continue;
				else if((const char*)_tangents[i] < (const char*)_filemem.get() || (const char*)_tangents[i] >= (const char*)_filemem.get() + _filemem.size())
					delete _tangents[i];
		}
		SPtr<LoadedMesh>			LoadedMesh::load(const wcs& filepath)
		{
			if(true)
			{
				LoadedMesh* m;
				if((m = _res_mgr.find<LoadedMesh>(filepath)) != NULL)
					return m;
			}

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			_DATA data;
			const Vec4* tangents[_countof(data.mchans)];

			parse(sm.get(), sm.size(), data, tangents);

			SPtr<LoadedMesh> m = new LoadedMesh(filepath, sm, data, tangents);

			_res_mgr.add(m.get());

			return m;
		}
		void						LoadedMesh::parse(const void* byte_array, size_t byte_count, _DATA& data, const Vec4* (&tangents)[MAX_MCHANNELS])
		{
			MemReader reader(byte_array, byte_count);

			if(true)
			{
				char header[4];
				reader.read(header, sizeof(header));
				Z_VERIFY( strncmp(header, "ZMSH", 4) == 0 );

				uint32_t version = reader.read<uint32_t>();
				Z_VERIFY( version == 1 );
			}

			data.face_count = reader.read<uint32_t>();

			data.vert_count = reader.read<uint32_t>();
			data.norm_count = reader.read<uint32_t>();
			data.color_count = reader.read<uint32_t>();
			data.mvert_count = reader.read<uint32_t>();
			data.mface_count = reader.read<uint32_t>();

			data.submesh_count = reader.read<uint32_t>();

			data.verts = (const Vec3*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.vert_count * sizeof(data.verts[0]));

			data.norms = (const Vec3*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.norm_count * sizeof(data.norms[0]));

			data.colors = NULL;
			if(data.color_count)
			{
				data.colors = (const Vec4*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + data.color_count * sizeof(data.colors[0]));
			}

			data.mverts = (const Vec2*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.mvert_count * sizeof(data.mverts[0]));

			data.faces = (const vnFace*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.face_count * sizeof(data.faces[0]));

			data.cfaces = NULL;
			if(data.color_count)
			{
				data.cfaces = (const cFace*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + data.face_count * sizeof(data.cfaces[0]));
			}

			data.mfaces = (const mFace*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.mface_count * sizeof(data.mfaces[0]));

			memset(data.mchans, 0, sizeof(data.mchans));
			uint32_t chan_count = reader.read<uint32_t>();
			for(size_t i = 0; i < chan_count; ++i)
			{
				uint32_t index = reader.read<uint32_t>();
				data.mchans[index] = (const mChan*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + sizeof(*data.mchans[0]));
				reader.seek(reader.offset() + sizeof(uint32_t));						// embed tangents??
			}

			memset(tangents, 0, sizeof(tangents));
			for(size_t i = 0; i < _countof(data.mchans); ++i)
			{
				if(!data.mchans[i])
					continue;
				if(!(*(uint32_t*)&data.mchans[i][1]))
					continue;
				tangents[i] = (const Vec4*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + data.face_count * 3 * sizeof(tangents[i][0]));
			}

			data.submeshes = (const SubMesh*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + data.submesh_count * sizeof(data.submeshes[0]));
		}
		const AABB&					LoadedMesh::bound() const
		{
			return _bound;
		}
		SPtr<VBO>					LoadedMesh::vbo_vert()
		{
			SPtr<VBO> vbo = _vbo.vert.lock();
			if(!vbo)
			{
				MeshHelper::__build_vbo_vert(vbo, data.face_count, data.verts, data.faces, false);
				if(!vbo)
					return NULL;
				vbo->ver.setGroup(_vbo.group);
				vbo->ver.update();
				_vbo.vert = vbo;
			}
			return vbo;
		}
		SPtr<VBO>					LoadedMesh::vbo_norm()
		{
			SPtr<VBO> vbo = _vbo.norm.lock();
			if(!vbo)
			{
				MeshHelper::__build_vbo_norm(vbo, data.face_count, data.norms, data.faces, false);
				if(!vbo)
					return NULL;
				vbo->ver.setGroup(_vbo.group);
				vbo->ver.update();
				_vbo.norm = vbo;
			}
			return vbo;
		}
		SPtr<VBO>					LoadedMesh::vbo_color()
		{
			SPtr<VBO> vbo = _vbo.color.lock();
			if(!vbo)
			{
				Z_ASSERT( data.color_count );
				MeshHelper::__build_vbo_color(vbo, data.face_count, data.colors, data.cfaces, false);
				if(!vbo)
					return NULL;
				vbo->ver.setGroup(_vbo.group);
				vbo->ver.update();
				_vbo.color = vbo;
			}
			return vbo;
		}
		SPtr<VBO>					LoadedMesh::vbo_mvert(size_t chan)
		{
			SPtr<VBO> vbo = _vbo.mverts[chan].lock();
			if(!vbo)
			{
				Z_ASSERT( data.mchans[chan] );
				MeshHelper::__build_vbo_mvert(vbo, data.face_count, data.mverts + data.mchans[chan]->voffset, data.mfaces + data.mchans[chan]->foffset, false);
				if(!vbo)
					return NULL;
				vbo->ver.setGroup(_vbo.group);
				vbo->ver.update();
				_vbo.mverts[chan] = vbo;
			}
			return vbo;
		}
		SPtr<VBO>					LoadedMesh::vbo_tangent(size_t chan)
		{
			SPtr<VBO> vbo = _vbo.tangents[chan].lock();
			if(!vbo)
			{
				Z_ASSERT( data.mchans[chan] );
				if(!get_tangents(chan))
					buildTangents(chan);
				MeshHelper::__build_vbo_nm_tangent(vbo, data.face_count, get_tangents(chan), false);
				if(!vbo)
					return NULL;
				vbo->ver.setGroup(_vbo.group);
				vbo->ver.update();
				_vbo.tangents[chan] = vbo;
			}
			return vbo;
		}
		const vector<INT3>&			LoadedMesh::getFaceNeighborLookup()
		{
			if(!_face_neighbor_lookup.size())
				MeshHelper::__buildFaceNeighborLookup(_face_neighbor_lookup, data.faces, data.face_count);
			return _face_neighbor_lookup;
		}
		const vector<Plane>&		LoadedMesh::getFacePlane()
		{
			if(!_face_plane.size())
				MeshHelper::__buildFacePlane(_face_plane, data.verts, data.faces, data.face_count);
			return _face_plane;
		}
		bool						LoadedMesh::manifold_2()
		{
			if(!_manifold_2)
			{
				getFaceNeighborLookup();

				_manifold_2 = 1 + MeshHelper::__isManifold2(_face_neighbor_lookup);
			}
			return !!(_manifold_2 - 1);
		}
		const vfLookup&				LoadedMesh::getVertexFaceLookup()
		{
			if(!_vf_lookup.vert_count())
				_vf_lookup.build(data.verts, data.vert_count, data.faces, data.face_count);
			return _vf_lookup;
		}
		const ConvexGroup&			LoadedMesh::getConvexGroup()
		{
			if(!_convex_group.valid())
				_convex_group.build(data.verts, data.vert_count, data.faces, data.face_count, VECTOR_BEGIN_PTR(getFaceNeighborLookup()));
			return _convex_group;
		}
		const Vec4*					LoadedMesh::get_tangents(size_t chan) const
		{
			return _tangents[chan];
		}

		void						LoadedMesh::buildTangents(size_t chan)
		{
			if(_tangents[chan])
				return;
			_tangents[chan] = new Vec4[data.face_count * 3];
			MeshHelper::__build_nm_tangents(_tangents[chan], data.verts, data.norms, data.mverts + data.mchans[chan]->voffset, data.faces, data.face_count, data.mfaces + data.mchans[chan]->foffset, data.submeshes, data.submesh_count, getVertexFaceLookup());
		}

		void				MeshHelper::__build_vbo_vert(SPtr<VBO>& vbo, size_t face_count, const Vec3* verts, const vnFace* faces, bool dynamic)
		{
			size_t len = face_count * 3 * sizeof(Vec3);
			if(!len)
				return;
			if(!vbo || vbo->length() < len || vbo->dynamic() != dynamic)
				vbo = VBO::create(len, dynamic);
			Z_ASSERT( vbo );
			vbo->lock();

			Vec3* v = (Vec3*)vbo->pointer();
			const vnFace* end_face = faces + face_count;
			for(const vnFace* f = faces; f != end_face; ++f)
			{
				memcpy(v + 0, verts + f->v[0], sizeof(*v));
				memcpy(v + 1, verts + f->v[1], sizeof(*v));
				memcpy(v + 2, verts + f->v[2], sizeof(*v));
				v += 3;
			}

			vbo->unlock();
		}
		void				MeshHelper::__build_vbo_norm(SPtr<VBO>& vbo, size_t face_count, const Vec3* norms, const vnFace* faces, bool dynamic)
		{
			size_t len = face_count * 3 * sizeof(Vec3);
			if(!len)
				return;
			if(!vbo || vbo->length() < len || vbo->dynamic() != dynamic)
				vbo = VBO::create(len, dynamic);
			Z_ASSERT( vbo );
			vbo->lock();

			Vec3* v = (Vec3*)vbo->pointer();
			const vnFace* end_face = faces + face_count;
			for(const vnFace* f = faces; f != end_face; ++f)
			{
				memcpy(v + 0, norms + f->n[0], sizeof(*v));
				memcpy(v + 1, norms + f->n[1], sizeof(*v));
				memcpy(v + 2, norms + f->n[2], sizeof(*v));
				v += 3;
			}

			vbo->unlock();
		}
		void				MeshHelper::__build_vbo_mvert(SPtr<VBO>& vbo, size_t face_count, const Vec2* mverts, const mFace* faces, bool dynamic)
		{
			size_t len = face_count * 3 * sizeof(Vec2);
			if(!len)
				return;
			if(!vbo || vbo->length() < len || vbo->dynamic() != dynamic)
				vbo = VBO::create(len, dynamic);
			Z_ASSERT( vbo );
			vbo->lock();

			Vec2* v = (Vec2*)vbo->pointer();
			const mFace* end_face = faces + face_count;
			for(const mFace* f = faces; f != end_face; ++f)
			{
				memcpy(v + 0, mverts + f->m[0], sizeof(*v));
				memcpy(v + 1, mverts + f->m[1], sizeof(*v));
				memcpy(v + 2, mverts + f->m[2], sizeof(*v));
				v += 3;
			}

			vbo->unlock();
		}
		void				MeshHelper::__build_vbo_color(SPtr<VBO>& vbo, size_t face_count, const Vec4* colors, const cFace* faces, bool dynamic)
		{
			size_t len = face_count * 3 * sizeof(Vec4);
			if(!len)
				return;
			if(!vbo || vbo->length() < len || vbo->dynamic() != dynamic)
				vbo = VBO::create(len, dynamic);
			Z_ASSERT( vbo );
			vbo->lock();

			Vec4* v = (Vec4*)vbo->pointer();
			const cFace* end_face = faces + face_count;
			for(const cFace* f = faces; f != end_face; ++f)
			{
				memcpy(v + 0, colors + f->c[0], sizeof(*v));
				memcpy(v + 1, colors + f->c[1], sizeof(*v));
				memcpy(v + 2, colors + f->c[2], sizeof(*v));
				v += 3;
			}

			vbo->unlock();
		}
		void						MeshHelper::__build_vbo_nm_tangent(SPtr<VBO>& vbo, size_t face_count, const Vec4* tangents, bool dynamic)
		{
			size_t len = face_count * 3 * sizeof(Vec4);
			if(!len)
				return;
			if(!vbo || vbo->length() < len || vbo->dynamic() != dynamic)
				vbo = VBO::create(len, dynamic);
			Z_ASSERT( vbo );
			vbo->lock();

			memcpy(vbo->pointer(), tangents, len);

			vbo->unlock();
		}
		void						MeshHelper::__calc_bound(AABB& bound, const Vec3* verts, size_t vert_count)
		{
			if(!vert_count)
			{
				bound = AABB::empty_bound;
				return;
			}
			AABB b;
#ifndef _OPENMP
			const Vec3* end = verts + vert_count;
			for(const Vec3* p = verts; p != end; ++p)
				b.fit(*p);
#else
#pragma omp parallel
			{
				Vec3 min_ = verts[0];
				Vec3 max_ = verts[0];
#pragma omp for
				for(ptrdiff_t i = 0; i < (ptrdiff_t)vert_count; ++i)
				{
					const Vec3& v = verts[i];
					min_[0] = min(min_[0], v[0]);
					min_[1] = min(min_[1], v[1]);
					min_[2] = min(min_[2], v[2]);
					max_[0] = max(max_[0], v[0]);
					max_[1] = max(max_[1], v[1]);
					max_[2] = max(max_[2], v[2]);
				}

#pragma omp critical
				{
					b.fit(min_);
					b.fit(max_);
				}
			}
#endif

			bound = b;
		}
		void						MeshHelper::__buildFaceNeighborLookup(vector<INT3>& face_neighbor_lookup, const vnFace* faces, size_t face_count)
		{
			Z_ASSERT(faces);
			face_neighbor_lookup.resize(face_count);
			memset(&face_neighbor_lookup[0], 0xff, sizeof(face_neighbor_lookup[0]) * face_neighbor_lookup.size());

			map<uint64_t, int> edge_face;
			uint32_t fi = 0;
			const vnFace* end = faces + face_count;
			for(const vnFace* p = faces; p != end; ++p)
			{
				const vnFace& f = *p;
				uint32_t* v = (uint32_t*)&f.v;
				edge_face[(((uint64_t)v[0]) << 32) | v[1]] = fi;
				edge_face[(((uint64_t)v[1]) << 32) | v[2]] = fi;
				edge_face[(((uint64_t)v[2]) << 32) | v[0]] = fi;

				map<uint64_t, int>::iterator iter2;
				if((iter2 = edge_face.find((((uint64_t)v[1]) << 32) | v[0])) != edge_face.end())
				{
					int fi2 = iter2->second;
					face_neighbor_lookup[fi][0] = fi2;
					face_neighbor_lookup[fi2][0] = fi;
				}
				if((iter2 = edge_face.find((((uint64_t)v[2]) << 32) | v[1])) != edge_face.end())
				{
					int fi2 = iter2->second;
					face_neighbor_lookup[fi][1] = fi2;
					face_neighbor_lookup[fi2][1] = fi;
				}
				if((iter2 = edge_face.find((((uint64_t)v[0]) << 32) | v[2])) != edge_face.end())
				{
					int fi2 = iter2->second;
					face_neighbor_lookup[fi][2] = fi2;
					face_neighbor_lookup[fi2][2] = fi;
				}

				++fi;
			}
		}
		void						MeshHelper::__buildFacePlane(vector<Plane>& face_plane, const Vec3* verts, const vnFace* faces, size_t face_count)
		{
			face_plane.clear();
			const vnFace* end = faces + face_count;
			for(const vnFace* iter = faces; iter != end; ++iter)
			{
				const vnFace& f = *iter;
				face_plane.push_back(Plane(verts[f.v[0]], verts[f.v[1]], verts[f.v[2]],false));
			}
		}
		bool						MeshHelper::__isManifold2(const vector<INT3>& face_neighbor_lookup)
		{
			const INT3* end = VECTOR_END_PTR(face_neighbor_lookup);
			for(const INT3* iter = VECTOR_BEGIN_PTR(face_neighbor_lookup); iter != end; ++iter)
				if((*iter)[0] == 0xffffffff || (*iter)[0] == 0xffffffff || (*iter)[0] == 0xffffffff)
					return false;
			return true;
		}

		Vec4						MeshHelper::__calcTangent(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec2& uv0, const Vec2& uv1, const Vec2& uv2, const Vec3* normal)
		{
			Vec3 e0 = v1 - v0;
			Vec3 e1 = v2 - v0;

			Vec3 u0 = uv1 - uv0;
			Vec3 u1 = uv2 - uv0;

			REAL det = u0.x * u1.y - u0.y * u1.x;
			if(det == 0.0f)
				det = 1.0f;
			REAL inv_det = 1.0f / det;
			if(!FINITE(inv_det))
				inv_det = 1.0f;

			Vec3 T = (inv_det * (u1.y * e0 - u0.y * e1)).normal_safe();
			Vec3 B = (inv_det * (-u1.x * e0 + u0.x * e1)).normal_safe();

			Vec3 N = (e0 ^ e1).normal_safe();

			REAL handedness = COPYSIGN(1.0f, (T ^ B) * N);

			if(true)
			{
				Vec3 T2 = (handedness * B) ^ N;
				T = ((T + T2) * 0.5f).normal_safe();
			}

			if(normal)
				T = (T - N * (N * T)).normal_safe();

			return Vec4(T, handedness);
		}
		void						MeshHelper::__build_nm_tangents(Vec4* tangents, const Vec3* verts, const Vec3* norms, const Vec2* mverts, const vnFace* faces, size_t face_count, const mFace* mfaces, const SubMesh* submeshes, size_t submesh_count, const vfLookup& vflookup)
		{
			Vec4* temp_tangents = (Vec4*)g_frame_temp_mem_allocator.alloc(sizeof(Vec4) * face_count * 3);

			//for(size_t i = 0; i < face_count; ++i)
#ifdef _OPENMP
#pragma omp parallel for
#endif
			for(ptrdiff_t i = 0; i < (ptrdiff_t)face_count; ++i)
			{
				const vnFace& face = faces[i];
				const mFace& mface = mfaces[i];

				Vec3 e0 = verts[face.v[1]] - verts[face.v[0]];
				Vec3 e1 = verts[face.v[2]] - verts[face.v[0]];

				Vec3 u0 = mverts[mface.m[1]] - mverts[mface.m[0]];
				Vec3 u1 = mverts[mface.m[2]] - mverts[mface.m[0]];

				REAL det = u0.x * u1.y - u0.y * u1.x;
				if(det == 0.0f)
					det = 1.0f;
				REAL inv_det = 1.0f / det;
				if(!FINITE(inv_det))
					inv_det = 1.0f;

				Vec3 T = (inv_det * (u1.y * e0 - u0.y * e1)).normal_safe();
				Vec3 B = (inv_det * (-u1.x * e0 + u0.x * e1)).normal_safe();

				Vec3 N = (e0 ^ e1).normal_safe();

				REAL handedness = COPYSIGN(1.0f, (T ^ B) * N);

				if(true)
				{
					Vec3 T2 = (handedness * B) ^ N;
					T = ((T + T2) * 0.5f).normal_safe();
				}

				temp_tangents[i * 3 + 0] = Vec4(T, handedness);
				temp_tangents[i * 3 + 1] = Vec4(T, handedness);
				temp_tangents[i * 3 + 2] = Vec4(T, handedness);
			}

			vfLookup::ENTRY const * const * lookup = vflookup.lookup();

			for(size_t i = 0; i < submesh_count; ++i)
			{
				const SubMesh& s = submeshes[i];
				size_t c = (size_t)s.face_count;

				//for(size_t j = 0; j < c; ++j)
#ifdef _OPENMP
#pragma omp parallel for
#endif
				for(ptrdiff_t j = 0; j < (ptrdiff_t)c; ++j)
				{
					size_t f = s.face_start + j;
					const vnFace& face = faces[f];
					const mFace& mface = mfaces[f];

					size_t t = f * 3;
					for(size_t k = 0; k < 3; ++k, ++t)
					{
						const Vec4& tangent = temp_tangents[t];

						Vec3 tangent_sum = *(Vec3*)&tangent;

						uint32_t corner = (uint32_t)(t);

						const vfLookup::ENTRY& e = *lookup[face.v[k]];
						for(uint32_t l = 0; l < e.face_count; ++l)
						{
							int32_t cor;
							if((cor = e.face_verts[l]) == corner)											// check referring the same corner
								continue;

							int32_t face_idx = cor / 3;
							if(face_idx < s.face_start || face_idx >= s.face_start + s.face_count)			// check same submesh
								continue;
							int32_t face_cor = cor % 3;
							if(faces[face_idx].n[face_cor] != face.n[k])									// check same normal idx (if so, so is the vertex idx)
								continue;
							if(mfaces[face_idx].m[face_cor] != mface.m[k])									// check same normal map uv idx
								continue;
							const Vec4& tangent2 = temp_tangents[face_idx * 3 + face_cor];
							if(tangent.w != tangent2.w)														// check same tangent space handedness
								continue;
							if(ACOS((*(Vec3*)&tangent) * (*(Vec3*)&tangent2)) > 90.0f)						// check if the directions differ too much
								continue;

							tangent_sum += *(Vec3*)&tangent2;
						}

						if(true)
						{
							const Vec3& N = norms[face.n[k]];
							tangent_sum = tangent_sum - N * (N * tangent_sum);								// orthogonalize to respect the vertex normal
						}

						tangent_sum.normalize_safe();

						tangents[t] = Vec4(tangent_sum, tangent.w);				// now add that averaged tangent
					}
				}
			}

			g_frame_temp_mem_allocator.dealloc(temp_tangents);
		}

		Mesh::Mesh()
		{
			data.verts.ver.setGroup(_group);
			data.norms.ver.setGroup(_group);
			for(size_t i = 0; i < _countof(data.tangents); ++i)
				data.tangents[0].ver.setGroup(_group);
			data.mtls.ver.setGroup(_group);

			_bound.ver.setGroup(_group);
			_face_neighbor_lookup.ver.setGroup(_group);
			_face_plane.ver.setGroup(_group);
			_array_face_tri.ver.setGroup(_group);
			_manifold_2.ver.setGroup(_group);
			_vf_lookup.ver.setGroup(_group);
		}
		Mesh::~Mesh()
		{
		}
		SPtr<Mesh>			Mesh::create(const SPtr<LoadedMesh>& ref, const SPtr<LoadedMtl>& ref_mtl)
		{
			Z_ASSERT( ref );
			Z_ASSERT( ref_mtl );

			SPtr<Mesh> mesh = new Mesh();

			mesh->_ref = ref;
			mesh->_ref_mtl = ref_mtl;

			mesh->data.mtls.edit() = ref_mtl->mtls;

			return mesh;
		}
		SPtr<Mesh>			Mesh::load(const wcs& mesh_filepath, const wcs& mtl_filepath)
		{
			SPtr<LoadedMesh> ref = LoadedMesh::load(mesh_filepath);
			if(!ref)
				return NULL;

			SPtr<LoadedMtl> mtl = LoadedMtl::load(mtl_filepath);
			if(!mtl)
				return NULL;

			return Mesh::create(ref, mtl);
		}
		const SPtr<LoadedMesh>&		Mesh::getRef() const
		{
			return _ref;
		}
		const SPtr<LoadedMtl>&		Mesh::getRefMtl() const
		{
			return _ref_mtl;
		}
		const Vec3*					Mesh::get_verts() const
		{
			if(data.verts.get().size())
				return VECTOR_BEGIN_PTR(data.verts.get());
			return _ref->data.verts;
		}
		const Vec3*					Mesh::get_norms() const
		{
			if(data.norms.get().size())
				return VECTOR_BEGIN_PTR(data.norms.get());
			return _ref->data.norms;
		}
		const Vec4*					Mesh::get_colors() const
		{
			return _ref->data.colors;
		}
		const Vec2*					Mesh::get_mverts(size_t chan) const
		{
			Z_ASSERT( _ref->data.mchans[chan] );
			return _ref->data.mverts + _ref->data.mchans[chan]->voffset;
		}
		const Vec4*					Mesh::get_tangents(size_t chan) const
		{
			if(data.verts.get().size() || data.norms.get().size())
			{
				if(data.tangents[chan].get().size())
					return VECTOR_BEGIN_PTR(data.tangents[chan].get());
				else
					return NULL;
			}
			return _ref->get_tangents(chan);
		}
		const vnFace*				Mesh::get_faces() const
		{
			return _ref->data.faces;
		}
		const cFace*				Mesh::get_cfaces() const
		{
			return _ref->data.cfaces;
		}
		const mFace*				Mesh::get_mfaces(size_t chan) const
		{
			Z_ASSERT( _ref->data.mchans[chan] );
			return _ref->data.mfaces + _ref->data.mchans[chan]->foffset;
		}
		mChan const * const *		Mesh::get_mchans() const
		{
			return _ref->data.mchans;
		}
		const SubMesh*				Mesh::get_submeshes() const
		{
			return _ref->data.submeshes;
		}
		size_t						Mesh::get_vert_count() const
		{
			return _ref->data.vert_count;
		}
		size_t						Mesh::get_norm_count() const
		{
			return _ref->data.norm_count;
		}
		size_t						Mesh::get_color_count() const
		{
			return _ref->data.color_count;
		}
		size_t						Mesh::get_mvert_count(size_t chan) const
		{
			Z_ASSERT( _ref->data.mchans[chan] );
			return _ref->data.mchans[chan]->vcount;
		}
		size_t						Mesh::get_face_count() const
		{
			return _ref->data.face_count;
		}
		size_t						Mesh::get_submesh_count() const
		{
			return _ref->data.submesh_count;
		}
		bool						Mesh::has_mchan(size_t chan) const
		{
			return !!_ref->data.mchans[chan];
		}
		size_t						Mesh::getSubMeshCount() const
		{
			return get_submesh_count();
		}
		const SubMesh&				Mesh::getSubMesh(size_t index) const
		{
			Z_ASSERT( index < get_submesh_count() );
			return get_submeshes()[index];
		}
		Vec3						Mesh::getFaceNormal(size_t triidx) const
		{
			const vnFace* faces = get_faces();
			const Vec3* verts = get_verts();

			return ((verts[faces[triidx].v[1]] - verts[faces[triidx].v[0]]) ^ (verts[faces[triidx].v[2]] - verts[faces[triidx].v[0]])).normal_safe();
		}

		const SPtr<VBO>&			Mesh::get_vbo_vert()
		{
			if(!data.verts.get().size())
				return _vbo.vert = _ref->vbo_vert();
			if(_vbo.vert)
			{
				if(!_vbo.vert->ver.sameGroup(_group))
					_vbo.vert.reset();
				else
				{
					if(_vbo.vert->ver >= data.verts.ver)
						return _vbo.vert;
				}
			}
			MeshHelper::__build_vbo_vert(_vbo.vert, get_face_count(), get_verts(), get_faces(), true);

			if(!_vbo.vert->ver.sameGroup(_group))
				_vbo.vert->ver.setGroup(_group);
			_vbo.vert->ver.update();

			return _vbo.vert;
		}
		const SPtr<VBO>&			Mesh::get_vbo_norm()
		{
			if(!data.norms.get().size())
				return _vbo.norm = _ref->vbo_norm();
			if(_vbo.norm)
			{
				if(!_vbo.norm->ver.sameGroup(_group))
					_vbo.norm.reset();
				else
				{
					if(_vbo.norm->ver >= data.norms.ver)
						return _vbo.norm;
				}
			}
			MeshHelper::__build_vbo_norm(_vbo.norm, get_face_count(), get_norms(), get_faces(), true);

			if(!_vbo.norm->ver.sameGroup(_group))
				_vbo.norm->ver.setGroup(_group);
			_vbo.norm->ver.update();

			return _vbo.norm;
		}
		const SPtr<VBO>&			Mesh::get_vbo_color()
		{
			return _vbo.color = _ref->vbo_color();
		}
		const SPtr<VBO>&			Mesh::get_vbo_mvert(size_t chan)
		{
			Z_ASSERT( chan < _countof(_vbo.mverts) );

			return _vbo.mverts[chan] = _ref->vbo_mvert(chan);
		}
		const SPtr<VBO>&			Mesh::get_vbo_tangent(size_t chan)
		{
			if(!data.verts.get().size() && !data.norms.get().size())
				return _vbo.tangents[chan] = _ref->vbo_tangent(chan);
			if(_vbo.tangents[chan])
			{
				if(!_vbo.tangents[chan]->ver.sameGroup(_group))
					_vbo.tangents[chan].reset();
			}
			buildTangents(chan);
			if(!data.tangents[chan].get().size())
			{
				_vbo.tangents[chan].reset();
				return _vbo.tangents[chan];
			}

			if(!_vbo.tangents[chan] || data.tangents[chan].ver >= _vbo.tangents[chan]->ver)
			{
				MeshHelper::__build_vbo_nm_tangent(_vbo.tangents[chan], get_face_count(), VECTOR_BEGIN_PTR(data.tangents[chan].get()), true);

				if(!_vbo.tangents[chan]->ver.sameGroup(_group))
					_vbo.tangents[chan]->ver.setGroup(_group);
				_vbo.tangents[chan]->ver.update();
			}

			return _vbo.tangents[chan];
		}
		void						Mesh::loadTextures()
		{
			size_t submesh_count = get_submesh_count();
			for(size_t i = 0; i < submesh_count; ++i)
			{
				const SubMesh& sub = get_submeshes()[i];
				Mtl& mtl = data.mtls.edit()[sub.mtl_index % data.mtls.get().size()];

				mtl.map_diffuse.loadTexture();
				if(mtl.sep_opacity_mapping && mtl.map_opacity.filepath.length() && mtl.map_opacity.filepath != mtl.map_diffuse.filepath)
					mtl.map_opacity.loadTexture();
				mtl.map_normal.loadTexture();
				mtl.map_selfill.loadTexture();
				mtl.map_specular.loadTexture();
				mtl.map_env.loadTexture();
			}
		}
		const AABB&					Mesh::local_bound()
		{
			if(!data.verts.get().size())
				return _ref->bound();
			if(_bound.ver >= data.verts.ver)
				return _bound.get();

			MeshHelper::__calc_bound(_bound.edit(), get_verts(), get_vert_count());
			return _bound.get();
		}
		const vector<INT3>&			Mesh::getFaceNeighborLookup()
		{
			return _ref->getFaceNeighborLookup();
		}
		const vector<Plane>&		Mesh::getFacePlane()
		{
			if(!data.verts.get().size())
				return _ref->getFacePlane();
			if(_face_plane.ver >= data.verts.ver)
				return _face_plane.get();

			MeshHelper::__buildFacePlane(_face_plane.edit(), get_verts(), get_faces(), get_face_count());

			return _face_plane.get();
		}
		const vector<TriangleAux>&	Mesh::getFaceTri()
		{
			if(_array_face_tri.ver >= data.verts.ver && _array_face_tri.get().size())
				return _array_face_tri.get();

			const Vec3* verts = get_verts();
			size_t vert_count = get_vert_count();
			const vnFace* faces = get_faces();
			size_t face_count = get_face_count();

			vector<TriangleAux>& a = _array_face_tri.edit();
			a.clear();

			const vnFace* face_end = faces + face_count;
			for(const vnFace* f = faces; f != face_end; ++f)
			{
				const vnFace& f2=*f;
				a.push_back(TriangleAux(verts[f2.v[0]], verts[f2.v[1]], verts[f2.v[2]]));
			}

			return _array_face_tri.get();
		}
		bool						Mesh::manifold_2()
		{
			return _ref->manifold_2();
		}
		const vfLookup&				Mesh::getVertexFaceLookup()
		{
			return _ref->getVertexFaceLookup();
		}
		const ConvexGroup&			Mesh::getConvexGroup()
		{
			if(!data.verts.get().size())
				return _ref->getConvexGroup();
			if(_convex_group.ver >= data.verts.ver)
				return _convex_group.get();

			_convex_group.edit().build(get_verts(), get_vert_count(), get_faces(), get_face_count(), VECTOR_BEGIN_PTR(getFaceNeighborLookup()));
			return _convex_group.get();
		}
		void						Mesh::buildTangents(size_t chan)
		{
			if(!data.verts.get().size() && !data.norms.get().size())
				return;
			else if(data.tangents[chan].get().size())
			{
				if((!data.verts.get().size() || (data.tangents[chan].ver >= data.verts.ver)) && (!data.norms.get().size() || (data.tangents[chan].ver >= data.norms.ver)))
					return;
			}

			vector<Vec4>& tangents = data.tangents[chan].edit();
			tangents.resize(get_face_count() * 3);

			MeshHelper::__build_nm_tangents(
				VECTOR_BEGIN_PTR(tangents),
				get_verts(),
				get_norms(),
				get_mverts(chan),
				get_faces(),
				get_face_count(),
				get_mfaces(chan),
				get_submeshes(),
				get_submesh_count(),
				getVertexFaceLookup()
				);
		}
	};
};



