
#include "Mesh.h"

#include "../Core/functor_inl.h"

#include "DataLoader.h"

namespace z3D
{
	namespace SG
	{
		bool					LoadedMtl::Loader::load(const wcs& filepath)
		{
			if(true)
			{
				XmlSax sax;
				mbs error;

				sax.setElementStartHandler(SaxBase::ELEMENT_START_HANDLER(this, &Loader::mtls_start));

				return sax.parse(filepath, error);
			}
		}
		void					LoadedMtl::Loader::mtls_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "mtl") == 0)
			{
				_m = Mtl();
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &Loader::mtl_start), SaxBase::ELEMENT_END_HANDLER(this, &Loader::mtl_end));
			}
		}
		void					LoadedMtl::Loader::mtl_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			SaxBase::AttribGetter getter;
			if(strcmp(name, "mtl") == 0)
			{
				const char* type = NULL;
				const char* method = NULL;

				getter.get("type", &type, false);
				getter.get("method", &method, false);

				parser->getAttribs(array_attrib, getter);

				Z_ASSERT( type && method );

				if(!_stricmp(type, "solid"))
					_m.type = Mtl::TYPE_SOLID;
				else if(!_stricmp(type, "translucent"))
					_m.type = Mtl::TYPE_TRANSPARENT;
				else if(!_stricmp(type, "transparent"))
					_m.type = Mtl::TYPE_TRANSPARENT;

				if(!_stricmp(method, "standard"))
					_m.method = Mtl::METHOD_STANDARD;
				else if(!_stricmp(method, "tone"))
					_m.method = Mtl::METHOD_TOON;

				return;
			}
			if(strcmp(name, "ambient") == 0)
				getter.get("value", &_m.ambient, false);
			else if(strcmp(name, "diffuse") == 0)
				getter.get("value", &_m.diffuse, false);
			else if(strcmp(name, "specular") == 0)
				getter.get("value", &_m.specular, false);
			else if(strcmp(name, "specular_level") == 0)
				getter.get("value", &_m.specular_level, false);
			else if(strcmp(name, "glossiness") == 0)
				getter.get("value", &_m.glossiness, false);
			else if(strcmp(name, "opacity") == 0)
				getter.get("value", &_m.opacity, false);
			else if(strcmp(name, "lighting") == 0)
				getter.get("value", &_m.lighting, false);
			else if(strcmp(name, "two_sided") == 0)
				getter.get("value", &_m.two_sided, false);
			else if(strcmp(name, "alpha_test") == 0)
			{
				const char* func = NULL;
				const char* enabled = NULL;
				getter.get("enabled", &enabled, false);
				getter.get("func", &func, false);
				getter.get("ref", &_m.alpha_test.ref, false);

				parser->getAttribs(array_attrib, getter);

				if(enabled)
				{
					if(strcmp(enabled, "AUTO") == 0)
						_m.alpha_test.enabled = 2;
					else
						_m.alpha_test.enabled = (uint32_t)strtoul(enabled, NULL, 10);
				}
				_m.alpha_test.func = getCmpFunc(func);

				return;
			}
			else if(strcmp(name, "blend") == 0)
			{
				const char* op = NULL;
				const char* sbf = NULL;
				const char* dbf = NULL;

				getter.get("enabled", &_m.blend.enabled, false);
				getter.get("op", &op, false);
				getter.get("sbf", &sbf, false);
				getter.get("dbf", &dbf, false);

				parser->getAttribs(array_attrib, getter);

				_m.blend.op = getBlendOperation(op);
				_m.blend.sbf = getBlendFactor(sbf);
				_m.blend.dbf = getBlendFactor(dbf);

				return;
			}
			else if(strcmp(name, "reflect") == 0)
				getter.get("value", &_m.reflect, false);
			else if(strcmp(name, "layer_index") == 0)
				getter.get("value", &_m.layer_index, false);
			else if(strcmp(name, "sep_opacity_mapping") == 0)
				getter.get("value", &_m.sep_opacity_mapping, false);
			else if(!strcmp(name, "diffuse_map") || !strcmp(name, "opacity_map") || !strcmp(name, "normal_map") || !strcmp(name, "selfill_map") || !strcmp(name, "specular_map") || !strcmp(name, "env_map"))
			{
				MtlMap mm;
				const char* min_filter = NULL;
				const char* mag_filter = NULL;
				const char* mip_filter = NULL;
				const char* u_wrap = NULL;
				const char* v_wrap = NULL;
				const char* w_wrap = NULL;
				getter.get("src", &mm.filepath, false);
				getter.get("channel", &mm.channel, false);
				getter.get("min_filter", &min_filter, false);
				getter.get("mag_filter", &mag_filter, false);
				getter.get("mip_filter", &mip_filter, false);
				getter.get("u_wrap", &u_wrap, false);
				getter.get("v_wrap", &v_wrap, false);
				getter.get("w_wrap", &w_wrap, false);
				getter.get("u_offset", &mm.uvgen.offset_u, false);
				getter.get("v_offset", &mm.uvgen.offset_v, false);
				getter.get("u_tiling", &mm.uvgen.tiling_u, false);
				getter.get("v_tiling", &mm.uvgen.tiling_v, false);
				getter.get("u_angle", &mm.uvgen.angle_u, false);
				getter.get("v_angle", &mm.uvgen.angle_v, false);
				getter.get("w_angle", &mm.uvgen.angle_w, false);

				parser->getAttribs(array_attrib, getter);

				mm.filter_min = getFilterMode(min_filter);
				mm.filter_mag = getFilterMode(mag_filter);
				mm.filter_mip = getFilterMode(mip_filter);
				mm.wrap_u = getWrapMode(u_wrap);
				mm.wrap_v = getWrapMode(v_wrap);
				mm.wrap_w = getWrapMode(w_wrap);

				if(!strcmp(name, "diffuse_map"))
					_mtlmap = &_m.map_diffuse;
				else if(!strcmp(name, "opacity_map"))
					_mtlmap = &_m.map_opacity;
				else if(!strcmp(name, "normal_map"))
					_mtlmap = &_m.map_normal;
				else if(!strcmp(name, "selfill_map"))
					_mtlmap = &_m.map_selfill;
				else if(!strcmp(name, "specular_map"))
					_mtlmap = &_m.map_specular;
				else if(!strcmp(name, "env_map"))
					_mtlmap = &_m.map_env;
				else
					_mtlmap = NULL;

				if(_mtlmap)
					*_mtlmap = mm;

				if(_mtlmap)
					parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &Loader::map_start));

				return;
			}
			else if(strcmp(name, "toon_shade") == 0)
			{
				getter.get("map_src", &_m.toon_shade.map_src, false);
				getter.get("outline_width", &_m.toon_shade.outline_width, false);
				getter.get("outline_color", &_m.toon_shade.outline_color, false);
			}
			else if(strcmp(name, "animdata") == 0)
			{
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &Loader::animdata_start));
				return;
			}
			else
			{
				//Z_ASSERT( false );
				return;
			}

			parser->getAttribs(array_attrib, getter);
		}
		void					LoadedMtl::Loader::mtl_end(SaxBase* parser, const char* name)
		{
			if(strcmp(name, "mtl") == 0)
				mtls.push_back(_m);
		}
		void					LoadedMtl::Loader::map_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "animdata") == 0)
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &Loader::map_animdata_start));
		}
		void					LoadedMtl::Loader::map_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(!_mtlmap)
				return;

			if(strcmp(name, "animdata") == 0)
				return;

			wcs src;
			uint32_t chunk_idx = 0;

			SaxBase::AttribGetter getter;
			getter.get("src", &src, false);
			getter.get("chunk", &chunk_idx, false);
			parser->getAttribs(array_attrib, getter);

			if(strcmp(name, "u_offset") == 0)
				_mtlmap->animdata.offset_u = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "v_offset") == 0)
				_mtlmap->animdata.offset_v = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "u_tiling") == 0)
				_mtlmap->animdata.tiling_u = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "v_tiling") == 0)
				_mtlmap->animdata.tiling_v = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "u_angle") == 0)
				_mtlmap->animdata.angle_u = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "v_angle") == 0)
				_mtlmap->animdata.angle_v = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "w_angle") == 0)
				_mtlmap->animdata.angle_w = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
		}
		void					LoadedMtl::Loader::animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			wcs src;
			uint32_t chunk_idx = 0;

			SaxBase::AttribGetter getter;
			getter.get("src", &src, false);
			getter.get("chunk", &chunk_idx, false);
			parser->getAttribs(array_attrib, getter);

			if(strcmp(name, "ambient") == 0)
				_m.animdata.ambient = AnimDataArchive::loadAnimData<Vec3>(src, chunk_idx);
			if(strcmp(name, "diffuse") == 0)
				_m.animdata.diffuse = AnimDataArchive::loadAnimData<Vec3>(src, chunk_idx);
			if(strcmp(name, "specular") == 0)
				_m.animdata.specular = AnimDataArchive::loadAnimData<Vec3>(src, chunk_idx);
			if(strcmp(name, "specular_level") == 0)
				_m.animdata.specular_level = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			if(strcmp(name, "glossiness") == 0)
				_m.animdata.glossiness = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			if(strcmp(name, "opacity") == 0)
				_m.animdata.opacity = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
		}
		GfxEnum::CMPFUNC		LoadedMtl::Loader::getCmpFunc(const char* s)
		{
			if(!s)
				return GfxEnum::CMPFUNC_ALWAYS;

			if(_stricmp(s, "NEVER") == 0)
				return GfxEnum::CMPFUNC_NEVER;
			if(_stricmp(s, "LESS") == 0)
				return GfxEnum::CMPFUNC_LESS;
			if(_stricmp(s, "EQUAL") == 0)
				return GfxEnum::CMPFUNC_EQUAL;
			if(_stricmp(s, "LEQUAL") == 0)
				return GfxEnum::CMPFUNC_LEQUAL;
			if(_stricmp(s, "GREATER") == 0)
				return GfxEnum::CMPFUNC_GREATER;
			if(_stricmp(s, "NOTEQUAL") == 0)
				return GfxEnum::CMPFUNC_NOTEQUAL;
			if(_stricmp(s, "GEQUAL") == 0)
				return GfxEnum::CMPFUNC_GEQUAL;
			if(_stricmp(s, "ALWAYS") == 0)
				return GfxEnum::CMPFUNC_ALWAYS;

			return GfxEnum::CMPFUNC_ALWAYS;
		}
		GfxEnum::BLENDOPERATION	LoadedMtl::Loader::getBlendOperation(const char* s)
		{
			if(!s)
				return GfxEnum::BLENDOPERATION_ADD;

			if(!_stricmp(s, "ADD"))
				return GfxEnum::BLENDOPERATION_ADD;
			if(!_stricmp(s, "SUBTRACT"))
				return GfxEnum::BLENDOPERATION_SUBTRACT;
			if(!_stricmp(s, "REVERSE_SUBTRACT"))
				return GfxEnum::BLENDOPERATION_REVERSE_SUBTRACT;
			if(!_stricmp(s, "MIN"))
				return GfxEnum::BLENDOPERATION_MIN;
			if(!_stricmp(s, "MAX"))
				return GfxEnum::BLENDOPERATION_MAX;

			return GfxEnum::BLENDOPERATION_ADD;
		}
		GfxEnum::BLENDFACTOR	LoadedMtl::Loader::getBlendFactor(const char* s)
		{
			if(!s)
				return GfxEnum::BLENDFACTOR_ONE;

			if(!_stricmp(s, "ZERO"))
				return GfxEnum::BLENDFACTOR_ZERO;
			if(!_stricmp(s, "ONE"))
				return GfxEnum::BLENDFACTOR_ONE;
			if(!_stricmp(s, "SRC_COLOR"))
				return GfxEnum::BLENDFACTOR_SRC_COLOR;
			if(!_stricmp(s, "ONE_MINUS_SRC_COLOR"))
				return GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_COLOR;
			if(!_stricmp(s, "SRC_ALPHA"))
				return GfxEnum::BLENDFACTOR_SRC_ALPHA;
			if(!_stricmp(s, "ONE_MINUS_SRC_ALPHA"))
				return GfxEnum::BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
			if(!_stricmp(s, "DEST_ALPHA"))
				return GfxEnum::BLENDFACTOR_DEST_ALPHA;
			if(!_stricmp(s, "ONE_MINUS_DEST_ALPHA"))
				return GfxEnum::BLENDFACTOR_ONE_MINUS_DEST_ALPHA;
			if(!_stricmp(s, "DEST_COLOR"))
				return GfxEnum::BLENDFACTOR_DEST_COLOR;
			if(!_stricmp(s, "ONE_MINUS_DEST_COLOR"))
				return GfxEnum::BLENDFACTOR_ONE_MINUS_DEST_COLOR;
			if(!_stricmp(s, "SRC_ALPHA_SAT"))
				return GfxEnum::BLENDFACTOR_SRC_ALPHA_SAT;

			return GfxEnum::BLENDFACTOR_ONE;
		}
		GfxEnum::FILTERMODE		LoadedMtl::Loader::getFilterMode(const char* s)
		{
			if(!s)
				return GfxEnum::FILTERMODE_LINEAR;

			if(!_stricmp(s, "NONE"))
				return GfxEnum::FILTERMODE_NONE;
			if(!_stricmp(s, "NEAREST"))
				return GfxEnum::FILTERMODE_NEAREST;
			if(!_stricmp(s, "LINEAR"))
				return GfxEnum::FILTERMODE_LINEAR;
			if(!_stricmp(s, "ANISOTROPIC"))
				return GfxEnum::FILTERMODE_ANISOTROPIC;

			return GfxEnum::FILTERMODE_LINEAR;
		}
		GfxEnum::WRAPMODE		LoadedMtl::Loader::getWrapMode(const char* s)
		{
			if(!s)
				return GfxEnum::WRAPMODE_REPEAT;

			if(!_stricmp(s, "REPEAT"))
				return GfxEnum::WRAPMODE_REPEAT;
			if(!_stricmp(s, "CLAMP"))
				return GfxEnum::WRAPMODE_CLAMP;
			if(!_stricmp(s, "MIRROR"))
				return GfxEnum::WRAPMODE_MIRROR;
			if(!_stricmp(s, "MIRROR_ONCE"))
				return GfxEnum::WRAPMODE_MIRROR_ONCE;

			return GfxEnum::WRAPMODE_REPEAT;
		}

		LoadedMtl::LoadedMtl(const wcs& filepath): ResBase(filepath) {}
		SPtr<LoadedMtl>			LoadedMtl::load(const wcs& filepath)
		{
			if(true)
			{
				LoadedMtl* m = _res_mgr.find<LoadedMtl>(filepath);
				if(m)
					return m;
			}

			Loader loader;
			if(!loader.load(filepath))
				return NULL;

			SPtr<LoadedMtl> mtl = new LoadedMtl(filepath);
			mtl->mtls = loader.mtls;

			_res_mgr.add(mtl.get());

			return mtl;
		}
		//void					Mesh::parseMtl(const string& utf8_str,vector<Mtl>& array_mtl/*,vector<MtlAnimDataSet>& array_mtl_animdata_set*/)
		//{
		//	int cmd_no=0;
		//	map<wstring,int> cmd_map;
		//	vector<wstring> cmd_param_map;

		//	cmd_map[L"resize_mtls"]=++cmd_no;
		//	cmd_map[L"set_mtl_render_type"]=++cmd_no;
		//	cmd_map[L"set_mtl_render_method"]=++cmd_no;
		//	cmd_map[L"set_mtl_ambient"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse"]=++cmd_no;
		//	cmd_map[L"set_mtl_specular"]=++cmd_no;
		//	cmd_map[L"set_mtl_specular_level"]=++cmd_no;
		//	cmd_map[L"set_mtl_glossiness"]=++cmd_no;
		//	cmd_map[L"set_mtl_two_side"]=++cmd_no;
		//	cmd_map[L"set_mtl_blend_type"]=++cmd_no;
		//	cmd_map[L"set_mtl_opacity"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map"]=++cmd_no;
		//	cmd_map[L"set_mtl_toon_map"]=++cmd_no;
		//	cmd_map[L"set_mtl_toon_outline"]=++cmd_no;
		//	cmd_map[L"set_mtl_toon_outline_width"]=++cmd_no;
		//	cmd_map[L"set_mtl_toon_outline_color"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_filter"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_filter"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_filter"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_wrap"]=++cmd_no;
		//	cmd_map[L"set_mtl_reflect_amount"]=++cmd_no;
		//	cmd_map[L"set_mtl_layer_index"]=++cmd_no;
		//	cmd_map[L"create_am_controller"]=++cmd_no;
		//	cmd_map[L"create_di_controller"]=++cmd_no;
		//	cmd_map[L"create_sp_controller"]=++cmd_no;
		//	cmd_map[L"create_sl_controller"]=++cmd_no;
		//	cmd_map[L"create_gl_controller"]=++cmd_no;
		//	cmd_map[L"create_op_controller"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_w_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_w_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_offset"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_tiling"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_w_angle"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_u_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_v_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_diffuse_map_w_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_u_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_v_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_normal_map_w_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_offset_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_tiling_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_u_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_v_angle_animdata"]=++cmd_no;
		//	cmd_map[L"set_mtl_selfill_map_w_angle_animdata"]=++cmd_no;

		//	int cmd_resize_mtls=cmd_map[L"resize_mtls"];
		//	int cmd_set_mtl_render_type=cmd_map[L"set_mtl_render_type"];
		//	int cmd_set_mtl_render_method=cmd_map[L"set_mtl_render_method"];
		//	int cmd_set_mtl_ambient=cmd_map[L"set_mtl_ambient"];
		//	int cmd_set_mtl_diffuse=cmd_map[L"set_mtl_diffuse"];
		//	int cmd_set_mtl_specular=cmd_map[L"set_mtl_specular"];
		//	int cmd_set_mtl_specular_level=cmd_map[L"set_mtl_specular_level"];
		//	int cmd_set_mtl_glossiness=cmd_map[L"set_mtl_glossiness"];
		//	int cmd_set_mtl_two_side=cmd_map[L"set_mtl_two_side"];
		//	int cmd_set_mtl_blend_type=cmd_map[L"set_mtl_blend_type"];
		//	int cmd_set_mtl_opacity=cmd_map[L"set_mtl_opacity"];
		//	int cmd_set_mtl_diffuse_map=cmd_map[L"set_mtl_diffuse_map"];
		//	int cmd_set_mtl_normal_map=cmd_map[L"set_mtl_normal_map"];
		//	int cmd_set_mtl_selfill_map=cmd_map[L"set_mtl_selfill_map"];
		//	int cmd_set_mtl_toon_map=cmd_map[L"set_mtl_toon_map"];
		//	int cmd_set_mtl_toon_outline=cmd_map[L"set_mtl_toon_outline"];
		//	int cmd_set_mtl_toon_outline_width=cmd_map[L"set_mtl_toon_outline_width"];
		//	int cmd_set_mtl_toon_outline_color=cmd_map[L"set_mtl_toon_outline_color"];
		//	int cmd_set_mtl_selfill=cmd_map[L"set_mtl_selfill"];
		//	int cmd_set_mtl_diffuse_map_filter=cmd_map[L"set_mtl_diffuse_map_filter"];
		//	int cmd_set_mtl_diffuse_map_u_wrap=cmd_map[L"set_mtl_diffuse_map_u_wrap"];
		//	int cmd_set_mtl_diffuse_map_v_wrap=cmd_map[L"set_mtl_diffuse_map_v_wrap"];
		//	int cmd_set_mtl_normal_map_filter=cmd_map[L"set_mtl_normal_map_filter"];
		//	int cmd_set_mtl_normal_map_u_wrap=cmd_map[L"set_mtl_normal_map_u_wrap"];
		//	int cmd_set_mtl_normal_map_v_wrap=cmd_map[L"set_mtl_normal_map_v_wrap"];
		//	int cmd_set_mtl_selfill_map_filter=cmd_map[L"set_mtl_selfill_map_filter"];
		//	int cmd_set_mtl_selfill_map_u_wrap=cmd_map[L"set_mtl_selfill_map_u_wrap"];
		//	int cmd_set_mtl_selfill_map_v_wrap=cmd_map[L"set_mtl_selfill_map_v_wrap"];
		//	int cmd_set_mtl_reflect_amount=cmd_map[L"set_mtl_reflect_amount"];
		//	int cmd_set_mtl_layer_index=cmd_map[L"set_mtl_layer_index"];
		//	int cmd_create_am_controller=cmd_map[L"create_am_controller"];
		//	int cmd_create_di_controller=cmd_map[L"create_di_controller"];
		//	int cmd_create_sp_controller=cmd_map[L"create_sp_controller"];
		//	int cmd_create_sl_controller=cmd_map[L"create_sl_controller"];
		//	int cmd_create_gl_controller=cmd_map[L"create_gl_controller"];
		//	int cmd_create_op_controller=cmd_map[L"create_op_controller"];
		//	int cmd_set_mtl_diffuse_map_u_offset=cmd_map[L"set_mtl_diffuse_map_u_offset"];
		//	int cmd_set_mtl_diffuse_map_v_offset=cmd_map[L"set_mtl_diffuse_map_v_offset"];
		//	int cmd_set_mtl_diffuse_map_u_tiling=cmd_map[L"set_mtl_diffuse_map_u_tiling"];
		//	int cmd_set_mtl_diffuse_map_v_tiling=cmd_map[L"set_mtl_diffuse_map_v_tiling"];
		//	int cmd_set_mtl_diffuse_map_u_angle=cmd_map[L"set_mtl_diffuse_map_u_angle"];
		//	int cmd_set_mtl_diffuse_map_v_angle=cmd_map[L"set_mtl_diffuse_map_v_angle"];
		//	int cmd_set_mtl_diffuse_map_w_angle=cmd_map[L"set_mtl_diffuse_map_w_angle"];
		//	int cmd_set_mtl_normal_map_u_offset=cmd_map[L"set_mtl_normal_map_u_offset"];
		//	int cmd_set_mtl_normal_map_v_offset=cmd_map[L"set_mtl_normal_map_v_offset"];
		//	int cmd_set_mtl_normal_map_u_tiling=cmd_map[L"set_mtl_normal_map_u_tiling"];
		//	int cmd_set_mtl_normal_map_v_tiling=cmd_map[L"set_mtl_normal_map_v_tiling"];
		//	int cmd_set_mtl_normal_map_u_angle=cmd_map[L"set_mtl_normal_map_u_angle"];
		//	int cmd_set_mtl_normal_map_v_angle=cmd_map[L"set_mtl_normal_map_v_angle"];
		//	int cmd_set_mtl_normal_map_w_angle=cmd_map[L"set_mtl_normal_map_w_angle"];
		//	int cmd_set_mtl_selfill_map_u_offset=cmd_map[L"set_mtl_selfill_map_u_offset"];
		//	int cmd_set_mtl_selfill_map_v_offset=cmd_map[L"set_mtl_selfill_map_v_offset"];
		//	int cmd_set_mtl_selfill_map_u_tiling=cmd_map[L"set_mtl_selfill_map_u_tiling"];
		//	int cmd_set_mtl_selfill_map_v_tiling=cmd_map[L"set_mtl_selfill_map_v_tiling"];
		//	int cmd_set_mtl_selfill_map_u_angle=cmd_map[L"set_mtl_selfill_map_u_angle"];
		//	int cmd_set_mtl_selfill_map_v_angle=cmd_map[L"set_mtl_selfill_map_v_angle"];
		//	int cmd_set_mtl_selfill_map_w_angle=cmd_map[L"set_mtl_selfill_map_w_angle"];
		//	int cmd_set_mtl_diffuse_map_u_offset_animdata=cmd_map[L"set_mtl_diffuse_map_u_offset_animdata"];
		//	int cmd_set_mtl_diffuse_map_v_offset_animdata=cmd_map[L"set_mtl_diffuse_map_v_offset_animdata"];
		//	int cmd_set_mtl_diffuse_map_u_tiling_animdata=cmd_map[L"set_mtl_diffuse_map_u_tiling_animdata"];
		//	int cmd_set_mtl_diffuse_map_v_tiling_animdata=cmd_map[L"set_mtl_diffuse_map_v_tiling_animdata"];
		//	int cmd_set_mtl_diffuse_map_u_angle_animdata=cmd_map[L"set_mtl_diffuse_map_u_angle_animdata"];
		//	int cmd_set_mtl_diffuse_map_v_angle_animdata=cmd_map[L"set_mtl_diffuse_map_v_angle_animdata"];
		//	int cmd_set_mtl_diffuse_map_w_angle_animdata=cmd_map[L"set_mtl_diffuse_map_w_angle_animdata"];
		//	int cmd_set_mtl_normal_map_u_offset_animdata=cmd_map[L"set_mtl_normal_map_u_offset_animdata"];
		//	int cmd_set_mtl_normal_map_v_offset_animdata=cmd_map[L"set_mtl_normal_map_v_offset_animdata"];
		//	int cmd_set_mtl_normal_map_u_tiling_animdata=cmd_map[L"set_mtl_normal_map_u_tiling_animdata"];
		//	int cmd_set_mtl_normal_map_v_tiling_animdata=cmd_map[L"set_mtl_normal_map_v_tiling_animdata"];
		//	int cmd_set_mtl_normal_map_u_angle_animdata=cmd_map[L"set_mtl_normal_map_u_angle_animdata"];
		//	int cmd_set_mtl_normal_map_v_angle_animdata=cmd_map[L"set_mtl_normal_map_v_angle_animdata"];
		//	int cmd_set_mtl_normal_map_w_angle_animdata=cmd_map[L"set_mtl_normal_map_w_angle_animdata"];
		//	int cmd_set_mtl_selfill_map_u_offset_animdata=cmd_map[L"set_mtl_selfill_map_u_offset_animdata"];
		//	int cmd_set_mtl_selfill_map_v_offset_animdata=cmd_map[L"set_mtl_selfill_map_v_offset_animdata"];
		//	int cmd_set_mtl_selfill_map_u_tiling_animdata=cmd_map[L"set_mtl_selfill_map_u_tiling_animdata"];
		//	int cmd_set_mtl_selfill_map_v_tiling_animdata=cmd_map[L"set_mtl_selfill_map_v_tiling_animdata"];
		//	int cmd_set_mtl_selfill_map_u_angle_animdata=cmd_map[L"set_mtl_selfill_map_u_angle_animdata"];
		//	int cmd_set_mtl_selfill_map_v_angle_animdata=cmd_map[L"set_mtl_selfill_map_v_angle_animdata"];
		//	int cmd_set_mtl_selfill_map_w_angle_animdata=cmd_map[L"set_mtl_selfill_map_w_angle_animdata"];

		//	cmd_param_map.resize(cmd_map.size()+1);

		//	cmd_param_map[cmd_resize_mtls]=L"d";
		//	cmd_param_map[cmd_set_mtl_render_type]=L"ds";
		//	cmd_param_map[cmd_set_mtl_render_method]=L"ds";
		//	cmd_param_map[cmd_set_mtl_ambient]=L"dfff";
		//	cmd_param_map[cmd_set_mtl_diffuse]=L"dfff";
		//	cmd_param_map[cmd_set_mtl_specular]=L"dfff";
		//	cmd_param_map[cmd_set_mtl_specular_level]=L"df";
		//	cmd_param_map[cmd_set_mtl_glossiness]=L"df";
		//	cmd_param_map[cmd_set_mtl_two_side]=L"dd";
		//	cmd_param_map[cmd_set_mtl_blend_type]=L"ds";
		//	cmd_param_map[cmd_set_mtl_opacity]=L"df";
		//	cmd_param_map[cmd_set_mtl_diffuse_map]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map]=L"ds";
		//	cmd_param_map[cmd_set_mtl_toon_map]=L"ds";
		//	cmd_param_map[cmd_set_mtl_toon_outline]=L"dd";
		//	cmd_param_map[cmd_set_mtl_toon_outline_width]=L"df";
		//	cmd_param_map[cmd_set_mtl_toon_outline_color]=L"dfff";
		//	cmd_param_map[cmd_set_mtl_selfill]=L"df";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_filter]=L"dd";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_normal_map_filter]=L"dd";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_selfill_map_filter]=L"dd";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_wrap]=L"dd";
		//	cmd_param_map[cmd_set_mtl_reflect_amount]=L"df";
		//	cmd_param_map[cmd_set_mtl_layer_index]=L"dd";
		//	cmd_param_map[cmd_create_am_controller]=L"sds";
		//	cmd_param_map[cmd_create_di_controller]=L"sds";
		//	cmd_param_map[cmd_create_sp_controller]=L"sds";
		//	cmd_param_map[cmd_create_sl_controller]=L"sds";
		//	cmd_param_map[cmd_create_gl_controller]=L"sds";
		//	cmd_param_map[cmd_create_op_controller]=L"sds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_w_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_normal_map_w_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_offset]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_tiling]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_selfill_map_w_angle]=L"dn";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_u_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_v_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_diffuse_map_w_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_u_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_v_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_normal_map_w_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_offset_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_tiling_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_u_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_v_angle_animdata]=L"ds";
		//	cmd_param_map[cmd_set_mtl_selfill_map_w_angle_animdata]=L"ds";

		//	Tokenizer tokenizer(utf8_str);
		//	int token_idx;
		//	for(token_idx=0;token_idx<tokenizer.getTokenCount();)
		//	{
		//		if(tokenizer.getTokenType(token_idx)!=SCN_TOKEN_IDENTIFIER)
		//		{
		//			++token_idx;
		//			continue;
		//		}

		//		int cmd=cmd_map[tokenizer.getTokenWString(token_idx)];
		//		const wstring& cmd_param=cmd_param_map[cmd];

		//		if(!cmd||!tokenizer.matchPattern(token_idx+1,cmd_param))
		//		{
		//			// unrecognized command or parameter pattern

		//			++token_idx;
		//			continue;
		//		}
		//		if(cmd==cmd_resize_mtls)
		//		{
		//			array_mtl.resize(tokenizer.getTokenInt(token_idx+1));
		//		}
		//		else if(cmd==cmd_set_mtl_render_type)
		//		{
		//			wstring s=TOLOWER(tokenizer.getTokenWString(token_idx+2));
		//			if(s==L"unlit")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_type = Mtl::RENDERTYPE_UNLIT;
		//			else if(s==L"lit")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_type = Mtl::RENDERTYPE_LIT;
		//			else if(s==L"transparent")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_type = Mtl::RENDERTYPE_TRANSPARENT;
		//			else if(s==L"reflection")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_type = Mtl::RENDERTYPE_REFLECTION;
		//		}
		//		else if(cmd==cmd_set_mtl_render_method)
		//		{
		//			wstring s=TOLOWER(tokenizer.getTokenWString(token_idx+2));
		//			if(s==L"unlit")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_UNLIT;
		//			else if(s==L"lit")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_LIT;
		//			else if(s==L"selfill")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_SELFILL;
		//			else if(s==L"toon")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_TOON;
		//			else if(s==L"reflection")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_REFLECTION;
		//			else if(s==L"normal_mapping")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_NORMAL_MAPPING;
		//			else if(s==L"sphere_env_mapping")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].render_method = Mtl::RENDERMETHOD_SPHERE_ENV_MAPPING;
		//		}
		//		else if(cmd==cmd_set_mtl_ambient)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].ambient=
		//				Vec3(tokenizer.getTokenFloat(token_idx+2),tokenizer.getTokenFloat(token_idx+3),tokenizer.getTokenFloat(token_idx+4));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].diffuse=
		//				Vec3(tokenizer.getTokenFloat(token_idx+2),tokenizer.getTokenFloat(token_idx+3),tokenizer.getTokenFloat(token_idx+4));
		//		}
		//		else if(cmd==cmd_set_mtl_specular)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].specular=
		//				Vec3(tokenizer.getTokenFloat(token_idx+2),tokenizer.getTokenFloat(token_idx+3),tokenizer.getTokenFloat(token_idx+4));
		//		}
		//		else if(cmd==cmd_set_mtl_specular_level)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].specular_level=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_glossiness)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].glossiness=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_two_side)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].two_side=tokenizer.getTokenInt(token_idx+2)?true:false;
		//		}
		//		else if(cmd==cmd_set_mtl_blend_type)
		//		{
		//			wstring s=TOLOWER(tokenizer.getTokenWString(token_idx+2));
		//			if(s==L"filter")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].blend_type=Mtl::BLENDTYPE_FILTER;
		//			else if(s==L"additive")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].blend_type=Mtl::BLENDTYPE_ADDITIVE;
		//			else if(s==L"subtractive")
		//				array_mtl[tokenizer.getTokenInt(token_idx+1)].blend_type=Mtl::BLENDTYPE_SUBTRACTIVE;
		//		}
		//		else if(cmd==cmd_set_mtl_opacity)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].opacity=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.filepath=tokenizer.getTokenWString(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.filepath=tokenizer.getTokenWString(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.filepath=tokenizer.getTokenWString(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_toon_map)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].toon_map_filepath=tokenizer.getTokenWString(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_toon_outline)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].toon_outline=!!tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_toon_outline_width)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].toon_outline_width=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_toon_outline_color)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].toon_outline_color=
		//				Vec3(tokenizer.getTokenFloat(token_idx+2),tokenizer.getTokenFloat(token_idx+3),tokenizer.getTokenFloat(token_idx+4));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_filter)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.filter=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.wrap_u=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.wrap_v=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_filter)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.filter=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.wrap_u=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.wrap_v=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_filter)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.filter=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.wrap_u=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_wrap)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.wrap_v=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_reflect_amount)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].reflect_amt=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_layer_index)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].layer_index=tokenizer.getTokenInt(token_idx+2);
		//		}
		//		else if(cmd==cmd_create_am_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_ambient = DataLoader::loadKeyframeVec3(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_ambient->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_create_di_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_diffuse = DataLoader::loadKeyframeVec3(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_diffuse->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_create_sp_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_specular = DataLoader::loadKeyframeVec3(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_specular->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_create_sl_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_specular_level = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_specular_level->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_create_gl_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_glossiness = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_glossiness->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_create_op_controller)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_opacity = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+3));
		//			array_mtl[tokenizer.getTokenInt(token_idx+2)].animdata_opacity->setSuggestedControllerName(tokenizer.getTokenWString(token_idx+1));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.offset_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.offset_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.tiling_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.tiling_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.angle_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.angle_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_w_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.angle_w=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.offset_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.offset_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.tiling_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.tiling_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.angle_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.angle_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_w_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.angle_w=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.offset_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_offset)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.offset_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.tiling_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_tiling)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.tiling_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.angle_u=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.angle_v=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_w_angle)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.angle_w=tokenizer.getTokenFloat(token_idx+2);
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.offset_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.offset_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.tiling_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.tiling_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_u_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.angle_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_v_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.angle_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_diffuse_map_w_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_diffuse.animdata.angle_w = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.offset_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.offset_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.tiling_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.tiling_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_u_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.angle_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_v_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.angle_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_normal_map_w_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_normal.animdata.angle_w = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.offset_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_offset_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.offset_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.tiling_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_tiling_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.tiling_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_u_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.angle_u = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_v_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.angle_v = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		else if(cmd==cmd_set_mtl_selfill_map_w_angle_animdata)
		//		{
		//			array_mtl[tokenizer.getTokenInt(token_idx+1)].map_selfill.animdata.angle_w = DataLoader::loadKeyframeREAL(tokenizer.getTokenWString(token_idx+2));
		//		}
		//		token_idx+=(int)cmd_param.length()+1;
		//	}
		//}
		//void				Mesh::parseMeshBin(MemReader& reader)
		//{
		//	/*
		//	array_vert.resize(reader.read<uint32_t>());
		//	if(array_vert.size())
		//		reader.read(&*array_vert.begin(),(int)(sizeof(Vec3)*array_vert.size()));

		//	array_norm.resize(reader.read<uint32_t>());
		//	if(array_norm.size())
		//		reader.read(&*array_norm.begin(),(int)(sizeof(Vec3)*array_norm.size()));

		//	array_mvert.resize(reader.read<uint32_t>());
		//	if(array_mvert.size())
		//		reader.read(&*array_mvert.begin(),(int)(sizeof(Vec2)*array_mvert.size()));

		//	array_face.resize(reader.read<uint32_t>());
		//	if(array_face.size())
		//		reader.read(&*array_face.begin(),(int)(sizeof(vnFace)*array_face.size()));

		//	array_mface.resize(reader.read<uint32_t>());
		//	if(array_mface.size())
		//		reader.read(&*array_mface.begin(),(int)(sizeof(mFace)*array_mface.size()));

		//	array_submesh.resize(reader.read<uint32_t>());
		//	for(int i=0;i<(int)array_submesh.size();++i)
		//	{
		//		SubMesh s;
		//		reader.read(&s,sizeof(s));
		//		*(SubMesh*)&array_submesh[i]=s;
		//	}

		//	reader.seek(0);
		//	*/

		//	_data.vert.resize(reader.read<uint32_t>());
		//	if(_data.vert.size())
		//		reader.read(&*_data.vert.begin(),(int)(sizeof(_data.vert[0])*_data.vert.size()));

		//	_data.norm.resize(reader.read<uint32_t>());
		//	if(_data.norm.size())
		//		reader.read(&*_data.norm.begin(),(int)(sizeof(_data.norm[0])*_data.norm.size()));

		//	_data.mvert.resize(reader.read<uint32_t>());
		//	if(_data.mvert.size())
		//		reader.read(&*_data.mvert.begin(),(int)(sizeof(_data.mvert[0])*_data.mvert.size()));

		//	_data.face.resize(reader.read<uint32_t>());
		//	if(_data.face.size())
		//		reader.read(&*_data.face.begin(),(int)(sizeof(_data.face[0])*_data.face.size()));

		//	_data.mface.resize(reader.read<uint32_t>());
		//	if(_data.mface.size())
		//		reader.read(&*_data.mface.begin(),(int)(sizeof(_data.mface[0])*_data.mface.size()));

		//	_data.submesh.resize(reader.read<uint32_t>());
		//	if(_data.submesh.size())
		//		reader.read(&*_data.submesh.begin(),(int)(sizeof(_data.submesh[0])*_data.submesh.size()));

		//	vert.ver.update();
		//	norm.ver.update();
		//	mvert.ver.update();
		//	face.ver.update();
		//	mface.ver.update();
		//}
	};
};

