
#pragma once

#include "predecl.h"

#pragma warning(push)
#pragma warning(disable:4200)

namespace z3D
{
	namespace SG
	{
		class SkinData: public ResBase
		{
		private:
			static ResMgr					_res_mgr;
		public:
			class VERT
			{
			public:
				class BONE_WEIGHT
				{
				public:
					uint32_t				b;
					REAL					w;
				};
			public:
				uint32_t					bone_weight_count;
				BONE_WEIGHT					bone_weights[0];
			private:
				VERT(const VERT&);
				VERT&						operator=(const VERT&);
			};
		private:
			SMem							_filemem;
		public:
			Mat4							inv_init_world_tm;
		public:
			size_t							vert_count;
			size_t							norm_count;
		public:
			const VERT*						verts;
			const int32_t*					verts_offsets;
			const int32_t*					norms_offsets;
		public:
			size_t							bone_count;
			const Mat4*						array_dismount_tm;
			const wchar_t*					array_bone_name;
		protected:
			SkinData(const wcs& filepath, const SMem& filemem);
			~SkinData();
		public:
			static SPtr<SkinData>			load(const wcs& filepath);
		private:
			bool							parse();
		};

		class MorpherData: public ResBase
		{
		private:
			static ResMgr					_res_mgr;
		private:
			SMem							_filemem;
		public:
			size_t							vert_count;
			size_t							norm_count;
		public:
			const Vec3*						base_verts;
			const Vec3*						base_norms;
		public:
			size_t							channel_count;
		public:
			const Vec3*						chans_verts[128];
			const Vec3*						chans_norms[128];
		protected:
			MorpherData(const wcs& filepath, const SMem& filemem);
			~MorpherData();
		public:
			static SPtr<MorpherData>		load(const wcs& filepath);
		private:
			bool							parse();
		};

		template<class T>
		class ChunkedAnimData: public AnimData<T>
		{
		protected:
			ChunkedAnimData(const wcs& name, const SPtr<RefBase>& const_opaque, const AnimKey<T>* const_keys, const uint8_t* const_keys_tangent, size_t const_key_count)
				: AnimData<T>(name, const_opaque, const_keys, const_keys_tangent, const_key_count)
			{}
			~ChunkedAnimData()
			{
				Z_ASSERT( getResMgr() );
				getResMgr()->remove(this);
			}
		public:
			friend class AnimDataArchive;
		};

		class AnimDataArchive: public ResBase
		{
		private:
			static ResMgr					_res_mgr;
		private:
			SMem							_filemem;
			vector<size_t>					_array_chunk_offset;
		private:
			ResMgr							_chunk_res_mgr;
		protected:
											AnimDataArchive(const wcs& filepath);
			virtual							~AnimDataArchive();
		public:
			static SPtr<AnimDataArchive>	load(const wcs& filepath);
		public:
			const void*						chunk(size_t chunk_idx) const;
		public:
			template<class T>
			SPtr< AnimData<T> >				getAnimData(size_t chunk_idx);
		public:
			template<class T>
			static SPtr< AnimData<T> >		loadAnimData(const wcs& filepath, size_t chunk_idx);
		private:
			template<class T>
			class TYPE_IDENTIFYING {};

			// must match with the max exporter
			uint32_t						getKeyTypeID(TYPE_IDENTIFYING<Vec3>) const				{	return 1;	}
			uint32_t						getKeyTypeID(TYPE_IDENTIFYING<Quat>) const				{	return 2;	}
			uint32_t						getKeyTypeID(TYPE_IDENTIFYING<REAL>) const				{	return 3;	}
			uint32_t						getKeyTypeID(TYPE_IDENTIFYING<Vec4>) const				{	return 4;	}
			template<class T>
			uint32_t						getKeyTypeID(T) const									{	Z_STATIC_ASSERT( false, WRONG_TYPE );	}
		};

		template<class T>
		SPtr< AnimData<T> >				AnimDataArchive::getAnimData(size_t chunk_idx)
		{
			wcs name = FORMATW(L"%I32d", (int32_t)chunk_idx);
			if(true)
			{
				AnimData<T>* a;				
				a = _chunk_res_mgr.find<AnimData<T>>(name);
				if(a)
					return a;
			}
			class CHUNK
			{
			public:
				uint32_t						chunk_size;
				uint32_t						key_type_id;
				uint32_t						key_count;
				uint8_t							data[0];
			};

			const CHUNK* c = (const CHUNK*)chunk(chunk_idx);
			Z_VERIFY( c->key_type_id == getKeyTypeID(TYPE_IDENTIFYING<T>()) );
			const uint8_t* tangents = ((c->chunk_size - 3 * sizeof(uint32_t) - sizeof(AnimKey<T>) * c->key_count) == 0) ? NULL : (uint8_t*)(((const AnimKey<T>*)c->data) + c->key_count);
			SPtr<AnimData<T>> a = new ChunkedAnimData<T>(name, this, (const AnimKey<T>*)c->data, tangents, c->key_count);
			_chunk_res_mgr.add(a.get());
			return a;
		}

		template<class T>
		SPtr< AnimData<T> >				AnimDataArchive::loadAnimData(const wcs& filepath, size_t chunk_idx)
		{
			SPtr<AnimDataArchive> archive = load(filepath);
			if(!archive)
				return NULL;
			return archive->getAnimData<T>(chunk_idx);
		}
	};
};

#pragma warning(pop)