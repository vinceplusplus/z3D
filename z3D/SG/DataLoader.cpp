
#include "DataLoader.h"

namespace z3D
{
	namespace SG
	{
		SkinData::SkinData(const wcs& filepath, const SMem& filemem): ResBase(filepath), _filemem(filemem)
		{
		}
		SkinData::~SkinData()
		{
		}
		SPtr<SkinData>			SkinData::load(const wcs& filepath)
		{
			if(true)
			{
				SkinData* cache = _res_mgr.find<SkinData>(filepath);
				if(cache)
					return cache;
			}

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			SPtr<SkinData> skindata = new SkinData(filepath, sm);
			if(!skindata->parse())
				return NULL;

			_res_mgr.add(skindata.get());

			return skindata;
		}
		bool					SkinData::parse()
		{
			MemReader reader(_filemem.get(), _filemem.size());

			if(true)
			{
				char header[4];
				reader.read(header, sizeof(header));
				Z_VERIFY( strncmp(header, "ZSKN", 4) == 0 );

				uint32_t version = reader.read<uint32_t>();
				Z_VERIFY( version == 1 );
			}

			size_t next_offset = 0;

			reader.read(&inv_init_world_tm, sizeof(inv_init_world_tm));
			vert_count = reader.read<uint32_t>();
			norm_count = reader.read<uint32_t>();

			next_offset = reader.read<uint32_t>();
			verts = (const VERT*)((const char*)reader.buffer() + reader.offset());

			reader.seek(next_offset);

			verts_offsets = (const int32_t*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + sizeof(int32_t) * vert_count);
			norms_offsets = (const int32_t*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + sizeof(int32_t) * norm_count);

			bone_count = reader.read<uint32_t>();
			array_dismount_tm = (const Mat4*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + sizeof(Mat4) * bone_count);
			array_bone_name = (const wchar_t*)((const char*)reader.buffer() + reader.offset());

			return true;
		}
		MorpherData::MorpherData(const wcs& filepath, const SMem& filemem)
			: ResBase(filepath), _filemem(filemem)
		{}
		MorpherData::~MorpherData()
		{
		}
		SPtr<MorpherData>				MorpherData::load(const wcs& filepath)
		{
			if(true)
			{
				MorpherData* cache = _res_mgr.find<MorpherData>(filepath);
				if(cache)
					return cache;
			}

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			SPtr<MorpherData> morpherdata = new MorpherData(filepath, sm);
			if(!morpherdata->parse())
				return NULL;

			_res_mgr.add(morpherdata.get());

			return morpherdata;
		}
		bool							MorpherData::parse()
		{
			MemReader reader(_filemem.get(), _filemem.size());

			if(true)
			{
				char header[4];
				reader.read(header, sizeof(header));
				Z_VERIFY( strncmp(header, "ZMPH", 4) == 0 );

				uint32_t version = reader.read<uint32_t>();
				Z_VERIFY( version == 1 );
			}

			vert_count = reader.read<uint32_t>();
			norm_count = reader.read<uint32_t>();

			channel_count = reader.read<uint32_t>();

			base_verts = (const Vec3*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + sizeof(Vec3) * vert_count);
			base_norms = (const Vec3*)((const char*)reader.buffer() + reader.offset());
			reader.seek(reader.offset() + sizeof(Vec3) * norm_count);

			for(size_t i = 0; i < channel_count; ++i)
			{
				chans_verts[i] = (const Vec3*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + sizeof(Vec3) * vert_count);
				chans_norms[i] = (const Vec3*)((const char*)reader.buffer() + reader.offset());
				reader.seek(reader.offset() + sizeof(Vec3) * norm_count);
			}

			return true;
		}
		AnimDataArchive::AnimDataArchive(const wcs& filepath): ResBase(filepath)
		{
		}
		AnimDataArchive::~AnimDataArchive()
		{
		}
		SPtr<AnimDataArchive>			AnimDataArchive::load(const wcs& filepath)
		{
			if(true)
			{
				AnimDataArchive* a = _res_mgr.find<AnimDataArchive>(filepath);
				if(a)
					return a;
			}

			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return NULL;

			SPtr<AnimDataArchive> a = new AnimDataArchive(filepath);
			a->_filemem = sm;

			size_t start_offset = 0;
			if(true)
			{
				MemReader reader(sm.get(), sm.size());

				char header[4];
				reader.read(header, sizeof(header));
				Z_VERIFY( strncmp(header, "ZANI", 4) == 0 );

				uint32_t version = reader.read<uint32_t>();
				Z_VERIFY( version == 1 );

				start_offset = reader.offset();
			}

			for(size_t i = start_offset; i < sm.size();)
			{
				if(i + 3 * sizeof(uint32_t) > sm.size())
					break;
				a->_array_chunk_offset.push_back(i);
				i += (size_t)(*(const uint32_t*)((const char*)sm.get() + i));
			}

			_res_mgr.add(a.get());

			return a;
		}
		const void*						AnimDataArchive::chunk(size_t chunk_idx) const
		{
			Z_ASSERT(chunk_idx < _array_chunk_offset.size());

			return (const char*)_filemem.get() + _array_chunk_offset[chunk_idx];
		}
	};
};

