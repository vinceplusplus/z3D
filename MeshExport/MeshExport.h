/**********************************************************************
 *<
	FILE: MeshExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __MeshExport__H
#define __MeshExport__H

#pragma warning(push)
#pragma warning(disable:4819)

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#ifdef _DEBUG
	#include <assert.h>
#endif

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "Simpobj.h"
#include "Simpmod.h"
#include "Simpobj.h"
#include "Simpobj.h"
//SIMPLE TYPE

#include "simpmod.h"
#include "simpobj.h"
#include "hsv.h"
#include <direct.h>
#include <commdlg.h>
#include <guplib.h>
#include "frontend.h"
#include "tvnode.h"
#include "bmmlib.h"
#include "fltlib.h"
#include "ViewFile.h"
#include "meshadj.h"
#include "XTCObject.h"
#include "samplers.h"
#include "texutil.h"
#include "shaders.h"
#include "macrorec.h"
#include "gport.h"
#include "shadgen.h"
#include "stdmat.h"
#include "imtl.h"
#include "macrorec.h"
#include "tvutil.h"
#include "utilapi.h"
#include "IKSolver.h"
#include "ISkin.h"
#include "ISkinCodes.h"
#include "icurvctl.h"
#include "Manipulator.h"
#include "gizmo.h"
#include "gizmoimp.h"
#include "XTCObject.h"

#include "IPathConfigMgr.h"

#ifdef MAX_RELEASE_R12
#include "IFileResolutionManager.h"
#include "AssetManagement/iassetmanager.h"
#pragma comment(lib, "assetmanagement.lib")
#endif

#include <IGame/IGame.h>
#include <IGame/IGameObject.h>
#include <IGame/IGameProperty.h>
#include <IGame/IGameControl.h>
#include <IGame/IGameModifier.h>
#include <IGame/IConversionManager.h>
#include <IGame/IGameError.h>

#include <CS/BIPEXP.H>

#include "imtledit.h"

#undef PI

#undef DEG_TO_RAD
#undef RAD_TO_DEG

#include "../z3D/Core/Header.h"
#include "../z3D/z3DCoreUIHelper/Header.h"
#include "../z3D/IO/Header.h"
#include "../FreeImage/Header.h"

using namespace z3D::Core;
using namespace z3D::IO;

#include <crtdbg.h>

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#pragma comment(lib,"IGame.lib")

#define MeshExport_CLASS_ID	Class_ID(0x70aaa7a2, 0x521bdde8)

#define ExtendedSupport_CLASS_ID Class_ID(0x7e4e7794, 0x11d65705)

#define ExtendedSupport_AppData_CLASS_ID Class_ID(0x7c2d5a51, 0x5bed0baa)

class ExportOptions
{
public:
	static bool			text_version;
	static bool			export_bone_as_mesh;
	static bool			export_white_ambient_diffuse_color;
	static bool			export_white_specular_color;
	static bool			force_lightmap_filter_linear;
	static bool			sample_every_frame;
	static bool			convert_psd_to_tga;
	static bool			make_tex_size_power_of_2;
	static bool			ObjectSpace;
	static bool			export_pos_rot;
	static wcs			mesh_dir;									// relative path, without trailing slash
	static wcs			texture_dir;									// relative path, without trailing slash
	static wcs			animdata_dir;								// relative path, without trailing slash
	static bool			default_toon;
	static wcs			default_toon_map;							// relative path, without trailing slash
	static float		default_toon_outline_width;
	static float		default_toon_outline_color_r;
	static float		default_toon_outline_color_g;
	static float		default_toon_outline_color_b;
	static wcs			default_export_dir;
	static uint32_t		generated_tex_size;
	static uint32_t		max_tex_size;
	static bool			export_current_view;
	static bool			add_helper_lights;
	static bool			skip_textures;
};

class KFWriter;

extern Config* scene_cfg;

extern Interface* g_interface;
extern IGameScene* g_gamescene;

extern wcs		export_base_dir;									// absolute path, with trailing slash
extern wcs		export_mesh_dir;									// absolute path, with trailing slash
extern wcs		export_texture_dir;									// absolute path, with trailing slash
extern wcs		export_animdata_dir;								// absolute path, with trailing slash

extern vector<TimeValue> _array_restricted_timevalue;

class skrootdlgcallback:public HitByNameDlgCallback
{
public:
	TCHAR* dialogTitle()
	{
		return "Choose the skeleton root node";
	}
	BOOL singleSelect() {return TRUE;}
};

class pathdlgcallback:public HitByNameDlgCallback
{
public:
	INode* node;
public:
	TCHAR* dialogTitle()
	{
		return "Choose the Path";
	}
	BOOL singleSelect() {return TRUE;}
	int filter(INode* node)
	{
		const ObjectState& os=node->EvalWorldState(0);
		if(os.obj&&os.obj->SuperClassID()==SHAPE_CLASS_ID)
			return TRUE;
		return FALSE;
	}
	TCHAR* buttonText() {return "OK";}
	BOOL useProc()
	{
		return FALSE;
	}
};

class AnimDataExporter
{
public:
	~AnimDataExporter() {}
public:
	virtual void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer) = 0;
};

extern Mat4 ROTXNEG90_4;
extern Mat3 ROTXNEG90_3;
extern Mat4 ROTX90_4;
extern Mat3 ROTX90_3;

Vec3 toVec3(Point3 p);
Mat4 toMat4(GMatrix m);
Mat4 toMat4(Matrix3 m);

Mat4 getLocalTM(IGameNode* node,TimeValue t = TIME_NegInfinity);
Mat4 getWorldTM(IGameNode* node);
Mat4 getWorldTM(INode* node, TimeValue t = TIME_NegInfinity);

RNormal*	getFaceVertexNormal(Mesh& mesh,int face,int vi);

wcs			getMeshDir();
wcs			getTextureDir();
wcs			getAnimDataDir();

bool chooseSkeletonRoot(Interface* inter);
void enumSkeletonNodes(IGameNode* node,vector<IGameNode*>& array_node);
void enumKeyTimeValue(IKeyControl* ikeycontrol,map<TimeValue,bool>& array_time);					// bool - step?
void convert(const map<TimeValue,bool>& map_time,vector<TimeValue>& vector_time);
void convert(const map<TimeValue,bool>& map_time,vector<pair<TimeValue, bool>>& vector_time);
void getNodePositionKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1=true);
void getNodeRotationKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1=true);
void getNodeScaleKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1=true);
bool anyKeyFrame(INode* node);
TriObject* GetTriObjectFromNode(INode *node, int &deleteIt, Mat4& tm);
TriObject* GetTriObjectFromNode(INode *node, int &deleteIt);
Mtl* getNodeExportMaterial(IGameNode* node);
Mtl* getSubMaterial(Mtl* mtl,MtlID mtlid);
int getRNormalCount(Mesh& mesh,int vertex);
RNormal& getRNormal(Mesh& mesh,int vertex,int index);
Point3 getRNormal(Mesh& mesh,int vertex,DWORD smgroup,INode* node);
Texmap* findTexmap(Mtl* mtl,int texmap_idx);
fipImage outputTexmap(Texmap* texmap,int tex_res,bool mono);
bool findTexMapChannel(Mtl* mtl,int texmap_idx,int& channel);
bool haveMapChannel(Mesh& mesh,int channel);
Point3 getMapVertex(Mesh& mesh,int channel,int vertex);
void markUsedMapVertices(Mesh& mesh,Mtl* mtl,int texmap_idx,const vector<int>& array_face,UniqueMap<__int64>& used_map_vertex_idx);
bool isNormalBump(Mtl* mtl);
bool exportNodeMesh(IGameNode* node,MemWriter& mesh_writer,MemWriter& mtl_writer);
bool exportSkeleton(IGameNode* root,MemWriter& writer);
void exportSkeletonNEW(Interface *inter,IGameScene* gs);
bool exportSkin(IGameNode* node,MemWriter& skin_writer);

bool isBoneNode(IGameNode* node);
bool isMeshNode(IGameNode* node);
bool isLightNode(IGameNode* node);
bool isCameraNode(IGameNode* node);
bool isMeshNodeSkinned(IGameNode* node);
vector<IGameNode*> enumSceneStructNode();
bool exportTMController(const vector<IGameNode*> array_node,MemWriter& writer,bool at_least_1=false);
void exportTMController2(const vector<IGameNode*>& array_node);
void exportTMController3(const vector<IGameNode*>& array_node);
bool exportSceneStruct(const vector<IGameNode*> array_node,MemWriter& writer);
bool exportSceneSkinController(const vector<IGameNode*> array_node,MemWriter& writer);
bool exportUserText(const vector<IGameNode*> array_node,MemWriter& writer);

void exportScene();

void exportPath();
bool choosePath(Interface* inter);
bool exportPath(IGameNode* node,MemWriter& path_writer);

Animatable* getSubAnim(Animatable* anim,const wcs& name);
void listAnimatables(Animatable* anim);
void listParams(IParamBlock2* pb);
void listParamBlocks(Animatable* animatable);


bool exportAnimData(IKeyControl* keyctl, AnimDataExporter& exporter, const wcs& output_filepath);					// will clear the writer

void clearMeshNodeCache();

void getMaterialExtendedProperties(MtlBase* mtl, Config& cfg);


class EXPORTED_MORPHER
{
public:
	INode*						node;
	wcs							filepath;
	vector<pair<float, size_t> >		chans;
public:
	EXPORTED_MORPHER()
	{
		node = NULL;
	}
};


bool exportMeshNode(IGameNode* node, wcs& exported_mesh, wcs& exported_mtl, EXPORTED_MORPHER& exported_morpher, wcs& exported_skin, KFWriter& kfwriter, const wcs& animdata_misc_path);


wcs		getAnimDataExportAbsDir();
wcs		getAnimDataExportRelDir();
wcs		getTextureExportAbsDir();
wcs		getTextureExportRelDir();
wcs		getMeshExportAbsDir();
wcs		getMeshExportRelDir();


wcs		getMeshDirREL();
wcs		getMeshDirABS();
wcs		getTextureDirREL();
wcs		getTextureDirABS();
wcs		getAnimDataDirREL();
wcs		getAnimDataDirABS();
wcs		getExportBaseDir();

void enumKeyTimeValueRecur(Animatable* ctl,map<TimeValue,bool>& array_time);

#include "../tinyxml/tinyxml.h"
#pragma comment(lib, "tinyxml.lib")

class zTiXmlElement: public TiXmlElement
{
public:
	// this is a must have. without it, e.g. setAttribute("type", "mesh") -> setAttribute( const z3D::Core::mbs&, bool ), because the compiler will find the closest match from PODs first then classes
	void SetAttribute( const z3D::Core::mbs& name, const char* _value )
	{
		TiXmlElement::SetAttribute(CONVERT(name), _value);
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::mbs& _value )
	{
		TiXmlElement::SetAttribute(CONVERT(name), CONVERT(_value));
	}
	void SetAttribute( const z3D::Core::mbs& name, REAL _value )
	{
		TiXmlElement::SetDoubleAttribute(name.c_str(), (double)_value);
	}
	void SetAttribute( const z3D::Core::mbs& name, double _value )
	{
		TiXmlElement::SetDoubleAttribute(name.c_str(), _value);
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::wcs& _value )
	{
		SetAttribute(name, WCSTOUTF8(_value));
	}
	void SetAttribute( const z3D::Core::mbs& name, bool _value )
	{
		SetAttribute(name, FORMATA("%d", _value? 1 : 0));
	}
	void SetAttribute( const z3D::Core::mbs& name, int32_t _value )
	{
		SetAttribute(name, FORMATA("%d", _value));
	}
	void SetAttribute( const z3D::Core::mbs& name, int64_t _value )
	{
		SetAttribute(name, FORMATA("%I64d", _value));
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::Vec2& _value )
	{
		SetAttribute(name, FORMATA("%f %f", _value[0], _value[1]));
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::Vec3& _value )
	{
		SetAttribute(name, FORMATA("%f %f %f", _value[0], _value[1], _value[2]));
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::Vec4& _value )
	{
		SetAttribute(name, FORMATA("%f %f %f %f", _value[0], _value[1], _value[2], _value[3]));
	}
	void SetAttribute( const z3D::Core::mbs& name, const z3D::Core::Quat& _value )
	{
		SetAttribute(name, FORMATA("%f %f %f %f", _value[0], _value[1], _value[2], _value[3]));
	}
};

template<class T>
class KeyGetter
{
public:
	typedef T					KEYTYPE;
public:
	T							get(TimeValue t)
	{
		Z_STATIC_ASSERT( false, MISSING_IMPLEMENTATION );
	}
};

template<class T>
class KEYTYPE_IDENTIFYING {};

// keyframe writer
class KFWriter
{
private:
	MemWriter					_writer;
private:
	size_t						_chunks;
public:
	KFWriter()
	{
		_chunks = 0;
	}
public:
	size_t						chunks() const												{	return _chunks;	}
	MemWriter&					getWriter()													{	return _writer;	}
public:
	uint32_t					KEYTYPE_ID(KEYTYPE_IDENTIFYING<z3D::Core::Vec3>) const		{	return 1;	}
	uint32_t					KEYTYPE_ID(KEYTYPE_IDENTIFYING<z3D::Core::Quat>) const		{	return 2;	}
	uint32_t					KEYTYPE_ID(KEYTYPE_IDENTIFYING<z3D::Core::REAL>) const		{	return 3;	}
	uint32_t					KEYTYPE_ID(KEYTYPE_IDENTIFYING<z3D::Core::Vec4>) const		{	return 4;	}
	template<class T>
	uint32_t					KEYTYPE_ID(T) const											{	Z_STATIC_ASSERT(false, WRONG_TYPE);	}
public:
	template<class KEYGETTER>
	bool						exportChunk(const vector<pair<TimeValue, bool>>& array_timevalue, KEYGETTER key_getter, size_t& chunk_idx)
	{
		if(!array_timevalue.size())
			return false;

		REAL ticks_divider = (REAL)(GetTicksPerFrame() * GetFrameRate());

		size_t chunk_size_offset = _writer.offset();

		// write chunk size
		_writer.write((uint32_t)0);

		// write key type id
		_writer.write(KEYTYPE_ID(KEYTYPE_IDENTIFYING<KEYGETTER::KEYTYPE>()));

		// write number of keys
		_writer.write((uint32_t)array_timevalue.size());

		// write key data
		for(size_t i = 0; i < array_timevalue.size(); ++i)
		{
			_writer.write((REAL)(array_timevalue[i].first / ticks_divider));
			KEYGETTER::KEYTYPE key = key_getter.get(array_timevalue[i].first);
			_writer.write(&key, sizeof(key));
		}

		vector<uint8_t> tangents_type;
		tangents_type.resize((array_timevalue.size() + 7) >> 3);

		// write keys' tangent type
		for(size_t i = 0; i < array_timevalue.size(); ++i)
			tangents_type[i >> 3] |= (uint8_t)(((uint8_t)array_timevalue[i].second) << (i & 7));

		_writer.write(&tangents_type[0], tangents_type.size() * sizeof(tangents_type[0]));

		// update chunk size
		size_t offset = _writer.offset();
		_writer.seek(chunk_size_offset);
		_writer.write((uint32_t)(offset - chunk_size_offset));
		_writer.seek(offset);

		chunk_idx = _chunks;
		++_chunks;

		return true;
	}
	template<class KEYGETTER>
	bool						exportChunk(Animatable* animatable, KEYGETTER key_getter, size_t& chunk_idx, bool force_export = false)
	{
		if(!animatable)
			return false;

		map<TimeValue, bool> map_timevalue;
		vector<pair<TimeValue, bool>> array_timevalue;

		enumKeyTimeValueRecur(animatable, map_timevalue);
		convert(map_timevalue, array_timevalue);

		if((force_export || array_timevalue.size() >= 1) && ExportOptions::sample_every_frame)
		{
			array_timevalue.clear();
			for(size_t i = 0; i < _array_restricted_timevalue.size(); ++i)
			{
				bool step = false;
				map<TimeValue, bool>::iterator lb = map_timevalue.lower_bound(_array_restricted_timevalue[i]);
				if(lb != map_timevalue.end())
					step = lb->second;
				array_timevalue.push_back(pair<TimeValue, bool>(_array_restricted_timevalue[i], step));
			}
		}

		return exportChunk(array_timevalue, key_getter, chunk_idx);
	}
};


#pragma warning(pop)

#endif
