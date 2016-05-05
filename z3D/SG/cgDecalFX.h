
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class cgDecalFXBase: public SNode
		{
			DECL_TYPEINFO();
		public:
			class IntrSet
			{
			public:
				class MeshRes
				{
				public:
					WPtr<SMeshNode>			mn;
					int32_t*				array_triidx;
					size_t					triidx_count;
				};
			private:
				vector<int32_t>			_array_triidx;
				vector<MeshRes>			_array_mesh_res;
			public:
				const vector<MeshRes>&	array_mesh_res;
			public:
				IntrSet();
			public:
				void					clear();
			public:
				friend class cgDecalFXBase;
			};
		private:
			AABB				_local_bound;
		public:
			cgDecalFXBase(const AABB& local_bound);
		protected:
			~cgDecalFXBase();
		public:
			const AABB&			local_bound();
		public:
			bool				renderable() const;
		public:
			void				getIntersectingSet(const AABB& search_box, const SPtr<SNode>& map_node, IntrSet& intr_set) const;
		public:
			static AABB			projAABBOnScreen(const AABB& box, const Mat4& viewproj);
		};

		class cgDecalFX: public cgDecalFXBase
		{
			DECL_TYPEINFO();
		public:
			class Item: public RefBase
			{
			public:
				class Piece
				{
				public:
					WPtr<SMeshNode>			mn;
					size_t					triidx_count;
					int32_t*				array_triidx;
					Vec2*					array_uv;
				};
			private:
				vector<Vec2>				_array_uv;
				vector<int32_t>				_array_triidx;
				vector<Piece>				_array_piece;
			public:
				Vec4						color;
			public:
				AABB						world_bound;
			public:
				friend class cgDecalFX;
			};
		private:
			vector<WPtr<Item>>				_array_item;
		private:
			SPtr<Tex2D>						_tex_decal;
		private:
			Jotter							_jotter;
		public:
			cgDecalFX(const AABB& local_bound);
		protected:
			~cgDecalFX();
		public:
			void							setDecalTexture(const SPtr<Tex2D>& tex);
			SPtr<Item>						create(const SPtr<SNode>& map_node, const Vec3& world_pos, const Vec3& world_dir, REAL radius, const Vec4& color);
		public:
			void							preRender(SRenderInfo* ri);
			void							render(intptr_t parameter, SRenderInfo* ri);
		};

		class cgPointLightFX: public cgDecalFXBase
		{
			DECL_TYPEINFO();
		public:
			class Item: public RefBase
			{
			private:
				cgDecalFXBase::IntrSet				_intr_set;
			public:
				Vec3								light_pos;
				REAL								light_radius;
				Vec4								color;
			public:
				friend class cgPointLightFX;
			};
		private:
			vector<WPtr<Item>>					_array_item;
		private:
			SPtr<Tex2D>							_tex_lookup;
		private:
			SPtr<Shader>						_vs;
			SPtr<Shader>						_ps;
		private:
			Jotter								_jotter;
		public:
			cgPointLightFX(const AABB& local_bound);
		protected:
			~cgPointLightFX();
		public:
			void							setLookupTexture(const SPtr<Tex2D>& tex);
			bool							loadShaders();
			SPtr<Item>						create(const SPtr<SNode>& map_node, const Vec3& world_pos, REAL radius, const Vec4& color);
		public:
			void							preRender(SRenderInfo* ri);
			void							render(intptr_t parameter, SRenderInfo* ri);
		};

	};
};


