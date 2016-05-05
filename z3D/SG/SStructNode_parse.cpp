
#define _CRT_SECURE_NO_WARNINGS 1

#include "SStructNode.h"

#include "../Core/functor_inl.h"

#include "SMeshNode.h"
#include "SBoneNode.h"
#include "SLightNode.h"
#include "SCameraNode.h"
#include "DataLoader.h"
#include "TMCtrl.h"
#include "MorpherCtrl.h"
#include "SkinCtrl.h"

namespace z3D
{
	namespace SG
	{
		void						SStructNode::Loader::scene_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "nodes") == 0 || strcmp(name, "array_node") == 0)
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::array_node_start));
			else if(strcmp(name, "hierarchy") == 0 || strcmp(name, "links") == 0)
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::hierarchy_start));
		}
		void						SStructNode::Loader::array_node_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "node") == 0)
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::node_start), SaxBase::ELEMENT_END_HANDLER(this, &SStructNode::Loader::node_end));
		}
		void						SStructNode::Loader::node_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "mesh") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::mesh_start));
			else if(strcmp(name, "light") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::light_start));
			else if(strcmp(name, "camera") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::camera_start));
			else if(strcmp(name, "bone") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::bone_start));

			if(strcmp(name, "node") != 0)
				return;

			_n.reset();

			_cast_shadow = false;
			_recv_shadow = false;

			const char* type = NULL;
			SaxBase::AttribGetter getter;
			getter.get("name", &_node_name, false);
			getter.get("pos", &_pos, false);
			getter.get("rot", &_rot, false);
			getter.get("scale", &_scale, false);
			getter.get("cast_shadow", &_cast_shadow, false);
			getter.get("recv_shadow", &_recv_shadow, false);
			getter.get("type", &type, false);
			parser->getAttribs(array_attrib, getter);

			if(type)
			{
				if(strcmp(type, "node") == 0)
				{
					_n = new SNode;
				}
			}
		}
		void						SStructNode::Loader::node_end(SaxBase* parser, const char* name)
		{
			_n->setPos(_pos);
			_n->setRot(_rot);
			_n->setScale(_scale);
			_n->setCastShadow(_cast_shadow);
			_n->setRecvShadow(_recv_shadow);

			_node->_node_map[_node_name] = _n;
			_n->setName(_node_name);
		}
		void						SStructNode::Loader::mesh_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "morpher") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::mesh_morpher_start), SaxBase::ELEMENT_END_HANDLER(this, &SStructNode::Loader::mesh_morpher_end));
			else if(strcmp(name, "mesh") != 0)
				return;

			wcs src;
			wcs mtl;

			SaxBase::AttribGetter getter;
			getter.get("src", &src, false);
			getter.get("mtl", &mtl, false);

			parser->getAttribs(array_attrib, getter);

			SPtr<SMeshNode> n = SMeshNode::load(src, mtl);
			_n = n;
		}
		void						SStructNode::Loader::mesh_morpher_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "morpher") == 0)
			{
				wcs src;
				uint32_t channel_count = 0;

				SaxBase::AttribGetter getter;
				getter.get("src", &src, false);
				getter.get("channel_count", &channel_count, false);

				parser->getAttribs(array_attrib, getter);

				_morpher_src = src;
				_morpher_channel_count = channel_count;

				_morpher_channels_weight.clear();
				_morpher_channels_animdata.clear();
			}
			else if(strcmp(name, "channel") == 0)
			{
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::mesh_morpher_channel_start));
			}
			else
				return;
		}
		void						SStructNode::Loader::mesh_morpher_end(SaxBase* parser, const char* name)
		{
			if(strcmp(name, "morpher") == 0)
			{
				if(_morpher_channels_weight.size() != _morpher_channel_count)
					return;

				SPtr<MorpherData> data = MorpherData::load(_morpher_src);
				if(!data)
					return;

				SPtr<MorpherCtrl> ctrl = new MorpherCtrl(L"", _n.cast<SMeshNode>(), data);
				for(size_t i = 0; i < _morpher_channel_count; ++i)
					ctrl->setChannelWeight(i, _morpher_channels_weight[i]);
				for(size_t i = 0; i < _morpher_channels_animdata.size(); ++i)
				{
					size_t index = _morpher_channels_animdata[i].first;
					SPtr<AnimData<REAL>> data = AnimDataArchive::loadAnimData<REAL>(_morpher_channels_animdata[i].second.first, _morpher_channels_animdata[i].second.second);
					if(!data)
						continue;

					ctrl->setChannelWeightAnimData(index, data);
				}

				REAL start, end;
				ctrl->getAnimDataTimeRange(start, end);
				ctrl->getAnimCtrl()->set(TimeAnimCfg(start, end, -1));

				_n.cast<SMeshNode>()->setMorpherCtrl(ctrl);
			}
		}
		void						SStructNode::Loader::mesh_morpher_channel_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "channel") == 0)
			{
				REAL weight;

				SaxBase::AttribGetter getter;
				getter.get("weight", &weight, false);
				parser->getAttribs(array_attrib, getter);

				_morpher_channels_weight.push_back(weight);
			}
			else if(strcmp(name, "animdata") == 0)
			{
			}
			else if(strcmp(name, "weight") == 0)
			{
				wcs src;
				uint32_t chunk = 0;

				SaxBase::AttribGetter getter;
				getter.get("src", &src, false);
				getter.get("chunk", &chunk, false);
				parser->getAttribs(array_attrib, getter);

				_morpher_channels_animdata.push_back(pair<size_t, pair<wcs, size_t> >(_morpher_channels_weight.size() - 1, pair<wcs, size_t>(src, chunk)));
			}
			else
				return;
		}
		void						SStructNode::Loader::light_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "animdata") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::light_animdata_start));

			if(strcmp(name, "light") != 0)
				return;

			const char* type = NULL;
			Vec3 color;
			REAL multiplier = 1.0f;
			REAL beam = 1.0f;
			REAL dist = 1.0f;
			REAL radius = 1.0f;
			REAL inner_cone = 1.0f;
			REAL outer_cone = 1.0f;

			bool enabled = true;
			bool cast_shadow = false;

			SaxBase::AttribGetter getter;
			getter.get("type", &type, false);
			getter.get("color", &color, false);
			getter.get("multiplier", &multiplier, false);
			getter.get("beam", &beam, false);
			getter.get("dist", &dist, false);
			getter.get("radius", &radius, false);
			getter.get("inner_cone", &inner_cone, false);
			getter.get("outer_cone", &outer_cone, false);
			getter.get("enabled", &enabled, false);
			getter.get("cast_shadow", &cast_shadow, false);

			parser->getAttribs(array_attrib, getter);

			if(type)
			{
				SPtr<SLightNode> n;
				if(strcmp(type, "ambient") == 0)
					n = SLightNode::create_ambient(color, multiplier, radius);
				else if(strcmp(type, "dir") == 0)
					n = SLightNode::create_dir(color, multiplier, radius, beam, dist, cast_shadow);
				else if(strcmp(type, "point") == 0)
					n = SLightNode::create_point(color, multiplier, radius, cast_shadow);
				else if(strcmp(type, "spot") == 0)
					n = SLightNode::create_spot(color, multiplier, radius, inner_cone, outer_cone, cast_shadow);
				_n = n;
				if(n)
					n->setEnabled(enabled);
				_l = n;
			}
		}
		void						SStructNode::Loader::light_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			wcs src;
			uint32_t chunk_idx = 0;

			SaxBase::AttribGetter getter;
			getter.get("src", &src, false);
			getter.get("chunk", &chunk_idx, false);
			parser->getAttribs(array_attrib, getter);

			if(strcmp(name, "color") == 0)
				_l->animdata_color() = AnimDataArchive::loadAnimData<Vec3>(src, chunk_idx);
			else if(strcmp(name, "multiplier") == 0)
				_l->animdata_multiplier() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "radius") == 0)
				_l->animdata_radius() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "inner_cone") == 0)
				_l->animdata_inner_cone() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "outer_cone") == 0)
				_l->animdata_outer_cone() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
		}
		void						SStructNode::Loader::camera_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "animdata") == 0)
				return parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::camera_animdata_start));

			if(strcmp(name, "camera") != 0)
				return;

			REAL fov = 90;
			REAL znear = 1;
			REAL zfar = 1000;
			REAL target_dist = 100;
			bool ortho = false;

			SaxBase::AttribGetter getter;
			getter.get("fov", &fov, false);
			getter.get("near", &znear, false);
			getter.get("far", &zfar, false);
			getter.get("target_dist", &target_dist, false);
			getter.get("ortho", &ortho, false);
			parser->getAttribs(array_attrib, getter);

			SPtr<SCameraNode> n = new SCameraNode();
			n->setPerspective(fov, znear, zfar);
			n->setTargetDist(target_dist);

			if(ortho)
				n->setOrtho(fov, znear, zfar);

			_n = n;

			_c = n;
		}
		void						SStructNode::Loader::camera_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			wcs src;
			uint32_t chunk_idx = 0;

			SaxBase::AttribGetter getter;
			getter.get("src", &src, false);
			getter.get("chunk", &chunk_idx, false);
			parser->getAttribs(array_attrib, getter);

			if(strcmp(name, "fov") == 0)
				_c->animdata_yfov() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "near") == 0)
				_c->animdata_znear() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "far") == 0)
				_c->animdata_zfar() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
			else if(strcmp(name, "target_dist") == 0)
				_c->animdata_target_dist() = AnimDataArchive::loadAnimData<REAL>(src, chunk_idx);
		}
		void						SStructNode::Loader::bone_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "bone") != 0)
				return;

			Vec3 mini, maxi;

			SaxBase::AttribGetter getter;
			getter.get("min", &mini, false);
			getter.get("max", &maxi, false);
			parser->getAttribs(array_attrib, getter);

			SPtr<SBoneNode> n = new SBoneNode(AABB(mini, maxi));
			_n = n;
		}
		void						SStructNode::Loader::hierarchy_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "link") != 0)
				return;

			wcs child;
			wcs parent;

			SaxBase::AttribGetter getter;
			getter.get("child", &child, false);
			getter.get("parent", &parent, false);

			parser->getAttribs(array_attrib, getter);

			SStructNode::MAP_NAME_LOADED_NODE::iterator iter = _node->_node_map.find(child);
			if(iter == _node->_node_map.end())
				return;
			SStructNode::MAP_NAME_LOADED_NODE::iterator iter2 = _node->_node_map.find(parent);
			if(iter2 == _node->_node_map.end())
				return;

			iter2->second->addChild(iter->second, false);
		}
		bool						SStructNode::Loader::load(const SPtr<SStructNode>& node, const wcs& filepath)
		{
			_node = node;

			if(true)
			{
				XmlSax sax;
				mbs error;

				sax.setElementStartHandler(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::scene_start));

				return sax.parse(filepath, error);
			}
			else
			{
				BxSax sax;
				mbs error;

				sax.setElementStartHandler(SaxBase::ELEMENT_START_HANDLER(this, &SStructNode::Loader::scene_start));

				return sax.parse(filepath + L".bx", error);
			}
		}
		SStructNode::SkinSetLoader::SkinSetLoader()
		{
		}
		bool							SStructNode::SkinSetLoader::load(const SPtr<SStructNode>& s, const wcs& filepath)
		{
			_s = s;

			if(true)
			{
				XmlSax sax;
				mbs error;

				sax.setElementStartHandler(SaxBase::ELEMENT_START_HANDLER(this, &SkinSetLoader::skins_start));

				return sax.parse(filepath, error);
			}
		}
		void							SStructNode::SkinSetLoader::skins_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "skin") != 0)
				return;

			SaxBase::AttribGetter getter;

			wcs node, src;

			getter.get("node", &node, false);
			getter.get("src", &src, false);

			parser->getAttribs(array_attrib, getter);

			SPtr<SkinData> skindata = SkinData::load(src);
			if(!skindata)
			{
				Z_ASSERT( false );
				return;
			}

			SPtr<SNode> n = _s->getLoadedNode(node);
			if(!n || !n->type_info()->exactly(SMeshNode::type_info_static()))
			{
				Z_ASSERT( false );
				return;
			}

			SPtr<SkinCtrl> ctrl = new SkinCtrl(L"", n.cast<SMeshNode>(), skindata, _s);
			n.cast<SMeshNode>()->setSkinCtrl(ctrl);
		}
		SStructNode::TMSetLoader::TMSetLoader()
		{
		}
		bool							SStructNode::TMSetLoader::load(const SPtr<SStructNode>& s, const wcs& filepath)
		{
			_s = s;

			if(true)
			{
				XmlSax sax;
				mbs error;

				sax.setElementStartHandler(SaxBase::ELEMENT_START_HANDLER(this, &TMSetLoader::tms_start));

				return sax.parse(filepath, error);
			}
		}
		void							SStructNode::TMSetLoader::tms_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "tm") == 0)
			{
				parser->passOnTo(SaxBase::ELEMENT_START_HANDLER(this, &TMSetLoader::tm_start), SaxBase::ELEMENT_END_HANDLER(this, &TMSetLoader::tm_end));
			}
		}
		void							SStructNode::TMSetLoader::tm_start(SaxBase* parser, const char* name, const char * const * array_attrib)
		{
			if(strcmp(name, "tm") == 0)
			{
				wcs node_name;

				SaxBase::AttribGetter getter;

				getter.get("node", &node_name, false);

				parser->getAttribs(array_attrib, getter);

				_n = _s->getLoadedNode(node_name);

				_tm = new TMCtrl(L"", _n);

				return;
			}
			wcs src;
			uint32_t chunk = 0;

			SaxBase::AttribGetter getter;

			getter.get("src", &src, false);
			getter.get("chunk", &chunk, false);

			parser->getAttribs(array_attrib, getter);

			if(strcmp(name, "pos") == 0)
			{
				if(_tm)
					_tm->setPosAnimData(AnimDataArchive::loadAnimData<Vec3>(src, chunk));
			}
			else if(strcmp(name, "rot") == 0)
			{
				if(_tm)
					_tm->setRotAnimData(AnimDataArchive::loadAnimData<Quat>(src, chunk));
			}
			else if(strcmp(name, "scale") == 0)
			{
				if(_tm)
					_tm->setScaleAnimData(AnimDataArchive::loadAnimData<Vec3>(src, chunk));
			}
		}
		void							SStructNode::TMSetLoader::tm_end(SaxBase* parser, const char* name)
		{
			if(strcmp(name, "tm") != 0)
				return;

			if(_n && _tm)
			{
				if(true)
				{
					REAL start, end;
					_tm->getAnimDataTimeRange(start, end);
					_tm->getAnimCtrl()->set(TimeAnimCfg(start, end, -1));
				}

				_n->setTMCtrl(_tm);
			}

			_n.reset();
			_tm.reset();
		}

		//void				SStructNode::parseTMCtrl(const string &utf8_str, NodeCtrlSet &tm_set)
		//{
		//	int cmd_no=0;
		//	map<wstring,int> cmd_map;
		//	vector<wstring> cmd_param_map;

		//	cmd_map[L"create_tm_controller"]=++cmd_no;

		//	int cmd_create_tm_controller=cmd_map[L"create_tm_controller"];

		//	cmd_param_map.resize(cmd_map.size()+1);

		//	cmd_param_map[cmd_create_tm_controller]=L"sssss";

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
		//		if(cmd==cmd_create_tm_controller)
		//		{
		//			SPtr<TMCtrl> ctrl=new TMCtrl(
		//				tokenizer.getTokenWString(token_idx+1),
		//				getNode(tokenizer.getTokenWString(token_idx+2))
		//				);
		//			ctrl->setPosAnimData(DataLoader::loadKeyframeVec3(tokenizer.getTokenWString(token_idx+3)));
		//			ctrl->setRotAnimData(DataLoader::loadKeyframeQuat(tokenizer.getTokenWString(token_idx+4)));
		//			ctrl->setScaleAnimData(DataLoader::loadKeyframeVec3(tokenizer.getTokenWString(token_idx+5)));
		//			ctrl->setByAnimDataRange();

		//			tm_set.push_back(ctrl);
		//		}
		//		token_idx+=(int)cmd_param.length()+1;
		//	}
		//}
		//void				SStructNode::parseSkinCtrl(const string &utf8_str, NodeCtrlSet &skin_set)
		//{
		//	int cmd_no=0;
		//	map<wstring,int> cmd_map;
		//	vector<wstring> cmd_param_map;

		//	cmd_map[L"create_skin_controller"]=++cmd_no;

		//	int cmd_create_skin_controller=cmd_map[L"create_skin_controller"];

		//	cmd_param_map.resize(cmd_map.size()+1);

		//	cmd_param_map[cmd_create_skin_controller]=L"sss";

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
		//		if(cmd==cmd_create_skin_controller)
		//		{
		//			SPtr<SkinData> skindata=DataLoader::loadSkinData(tokenizer.getTokenWString(token_idx+3));
		//			if(skindata)
		//			{
		//				SkinCtrl* ctrl=new SkinCtrl(tokenizer.getTokenWString(token_idx+1),getNode(tokenizer.getTokenWString(token_idx+2)).cast<SMeshNode>(),skindata,this);
		//				ctrl->set(0,1,-1,0);
		//				skin_set.push_back(ctrl);
		//			}
		//		}
		//		token_idx+=(int)cmd_param.length()+1;
		//	}
		//}
		void				SStructNode::parseUserText(const mbs& utf8_str, SNodeUserTextParser& parser)
		{
			int cmd_no = 0;
			map<wcs, int> cmd_map;
			vector<wcs> cmd_param_map;

			cmd_map[L"set_user_text"] = ++cmd_no;

			int cmd_set_user_text = cmd_map[L"set_user_text"];

			cmd_param_map.resize(cmd_map.size() + 1);

			cmd_param_map[cmd_set_user_text] = L"ss";

			Tokenizer tokenizer(utf8_str);
			size_t token_idx;
			for(token_idx = 0; token_idx < tokenizer.getTokenCount();)
			{
				if(tokenizer.getTokenType(token_idx) != SCN_TOKEN_IDENTIFIER)
				{
					++token_idx;
					continue;
				}

				int cmd = cmd_map[tokenizer.getTokenWString(token_idx)];
				const wcs& cmd_param = cmd_param_map[cmd];

				if(!cmd || !tokenizer.matchPattern(token_idx + 1, cmd_param))
				{
					// unrecognized command or parameter pattern

					++token_idx;
					continue;
				}
				if(cmd == cmd_set_user_text)
				{
					wcs name = tokenizer.getTokenWString(token_idx + 1);
					SPtr<SNode> node = getLoadedNode(name);

					parser.parseUserText(name, node, tokenizer.getTokenWString(token_idx + 2));
				}
				token_idx += (int)cmd_param.length() + 1;
			}
		}
	};
};

