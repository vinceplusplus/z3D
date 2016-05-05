
#define _CRT_SECURE_NO_WARNINGS 1

#include "ShaderGenConf.h"

namespace z3D
{
	namespace Video
	{
		//void								ShaderGenConf::begin(const Mat4& world, const Mat4& view, const Mat4& unadjusted_proj)
		void								ShaderGenConf::begin(const Mat4& world)
		{
			memcpy(this->world, &world, sizeof(world));
			//memcpy(this->view, &view, sizeof(view));
			//memcpy(this->unadjusted_proj, &unadjusted_proj, sizeof(unadjusted_proj));

			lights = 0;

			memset(array_tc, 0, sizeof(array_tc));
			memset(array_sampler, 0, sizeof(array_sampler));
			tex_fetches = 0;

			*_ps_code = 0;
			_ps_code_end = _ps_code;

			__ENABLE = 0;
		}
		void								ShaderGenConf::setMaterial(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, REAL shininess, REAL opacity)
		{
			memcpy(material.ambient, &ambient, sizeof(ambient));
			memcpy(material.diffuse, &diffuse, sizeof(diffuse));
			memcpy(material.specular, &specular, sizeof(specular));
			material.shininess = shininess;
			material.opacity = opacity;
		}
		void								ShaderGenConf::setMaterialAmbient(const Vec3& ambient, REAL opacity)
		{
			memcpy(material.ambient, &ambient, sizeof(ambient));
			material.opacity = opacity;

			memset(material.diffuse, 0, sizeof(material.diffuse));
			memset(material.specular, 0, sizeof(material.specular));
			material.shininess = 0;
		}
		void								ShaderGenConf::setLightAmbient(const Vec3& light_ambient)
		{
			memcpy(this->light_ambient, &light_ambient, sizeof(light_ambient));
		}
		void								ShaderGenConf::addDirLight(const Vec3& color, const Vec3& dir, ptrdiff_t visibility_channel)
		{
			if(lights >= MAX_LIGHTS)
				return;

			LIGHT& l = array_light[lights];

			l.type = GfxEnum::LIGHTTYPE_DIRECTIONAL;
			memcpy(l.color, &color, sizeof(color));
			memcpy(l.dir, &dir, sizeof(dir));
			l.visibility_channel = visibility_channel;

			++lights;
		}
		void								ShaderGenConf::addPointLight(const Vec3& color, const Vec3& pos, REAL range, ptrdiff_t visibility_channel)
		{
			if(lights >= MAX_LIGHTS)
				return;

			LIGHT& l = array_light[lights];

			l.type = GfxEnum::LIGHTTYPE_POINT;
			memcpy(l.color, &color, sizeof(color));
			memcpy(l.pos, &pos, sizeof(pos));
			l.range = range;
			l.visibility_channel = visibility_channel;

			++lights;
		}
		void								ShaderGenConf::addSpotLight(const Vec3& color, const Vec3& pos, const Vec3& dir, REAL inner_cone, REAL outer_cone, REAL range, ptrdiff_t visibility_channel)
		{
			if(lights >= MAX_LIGHTS)
				return;

			LIGHT& l = array_light[lights];

			l.type = GfxEnum::LIGHTTYPE_SPOT;
			memcpy(l.color, &color, sizeof(color));
			memcpy(l.pos, &pos, sizeof(pos));
			memcpy(l.dir, &dir, sizeof(dir));
			l.inner_cone = inner_cone;
			l.outer_cone = outer_cone;
			l.range = range;
			l.visibility_channel = visibility_channel;

			++lights;
		}
		void								ShaderGenConf::useTexCoord(size_t index, const Mat4& xform)
		{
			if(index >= MAX_TEXCOORD_SETS)
				return;

			if(memcmp(&xform, &Mat4::identity, sizeof(Mat4)) == 0)
				array_tc[index] = TCGEN_INTACT;
			else
			{
				array_tc[index] = TCGEN_XFORM;
				memcpy(array_tc_xform[index], &xform, sizeof(Mat4));
			}
		}
		void								ShaderGenConf::useTexCoord(size_t index, TCGEN tcgen)
		{
			if(index >= MAX_TEXCOORD_SETS)
				return;

			array_tc[index] = (uint8_t)tcgen;
		}
		void								ShaderGenConf::useTexSampler(size_t index, GfxEnum::TEXTARGET target)
		{
			if(index >= MAX_SAMPLERS)
				return;

			array_sampler[index] = (uint8_t)target;
		}
		void								ShaderGenConf::fetchTex(size_t sampler_idx, size_t tc_idx)
		{
			if(tex_fetches >= MAX_TEX_FETCHES)
				return;

			_TEX_FETCH& f = array_tex_fetch[ tex_fetches ];
			f.tc_idx = (uint8_t)tc_idx;
			f.sampler_idx = (uint8_t)sampler_idx;

			++tex_fetches;
		}
		void								ShaderGenConf::addPSCode(const mbs& code)
		{
			//memcpy(ps_code, code.c_str(), code.length() + 1);
			Z_ASSERT( code.length() + 1 + (_ps_code_end - _ps_code) <= _countof(_ps_code) );
			strcpy(_ps_code_end, code.c_str());
			_ps_code_end += code.length();
		}
		void								ShaderGenConf::addPSCode(const char* code)
		{
			//strcpy(ps_code, code);
			size_t len = strlen(code);
			Z_ASSERT( len + 1 + (_ps_code_end - _ps_code) <= _countof(_ps_code) );
			strcpy(_ps_code_end, code);
			_ps_code_end += len;
		}
		const char*							ShaderGenConf::getPSCode() const
		{
			return _ps_code;
		}
		void								ShaderGenConf::enableVertexColor(bool has_alpha)
		{
			enable.vertex_color = 1 + has_alpha;
		}
		void								ShaderGenConf::enablePerPixelLighting()
		{
			enable.per_pixel_lighting = true;
		}
		void								ShaderGenConf::enableNormalMapping(const Mat4& xform)
		{
			enable.normal_mapping = ((memcmp(&xform, &Mat4::identity, sizeof(Mat4)) == 0) ? 1 : 2);
			if(enable.normal_mapping == 2)
				memcpy(nm_tc_xform, &xform, sizeof(Mat4));
			enable.per_pixel_lighting = true;
		}
		void								ShaderGenConf::enableTwoSidedLighting()
		{
			enable.two_sided_lighting = true;
		}
		bool								ShaderGenConf::queryUseSpecular() const
		{
			return (lights && *(Vec3*)material.specular != Vec3::zero);
		}
	};
};

