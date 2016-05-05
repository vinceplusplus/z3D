
#pragma once

#include "predecl.h"
#include "SNode.h"
#include "SKDTNode.h"
#include "TMCtrl.h"

namespace z3D
{
	namespace SG
	{
		class SNodeUserTextParser
		{
		public:
			virtual ~SNodeUserTextParser();
		public:
			virtual void		parseUserText(const wcs& node_name,const SPtr<SNode>& node,const wcs& user_text) = 0;
		};

		class SStructNode:public SKDTNode, public SNodeUserTextParser
		{
		private:
			class Loader
			{
			private:
				SPtr<SStructNode>				_node;
			private:
				wcs								_node_name;
				Vec3							_pos;
				Quat							_rot;
				Vec3							_scale;
				bool							_cast_shadow;
				bool							_recv_shadow;
			private:
				SPtr<SNode>						_n;
				SPtr<SLightNode>				_l;
				SPtr<SCameraNode>				_c;
			private:
				wcs								_morpher_src;
				size_t							_morpher_channel_count;
				vector<REAL>					_morpher_channels_weight;
				vector<pair<size_t, pair<wcs, size_t> > >
												_morpher_channels_animdata;
			public:
				bool							load(const SPtr<SStructNode>& node, const wcs& filepath);
			private:
				void							scene_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							array_node_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							node_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							node_end(SaxBase* parser, const char* name);
				void							mesh_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							mesh_morpher_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							mesh_morpher_end(SaxBase* parser, const char* name);
				void							mesh_morpher_channel_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							light_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							light_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							camera_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							camera_animdata_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							bone_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							hierarchy_start(SaxBase* parser, const char* name, const char * const * array_attrib);
			};
		private:
			class SkinSetLoader
			{
			private:
				SPtr<SStructNode>				_s;
			public:
				SkinSetLoader();
			public:
				bool							load(const SPtr<SStructNode>& s, const wcs& filepath);
			private:
				void							skins_start(SaxBase* parser, const char* name, const char * const * array_attrib);
			};
		private:
			class TMSetLoader
			{
			private:
				SPtr<SStructNode>				_s;
			private:
				SPtr<TMCtrl>					_tm;
				SPtr<SNode>						_n;
			public:
				TMSetLoader();
			public:
				bool							load(const SPtr<SStructNode>& s, const wcs& filepath);
			private:
				void							tms_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							tm_start(SaxBase* parser, const char* name, const char * const * array_attrib);
				void							tm_end(SaxBase* parser, const char* name);
			};
		public:
			typedef map< wcs, SPtr<SNode> >	MAP_NAME_LOADED_NODE;						// TODO: develop specialized allocator
		public:
			DECL_TYPEINFO();
		protected:
			wcs								_filepath;
		protected:
			SPtr<SNode>						_null_node;
		protected:
			MAP_NAME_LOADED_NODE			_node_map;
		protected:
			SStructNode(const wcs& filepath);
			~SStructNode();
		public:
			const wcs&					filepath() const;
		public:
			static SPtr<SStructNode>	load(const wcs& filepath);
		public:
			void						loadTMCtrl(const wcs& filepath);											// load TM(transformation, SRT) controllers
			void						loadSkinCtrl(const wcs& filepath);											// load skin controllers
			void						loadUserText(const wcs& filepath);
			void						loadUserText(const wcs& filepath, SNodeUserTextParser& parser);				// load user text
			void						parseUserText(const mbs& utf8_str, SNodeUserTextParser& parser);
		public:
			const SPtr<SNode>&			getLoadedNode(const wcs& node_name) const;
			vector< SPtr<SNode> >		getLoadedNodesKindOf(const TypeInfo* type) const;
			vector< SPtr<SNode> >		getLoadedNodesTypeOf(const TypeInfo* type) const;
			const MAP_NAME_LOADED_NODE&	getLoadedNodes() const;
			size_t						loaded_node_count() const;
		public:
			void						loadTextures();
		public:
			void						parseUserText(const wcs& node_name, const SPtr<SNode>& node, const wcs& user_text);			// default implementation of user text parsing
		public:
			void						setBillboardCameraNode(const SPtr<SCameraNode>& camera_node);
		public:
			wcs							toString();
		};
	};
};

