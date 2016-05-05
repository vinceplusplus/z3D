
#include "SStructNode.h"
#include "SMeshNode.h"
#include "SLightNode.h"
#include "SCameraNode.h"
#include "BillboardCtrl.h"
#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		SNodeUserTextParser::~SNodeUserTextParser() {}

		SStructNode::SStructNode(const wcs& filepath)
			: _filepath(filepath)
		{
		}
		SStructNode::~SStructNode()
		{
		}
		const wcs&					SStructNode::filepath() const
		{
			return _filepath;
		}
		SPtr<SStructNode>		SStructNode::load(const wcs& filepath)
		{
			SPtr<SStructNode> struct_node = new SStructNode(filepath);
			Loader loader;
			if(!loader.load(struct_node, filepath))
			{
				Z_ASSERT( false );
				return NULL;
			}

			MAP_NAME_LOADED_NODE::iterator iter;
			for(iter = struct_node->_node_map.begin(); iter != struct_node->_node_map.end(); ++iter)
			{
				if(iter->second && !iter->second->parent())
					struct_node->addChild(iter->second, false);
				if(iter->second)
					iter->second->buildControllers(iter->second->getName());
			}

			return struct_node;
		}
		//void					SStructNode::loadTMCtrl(const wcs& filepath)
		//{
		//	loadTMCtrl(filepath/*,ctrlset_tm()*/);
		//}
		//void					SStructNode::loadSkinCtrl(const wcs& filepath)
		//{
		//	loadSkinCtrl(filepath/*,ctrlset_skin()*/);
		//}
		//void					SStructNode::loadUserText(const wcs& filepath)
		//{
		//	loadUserText(filepath, *this);
		//}
		void					SStructNode::loadTMCtrl(const wcs& filepath/*, NodeCtrlSet& tm_set*/)
		{
			//string tm_utf8=FileSystem::loadUTF8FromAll(filepath);
			//if(!tm_utf8.length())
			//	return;
			//parseTMCtrl(tm_utf8,tm_set);
			TMSetLoader loader;
			loader.load(this, filepath);
		}
		void					SStructNode::loadSkinCtrl(const wcs& filepath/*, NodeCtrlSet& skin_set*/)
		{
			//string skin_utf8=FileSystem::loadUTF8FromAll(filepath);
			//if(!skin_utf8.length())
			//	return;
			//parseSkinCtrl(skin_utf8,skin_set);
			SkinSetLoader loader;
			loader.load(this, filepath);
		}
		void					SStructNode::loadUserText(const wcs& filepath)
		{
			loadUserText(filepath, *this);
		}
		void					SStructNode::loadUserText(const wcs& filepath, SNodeUserTextParser& parser)
		{
			mbs ut_utf8 = FileSystem::loadUTF8FromAll(filepath);
			if(!ut_utf8.length())
				return;
			parseUserText(ut_utf8, parser);
		}
		const SPtr<SNode>&			SStructNode::getLoadedNode(const wcs& node_name) const
		{
			MAP_NAME_LOADED_NODE::const_iterator iter = _node_map.find(node_name);
			if(iter != _node_map.end())
				return iter->second;
			return _null_node;
		}
		vector<SPtr<SNode> >		SStructNode::getLoadedNodesKindOf(const TypeInfo* type) const
		{
			vector<SPtr<SNode> > array_node;
			MAP_NAME_LOADED_NODE::const_iterator iter;
			for(iter=_node_map.begin(); iter != _node_map.end(); ++iter)
				if(iter->second->type_info()->kind_of(type))
					array_node.push_back(iter->second);
			return array_node;
		}
		vector<SPtr<SNode> >		SStructNode::getLoadedNodesTypeOf(const TypeInfo* type) const
		{
			vector<SPtr<SNode> > array_node;
			MAP_NAME_LOADED_NODE::const_iterator iter;
			for(iter = _node_map.begin(); iter != _node_map.end(); ++iter)
				if(iter->second->type_info()->exactly(type))
					array_node.push_back(iter->second);
			return array_node;
		}
		const SStructNode::MAP_NAME_LOADED_NODE&
									SStructNode::getLoadedNodes() const
		{
			return _node_map;
		}
		size_t						SStructNode::loaded_node_count() const
		{
			return _node_map.size();
		}
		void					SStructNode::loadTextures()
		{
			MAP_NAME_LOADED_NODE::iterator iter;
			for(iter = _node_map.begin(); iter != _node_map.end(); ++iter)
				if(iter->second && iter->second->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					SPtr<SMeshNode> mn = iter->second.cast<SMeshNode>();
					mn->loadTextures();
				}
		}
		/*
		void					SStructNode::buildVBO()
		{
			hash_map<wstring,SPtr<SNode> >::iterator iter;
			for(iter=_node_map.begin();iter!=_node_map.end();++iter)
				if(iter->second&&iter->second->type_info()->kind_of(SMeshNode::type_info_static()))
					iter->second.cast<SMeshNode>()->buildVBO();
		}
		void					SStructNode::buildSPT()
		{
			hash_map<wstring,SPtr<SNode> >::iterator iter;
			for(iter=_node_map.begin();iter!=_node_map.end();++iter)
				if(iter->second&&iter->second->type_info()->kind_of(SMeshNode::type_info_static()))
					iter->second.cast<SMeshNode>()->buildSPT();
		}
		*/
		void					SStructNode::parseUserText(const wcs& node_name, const SPtr<SNode>& node, const wcs& user_text)
		{
			Config cfg(L"");
			cfg.readFromWCS(user_text, false);

			/*
			if(cfg.getInt32(L"build_spt")==1)
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
					node.cast<SMeshNode>()->buildSPT();
			}
			*/

			if(cfg.getInt32(L"billboard") == 1)
			{
				SPtr<BillboardCtrl> ctrl = getCtrlSet().getFirstTypeOf(BillboardCtrl::type_info_static()).cast<BillboardCtrl>();
				if(!ctrl)
				{
					ctrl = new BillboardCtrl(L"scene_billboard_controller");
					getCtrlSet().attachCtrl(ctrl);
				}
				if(cfg.getInt32(L"billboard_yaw_only") == 1)
					ctrl->addBillboard(node, true);
				else
					ctrl->addBillboard(node, false);
			}
		}
		//NodeCtrlSet&		SStructNode::ctrlset_tm()
		//{
		//	return _ctrlset_tm;
		//}
		//NodeCtrlSet&		SStructNode::ctrlset_skin()
		//{
		//	return _ctrlset_skin;
		//}
		//NodeCtrlSet&		SStructNode::ctrlset_specific()
		//{
		//	return _ctrlset_specific;
		//}
		//CtrlSet&			SStructNode::ctrlset_misc()
		//{
		//	return _ctrlset_misc;
		//}
		//void					SStructNode::advance_all(REAL dt)
		//{
		//	ctrlset_tm().advance(dt);
		//	//ctrlset_skin().advance(dt);
		//	ctrlset_specific().advance(dt);
		//	ctrlset_misc().advance(dt);
		//}
		//void					SStructNode::update_all()
		//{
		//	ctrlset_tm().update();
		//	ctrlset_skin().update();
		//	ctrlset_specific().update();
		//	ctrlset_misc().update();
		//}
		void					SStructNode::setBillboardCameraNode(const SPtr<SCameraNode>& camera_node)
		{
			SPtr<BillboardCtrl> ctrl = getCtrlSet().getFirstNamed(L"scene_billboard_controller").cast<BillboardCtrl>();
			if(!ctrl)
				return;
			ctrl->setCameraNode(camera_node);
		}
		wcs						SStructNode::toString()
		{
			return SNode::toString() + L", \"" + filepath() + L"\"";
		}
	};
};

