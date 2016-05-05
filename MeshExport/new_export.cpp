
#pragma warning(push)
#pragma warning(disable:4996)
#include "../samples/modifiers/morpher/wm3.h"
#pragma warning(pop)

#include "MeshExport.h"

#include <shlwapi.h>

#pragma comment(lib, "bmm.lib")

#pragma comment(lib, "shlwapi.lib")

class TexSampler: public ShadeContext
{
public:
		Point3 ltPos; // position of point in light space
		Point3 view;  // unit vector from light to point, in light space
		Point3 dp; 
		Point2 uv,duv;
		IPoint2 scrpos;
public:
	Point3		UVW(int chan) { return Point3(uv.x, uv.y, 0.0f); }
		Point3 DUVW(int chan) { return Point3(duv.x, duv.y, 0.0f);  }
		BOOL InMtlEditor() {return FALSE;}
		LightDesc* Light(int n) { return NULL; }
		TimeValue CurTime() { return g_interface->GetTime(); }
		int NodeID() { return -1; }
		int FaceNumber() { return 0; }
		//int ProjType() { return projType; }
		Point3 Normal() { return Point3(0,0,0); }
		Point3 GNormal() { return Point3(0,0,0); }
		Point3 ReflectVector(){ return Point3(0,0,0); }
		Point3 RefractVector(float ior){ return Point3(0,0,0); }
		Point3 CamPos() { return Point3(0,0,0); }

		Point3 V() { return view; }
		void SetView(Point3 v) { view = v; }
		Point3 P() { return ltPos; }	
		Point3 DP() { return dp; }
		Point3 PObj() { return ltPos; }
		Point3 DPObj(){ return Point3(0,0,0); } 
		Box3 ObjectBox() { return Box3(Point3(-1,-1,-1),Point3(1,1,1));}   	  	
		Point3 PObjRelBox() { return view; }
		Point3 DPObjRelBox() { return Point3(0,0,0); }
		void ScreenUV(Point2& UV, Point2 &Duv) { UV = uv; Duv = duv; }
		IPoint2 ScreenCoord() { return scrpos;} 
		void DPdUVW(Point3 dP[3], int chan) {}  // dont need bump vectors
		void GetBGColor(Color &bgcol, Color& transp, BOOL fogBG=TRUE) {}   // returns Background color, bg transparency
		Point3 PointTo(const Point3& p, RefFrame ito) { return p; } 
		Point3 PointFrom(const Point3& p, RefFrame ifrom) { return p; } 
		Point3 VectorTo(const Point3& p, RefFrame ito) { return p; } 
		Point3 VectorFrom(const Point3& p, RefFrame ifrom) { return p; } 
};

class uoff_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:uoff_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetUOffs(timevalue));	}};
class voff_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:voff_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetVOffs(timevalue));	}};
class uscl_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:uscl_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetUScl(timevalue));	}};
class vscl_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:vscl_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetVScl(timevalue));	}};
class uang_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:uang_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetUAng(timevalue));	}};
class vang_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:vang_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetVAng(timevalue));	}};
class wang_exporter: public AnimDataExporter	{private:StdUVGen*					uvgen;public:wang_exporter(StdUVGen* gen):uvgen(gen) {}public:
	void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
		writer.write(uvgen->GetWAng(timevalue));	}};

//wstring getAnimDataExportAbsDir()
//{
//	return export_animdata_dir + L"/";
//}
//
//wstring getAnimDataExportRelDir()
//{
//	return ExportOptions::animdata_dir+L"/";
//}
//
//wstring getTextureExportAbsDir()
//{
//	return export_texture_dir + L"/";
//}
//
//wstring getTextureExportRelDir()
//{
//	return ExportOptions::texture_dir+L"/";
//}
//
//wstring getMeshExportAbsDir()
//{
//	return export_mesh_dir + L"/";
//}
//
//wstring getMeshExportRelDir()
//{
//	return ExportOptions::mesh_dir+L"/";
//}
//
void writeUVGen(int mtl_index,const wcs& map_name,StdUVGen* uvgen,wcs& mtl_output)
{
	mtl_output+=FORMATW(L"set_mtl_%s_map_u_offset %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetUOffs(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_v_offset %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetUOffs(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_u_tiling %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetUScl(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_v_tiling %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetVScl(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_u_angle %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetUAng(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_v_angle %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetVAng(0));
	mtl_output+=FORMATW(L"set_mtl_%s_map_w_angle %d %f\r\n",map_name.c_str(),mtl_index,uvgen->GetWAng(0));
}

void writeUVGenAnimData(IGameNode* node, int mtl_index, const wcs& map_name, Texmap* texmap, StdUVGen* uvgen, wcs& mtl_output)
{
	Animatable* animatable=NULL;

	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"U Offset");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_offu_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),uoff_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_u_offset_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"V Offset");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_offv_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),voff_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_v_offset_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"U Tiling");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_sclu_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),uscl_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_u_tiling_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"V Tiling");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_sclv_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),vscl_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_v_tiling_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"U Angle");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_angu_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),uang_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_u_angle_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"V Angle");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_angv_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),vang_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_v_angle_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
	animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),L"W Angle");
	if(animatable && GetKeyControlInterface(animatable))
	{
		wcs name=FORMATW(L"mtl_%s_map_angw_",map_name.c_str())+MBSTOWCS(node->GetName())+FORMATW(L"_%d",mtl_index);
		wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
		wcs path=export_animdata_dir + L"/" + name + L".bin";
		if(exportAnimData(GetKeyControlInterface(animatable),wang_exporter(uvgen),path))
			mtl_output += FORMATW(L"set_mtl_%s_map_w_angle_animdata %d \"%s\"",map_name.c_str(),mtl_index,rel_path.c_str());
	}
}


Mat4 getWorldTM(IGameNode* node)
{
	/*
	Mat4 m=Mat4::identity;
	while(node)
	{
		m=m*toMat4(node->GetLocalTM());
		node=node->GetNodeParent();
	}
	return m;*/
	return toMat4(node->GetWorldTM());
}

Mat4 getWorldTM(INode* node, TimeValue t)
{
	return ROTX90_4 * toMat4(node->GetNodeTM(t)) * ROTXNEG90_4;
}

Mat4 toMat4(Matrix3 m)
{
	Mat4 m2;
	m2[3][3] = 1.0f;
	m2[0][0] = m.GetAddr()[0][0]; m2[0][1] = m.GetAddr()[0][1]; m2[0][2] = m.GetAddr()[0][2];
	m2[1][0] = m.GetAddr()[1][0]; m2[1][1] = m.GetAddr()[1][1]; m2[1][2] = m.GetAddr()[1][2];
	m2[2][0] = m.GetAddr()[2][0]; m2[2][1] = m.GetAddr()[2][1]; m2[2][2] = m.GetAddr()[2][2];
	m2[3][0] = m.GetAddr()[3][0]; m2[3][1] = m.GetAddr()[3][1]; m2[3][2] = m.GetAddr()[3][2];
	return m2;
}

Point3 getFaceNormal(Mesh& mesh, int face)
{
	Face& f = mesh.faces[face];
	return (mesh.getVert(f.v[1]) - mesh.getVert(f.v[0])) ^ (mesh.getVert(f.v[2]) - mesh.getVert(f.v[1]));
}

RNormal*	getVertexRNormal(Mesh& mesh, int vertex, DWORD smGroup)
{
	if(!smGroup)
		return NULL;
	RVertex& r = mesh.getRVert(vertex);
	if(r.rFlags & SPECIFIED_NORMAL)
		return &r.rn;
	int num = r.rFlags & NORCT_MASK;
	if(!num)
		return NULL;
	if(num == 1)
		return &r.rn;
	if(num < 0)
		return NULL;
	for(int i = 0; i < num; ++i)
		if((r.ern[i].getSmGroup() & smGroup) == smGroup)
			return r.ern + i;
	return NULL;
}

RNormal*	getFaceVertexNormal(Mesh& mesh, int face, int vi)
{
	int v = mesh.faces[face].v[vi];
	DWORD sm = mesh.faces[face].smGroup;
	return getVertexRNormal(mesh, v, sm);
}

bool chooseSkeletonRoot(Interface* inter)
{
	/*
	skrootdlgcallback callback;
	if(!inter->DoHitByNameDialog(&callback))
		return false;
	return true;*/

	if(!inter->DoHitByNameDialog())
		return false;
	return true;
}

void enumSkeletonNodes(IGameNode* node,vector<IGameNode*>& array_node)
{
	array_node.push_back(node);
	int i;
	for(i=0;i<node->GetChildCount();++i)
	{
		if(!node->GetNodeChild(i)->IsNodeHidden())
			enumSkeletonNodes(node->GetNodeChild(i),array_node);
	}
}

void enumKeyTimeValue(IKeyControl* ikeycontrol, map<TimeValue, bool>& array_time)
{
	IKey* key = (IKey*)new char[ikeycontrol->GetKeySize() * 2];

	for(int i = 0; i < ikeycontrol->GetNumKeys(); ++i)
	{
		ikeycontrol->GetKey(i, key);
		if(!array_time.count(key->time))
			array_time[key->time] = (GetOutTanType(key->flags) == BEZKEY_STEP);
		else
			array_time[key->time] &= (GetOutTanType(key->flags) == BEZKEY_STEP);
	}

	delete (char*)key;
}

void convert(const map<TimeValue, bool>& map_time, vector<TimeValue>& vector_time)
{
	vector_time.clear();
	map<TimeValue,bool>::const_iterator iter;
	for(iter=map_time.begin();iter!=map_time.end();++iter)
		vector_time.push_back(iter->first);
}

void convert(const map<TimeValue, bool>& map_time, vector<pair<TimeValue, bool>>& vector_time)
{
	vector_time.clear();
	map<TimeValue, bool>::const_iterator iter;
	for(iter = map_time.begin(); iter != map_time.end(); ++iter)
		vector_time.push_back(*iter);
}

void enumKeyTimeValueRecur(Animatable* ctl, map<TimeValue, bool>& array_time)
{
	if(!ctl)
		return;
	IKeyControl* key_ctl = NULL;
	key_ctl = GetKeyControlInterface(ctl);
	if(key_ctl)
		enumKeyTimeValue(key_ctl, array_time);
	
	for(int i = 0; i < ctl->NumSubs(); ++i)
		enumKeyTimeValueRecur(ctl->SubAnim(i), array_time);
}

void getNodePositionKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1)
{
	map<TimeValue,bool> key_timevalue_map;
	timevalue.clear();

	if(node->GetTMController())
	{
		enumKeyTimeValueRecur(node->GetTMController(),key_timevalue_map);
		if(node->GetTMController()->GetPositionController())
		{
			enumKeyTimeValueRecur(node->GetTMController()->GetPositionController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetPositionController()->GetXController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetPositionController()->GetYController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetPositionController()->GetZController(),key_timevalue_map);
		}
	}
	convert(key_timevalue_map,timevalue);

	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);

	if(timevalue.size()>=1 && ExportOptions::sample_every_frame)
		timevalue=_array_restricted_timevalue;

	return;

	Control* tm_ctl=node->GetTMController();
	IKeyControl* key_ctl=NULL;
	if(tm_ctl)
	{
		if(tm_ctl->ClassID()==BIPBODY_CONTROL_CLASS_ID)
		{
			Animatable* sub_ctl;
			if(tm_ctl->NumSubs()>VERTICAL_SUBANIM)
				if((sub_ctl=tm_ctl->SubAnim(VERTICAL_SUBANIM))!=NULL)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
			if(tm_ctl->NumSubs()>HORIZONTAL_SUBANIM)
				if((sub_ctl=tm_ctl->SubAnim(HORIZONTAL_SUBANIM))!=NULL)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
			convert(key_timevalue_map,timevalue);
			return;
		}
		Control* pos_ctl=tm_ctl->GetPositionController();
		if(pos_ctl)
		{
			key_ctl=GetKeyControlInterface(pos_ctl);
			if(key_ctl)
			{
				enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
			else
			{
				Control* sub_ctl;
				sub_ctl=pos_ctl->GetXController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=pos_ctl->GetYController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=pos_ctl->GetZController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
		}
	}
	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);
}

void getNodeRotationKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1)
{
	map<TimeValue,bool> key_timevalue_map;
	timevalue.clear();

	if(node->GetTMController())
	{
		enumKeyTimeValueRecur(node->GetTMController(),key_timevalue_map);
		if(node->GetTMController()->GetRotationController())
		{
			enumKeyTimeValueRecur(node->GetTMController()->GetRotationController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetRotationController()->GetXController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetRotationController()->GetYController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetRotationController()->GetZController(),key_timevalue_map);
		}
	}
	convert(key_timevalue_map,timevalue);

	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);

	if(timevalue.size()>=1 && ExportOptions::sample_every_frame)
		timevalue=_array_restricted_timevalue;

	return;

	Control* tm_ctl=node->GetTMController();
	IKeyControl* key_ctl=NULL;
	if(tm_ctl)
	{
		if(tm_ctl->ClassID()==BIPBODY_CONTROL_CLASS_ID)
		{
			Animatable* sub_ctl;
			if(tm_ctl->NumSubs()>ROTATION_SUBANIM)
				if((sub_ctl=tm_ctl->SubAnim(ROTATION_SUBANIM))!=NULL)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
		}
		Control* rot_ctl=tm_ctl->GetRotationController();
		if(rot_ctl)
		{
			key_ctl=GetKeyControlInterface(rot_ctl);
			if(key_ctl)
			{
				enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
			else
			{
				Control* sub_ctl;
				sub_ctl=rot_ctl->GetXController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=rot_ctl->GetYController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=rot_ctl->GetZController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
		}
	}
	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);
}

void getNodeScaleKeyFrameTimeValue(INode* node,vector<TimeValue>& timevalue,bool at_least_1)
{
	map<TimeValue,bool> key_timevalue_map;
	timevalue.clear();

	if(node->GetTMController())
	{
		enumKeyTimeValueRecur(node->GetTMController(),key_timevalue_map);
		if(node->GetTMController()->GetScaleController())
		{
			enumKeyTimeValueRecur(node->GetTMController()->GetScaleController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetScaleController()->GetXController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetScaleController()->GetYController(),key_timevalue_map);
			enumKeyTimeValueRecur(node->GetTMController()->GetScaleController()->GetZController(),key_timevalue_map);
		}
	}
	convert(key_timevalue_map,timevalue);

	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);

	if(timevalue.size()>=1 && ExportOptions::sample_every_frame)
		timevalue=_array_restricted_timevalue;

	return;

	Control* tm_ctl=node->GetTMController();
	IKeyControl* key_ctl=NULL;
	if(tm_ctl)
	{
		Control* scale_ctl=tm_ctl->GetScaleController();
		if(scale_ctl)
		{
			key_ctl=GetKeyControlInterface(scale_ctl);
			if(key_ctl)
			{
				enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
			else
			{
				Control* sub_ctl;
				sub_ctl=scale_ctl->GetXController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=scale_ctl->GetYController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				sub_ctl=scale_ctl->GetZController();
				if(sub_ctl)
					if((key_ctl=GetKeyControlInterface(sub_ctl))!=NULL)
						enumKeyTimeValue(key_ctl,key_timevalue_map);
				convert(key_timevalue_map,timevalue);
			}
		}
	}
	if(at_least_1&&!timevalue.size())
		timevalue.push_back(0);
}

bool anyKeyFrame(INode* node)
{
	vector<TimeValue> time1,time2,time3;
	getNodePositionKeyFrameTimeValue(node,time1,false);
	getNodeRotationKeyFrameTimeValue(node,time2,false);
	getNodeScaleKeyFrameTimeValue(node,time3,false);
	if(!time1.size()&&!time2.size()&&!time3.size())
		return false;
	return true;
}

TriObject* GetTriObjectFromNode(INode *node, int &deleteIt)
{
	Mat4 tm;
	return GetTriObjectFromNode(node, deleteIt, tm);
}

TriObject* GetTriObjectFromNode(INode *node, int &deleteIt, Mat4& tm)
{
	deleteIt = FALSE;

	const ObjectState& objstate = node->EvalWorldState(GetCOREInterface()->GetTime());
	Object *obj = objstate.obj;

	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{ 
		TriObject *tri = (TriObject *) obj->ConvertToType(0, 

		Class_ID(TRIOBJ_CLASS_ID, 0));

		// Note that the TriObject should only be deleted

		// if the pointer to it is not equal to the object

		// pointer that called ConvertToType()

		if (obj != tri)
			deleteIt = TRUE;

		tm = Mat4::identity;
		if(!objstate.TMIsIdentity() && objstate.GetTM())
			tm = toMat4(*objstate.GetTM());

		return tri;

	}
	else
		return NULL;
}

Mtl* getNodeExportMaterial(IGameNode* node)
{
	Mtl* mtl = node->GetMaxNode()->GetMtl();
	if(!mtl)
		return NULL;
	if(mtl->ClassID() != Class_ID(BAKE_SHELL_CLASS_ID, 0))
		return mtl;
	return mtl->GetSubMtl(1);
}

Mtl* getSubMaterial(Mtl* mtl,MtlID mtlid)
{
	if(!mtl)
		return NULL;
	if(!mtl->NumSubMtls())
	{
		if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0))
		{
			syslog<<wcs(L"unsupported material: ") + MBSTOWCS((char*)mtl->GetName()) + L", only standard and multi are supported";
			return NULL;
		}
		return mtl;
	}
	//syslog_stl<<FORMATW(L"mtl id: %d",mtlid);
	//Mtl* mtl2=mtl->GetSubMtl((mtlid-1)%mtl->NumSubMtls());
	Mtl* mtl2 = mtl->GetSubMtl(mtlid % mtl->NumSubMtls());
	if(!mtl2)
		return NULL;
	if(mtl2->ClassID() != Class_ID(DMTL_CLASS_ID,0))
	{
		syslog<<wcs(L"unsupported material: ") + MBSTOWCS((char*)mtl->GetName()) + L", only standard and multi are supported";
		return NULL;
	}
	return mtl2;
}

int getSubMaterialIndex(Mtl* mtl,Mtl* sub_mtl)
{
	if(mtl == sub_mtl)
		return 0;
	if(!mtl || !sub_mtl)
		return 0;
	if(!mtl->NumSubMtls())
		return 0;
	int i;
	for(i = 0;i < mtl->NumSubMtls(); ++i)
		if(mtl->GetSubMtl(i) == sub_mtl)
			return i;
	return 0;
}

int getSubMaterialCount(Mtl* mtl)
{
	if(!mtl)
		return 0;
	return max(mtl->NumSubMtls(), 1);
}

//int getRNormalCount(Mesh& mesh,int vertex)
//{
//	RVertex& rv=mesh.getRVert(vertex);
//	return rv.rFlags & NORCT_MASK;
//}
//
//RNormal& getRNormal(Mesh& mesh,int vertex,int index)
//{
//	RVertex& rv=mesh.getRVert(vertex);
//	int normals=rv.rFlags & NORCT_MASK;
//
//	if(normals==1)
//		return rv.rn;
//	else
//		return rv.ern[index];
//}
//
//Point3 getRNormal(Mesh& mesh,int vertex,DWORD smgroup,INode* node)
//{
//	RVertex& rv=mesh.getRVert(vertex);
//	int normals=rv.rFlags & NORCT_MASK;
//
//	if(normals==1)
//		return rv.rn.getNormal();
//	else
//		for(int i=0;i<normals;++i)
//			if(rv.ern[i].getSmGroup()==smgroup)
//				return rv.ern[i].getNormal();
//	syslog<<FORMATW(L"unknown smoothing group found in object: %s, vertex: %d, sm group: %d", MBSTOWCS(node->GetName()).c_str(), vertex,smgroup);
//	return Point3(0,0,1);
//}

Color getMtlDiffuseColor(StdMat* mtl, TimeValue t);

Color getMtlAmbientColor(StdMat* mtl, TimeValue t)
{
	//if(mtl->GetAmbDiffTexLock())
	//{
	//	if(mtl->GetSubTexmap(ID_DI) && mtl->GetSubTexmap(ID_DI)->ClassID() == Class_ID(BMTEX_CLASS_ID, 0) && ExportOptions::export_white_ambient_diffuse_color)
	//		return Color(1.0f, 1.0f, 1.0f);
	//	return mtl->GetDiffuse(t);
	//}
	//return mtl->GetAmbient(t);
	if(mtl->GetAmbDiffTexLock())
		return getMtlDiffuseColor(mtl, t);

	Texmap* texmap = mtl->GetSubTexmap(ID_AM);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get ambient color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				IParamBlock2* pb = texmap->GetParamBlock(0);
				int index;
				if(texmap->GetSubTexmap(0))
				{
					if((index = pb->GetDesc()->NameToIndex("color2")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
				else
				{
					if((index = pb->GetDesc()->NameToIndex("color1")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
			}
		}
	}
	if(texmap && ExportOptions::export_white_ambient_diffuse_color)
		return Color(1.0f, 1.0f, 1.0f);
	return mtl->GetAmbient(t);
}

Color getMtlDiffuseColor(StdMat* mtl, TimeValue t)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_DI);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get diffuse color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				IParamBlock2* pb = texmap->GetParamBlock(0);
				int index;
				if(texmap->GetSubTexmap(0))
				{
					if((index = pb->GetDesc()->NameToIndex("color2")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
				else
				{
					if((index = pb->GetDesc()->NameToIndex("color1")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
			}
		}
	}
	if(texmap && ExportOptions::export_white_ambient_diffuse_color)
		return Color(1.0f, 1.0f, 1.0f);
	return mtl->GetDiffuse(t);
}

Color getMtlSpecularColor(StdMat* mtl, TimeValue t)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_SP);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get specular color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				IParamBlock2* pb = texmap->GetParamBlock(0);
				int index;
				if(texmap->GetSubTexmap(0))
				{
					if((index = pb->GetDesc()->NameToIndex("color2")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
				else
				{
					if((index = pb->GetDesc()->NameToIndex("color1")) >= 0)
						return pb->GetColor(pb->IndextoID(index), t);
				}
			}
		}
	}
	if(texmap && ExportOptions::export_white_specular_color)
		return Color(1.0f, 1.0f, 1.0f);
	return mtl->GetSpecular(t);
}

float getMtlOpacity(StdMat* mtl, TimeValue t)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_OP);
	if(texmap && texmap->ClassID() == Class_ID(OUTPUT_CLASS_ID, 0))
	{
		Animatable* anim = getSubAnim(texmap, L"Output");
		if(anim)
		{
			TextureOutput* output = (TextureOutput*)anim;
			if(output)
				return ((StdTexoutGen*)output)->GetOutAmt(t);
		}
	}
	return mtl->GetOpacity(t);
}

Animatable* getMtlAmbientColorAnimatable(StdMat* mtl)
{
	//return getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Ambient Color");
	Texmap* texmap = mtl->GetSubTexmap(ID_AM);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get ambient color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				if(texmap->GetSubTexmap(0))
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 2");
				else
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 1");
			}
		}
	}
	return getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Ambient Color");
}

Animatable* getMtlDiffuseColorAnimatable(StdMat* mtl)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_DI);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get diffuse color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				if(texmap->GetSubTexmap(0))
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 2");
				else
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 1");
			}
		}
	}
	return getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Diffuse Color");
}

Animatable* getMtlSpecularColorAnimatable(StdMat* mtl)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_SP);
	if(texmap && texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
	{
		if(texmap->NumSubTexmaps() >= 2)
		{
			if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
			{
				// ambiguous use, report
				syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get specular color";
			}
			if(texmap->NumParamBlocks() >= 1)
			{
				listAnimatables(getSubAnim(texmap, L"Parameters"));

				if(texmap->GetSubTexmap(0))
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 2");
				else
					return getSubAnim(getSubAnim(texmap, L"Parameters"), L"Color 1");
			}
		}
	}
	return getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Specular Color");
}

Animatable* getMtlOpacityAnimatable(StdMat* mtl)
{
	Texmap* texmap = mtl->GetSubTexmap(ID_OP);
	if(texmap && texmap->ClassID() == Class_ID(OUTPUT_CLASS_ID, 0))
	{
		Animatable* anim = getSubAnim(texmap, L"Output");
		if(anim)
		{
			TextureOutput* output = (TextureOutput*)anim;
			if(output)
				return getSubAnim(getSubAnim(output, L"Parameters"), L"Output Amount");
		}
	}
	return getSubAnim(getSubAnim(mtl, L"Extended Parameters"), L"Opacity");
}

Texmap* findTexmap(Mtl* mtl, int texmap_idx)
{
	if(!mtl)
		return NULL;
	if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0))
	{
		syslog << wcs(L"unsupported material: ") + MBSTOWCS((char*)mtl->GetName()) + L", only standard and multi are supported";
		return NULL;
	}
	if(!mtl->SubTexmapOn(texmap_idx))
		return NULL;
	Texmap* texmap = mtl->GetSubTexmap(texmap_idx);
	if(!texmap)
		return NULL;
	if(texmap_idx == ID_DI || texmap_idx == ID_AM || texmap_idx == ID_SP)
	{
		if(texmap->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
		{
			if(texmap->NumSubTexmaps() >= 2)
			{
				if(texmap->GetSubTexmap(0) && texmap->GetSubTexmap(1))
				{
					// ambiguous use, report
					syslog << MBSTOWCS(mtl->GetName().data()) << L": ambiguous use found, when trying to get a bitmap texture from a rgb multiply texture";
				}
				if(texmap->GetSubTexmap(0))
					return texmap->GetSubTexmap(0);
				else if(texmap->GetSubTexmap(1))
					return texmap->GetSubTexmap(1);
			}
			return NULL;
		}
	}
	if(texmap_idx == ID_OP)
	{
		if(texmap->ClassID() == Class_ID(OUTPUT_CLASS_ID, 0))
		{
			if(texmap->NumSubTexmaps() >= 1)
			{
				return texmap->GetSubTexmap(0);
			}
			return NULL;
		}
	}
	if(texmap_idx == ID_BU)
	{
		if(!isNormalBump(mtl))
			return NULL;
		return texmap->GetSubTexmap(0);
	}
	//if(texmap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
		//return NULL;
	return texmap;
}

bool findTexMapChannel(Mtl* mtl, int texmap_idx, int& channel)
{
	if(!mtl)
		return NULL;
	Texmap* texmap = findTexmap(mtl,texmap_idx);
	if(!texmap)
		return false;
	if(texmap->GetUVWSource() != UVWSRC_EXPLICIT)
		return false;
	channel = texmap->GetMapChannel();
	return true;
}

bool haveMapChannel(Mesh& mesh, int channel)
{
	//syslog_stl<<FORMATW(L"finding channel: %d",channel);
	if(channel >= mesh.getNumMaps())
		return false;
	if(!mesh.mapSupport(channel))
		return false;
	if(!mesh.Map(channel).getNumFaces() || !mesh.Map(channel).getNumVerts())
		return false;
	if(mesh.Map(channel).getNumFaces() != mesh.getNumFaces())
		return false;
	return true;
}

Point3 getMapVertex(Mesh& mesh, int channel, int vertex)
{
	MeshMap& map = mesh.Map(channel);
	return map.tv[vertex];
}

void markUsedMapVertices(Mesh& mesh,Mtl* mtl, int texmap_idx, const vector<int>& array_face, UniqueMap<__int64>& used_map_vertex_idx)
{
	int j,mapchannel;
	if(findTexMapChannel(mtl,texmap_idx,mapchannel))
		if(haveMapChannel(mesh,mapchannel))
		{
			UVVert* tv=mesh.Map(mapchannel).tv;
			TVFace* tf=mesh.Map(mapchannel).tf;
			for(j=0;j<(int)array_face.size();++j)
			{
				TVFace& f=tf[array_face[j]];
				used_map_vertex_idx[(((__int64)mapchannel)<<32)|f.t[0]]=true;
				used_map_vertex_idx[(((__int64)mapchannel)<<32)|f.t[1]]=true;
				used_map_vertex_idx[(((__int64)mapchannel)<<32)|f.t[2]]=true;
			}
		}
}

bool isNormalBump(Mtl* mtl)
{
	if(!mtl)
		return NULL;
	if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID, 0))
		return NULL;
	Texmap* texmap = mtl->GetSubTexmap(ID_BU);
	if(!texmap)
		return false;
	if(texmap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
	{
		MSTR s;
		texmap->GetClassName(s);
		if(s == TSTR("Normal Bump"))
			if(texmap->GetSubTexmap(0)->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
				return true;
	}
	return false;
}

fipImage outputTexmap(Texmap* texmap, int tex_dx, int tex_dy, bool mono)
{
	BitmapInfo info;
	info.SetWidth(tex_dx);
	info.SetHeight(tex_dy);
	info.SetCustWidth(tex_dx);
	info.SetCustHeight(tex_dy);
	info.SetType(mono ? BMM_GRAY_16 : BMM_TRUE_64);
	info.SetFlags(info.Flags() | MAP_HAS_ALPHA);
	Bitmap* bm = TheManager->Create(&info);

	texmap->RenderBitmap(g_interface->GetTime(), bm, 100.0f, TRUE);

	fipImage image(FIT_BITMAP, tex_dx, tex_dy, 32);

#include "../z3D/Core/z_off_debug_new.h"

	BMM_Color_64* scanline2 = new BMM_Color_64[tex_dx];

#include "../z3D/Core/z_on_debug_new.h"

	int row, col;
	for(row = 0; row < tex_dy; ++row)
	{
		bm->GetLinearPixels(0, tex_dy - 1 - row, tex_dx, scanline2);
		for(col = 0; col < tex_dx; ++col)
		{
			unsigned char quad[4] = {(unsigned char)(255 * scanline2[col].b / (float)bm->MaxRGBLevel()), (unsigned char)(255 * scanline2[col].g / (float)bm->MaxRGBLevel()), (unsigned char)(255 * scanline2[col].r / (float)bm->MaxRGBLevel()), (unsigned char)(255 * scanline2[col].a / (float)bm->MaxRGBLevel())};
			image.setPixelColor(col, row, (RGBQUAD*)quad); 
		}
	}
	bm->DeleteThis();

	delete[] scanline2;

	if(mono)
		image.convertTo8Bits();
	return image;
}

fipImage outputTexmap(Texmap* texmap, int tex_res, bool mono)
{
	return outputTexmap(texmap, tex_res, tex_res, mono);
}

bool exportNodeMesh(IGameNode* node,MemWriter& mesh_writer,MemWriter& mtl_writer)
{
	mesh_writer.clear();
	mtl_writer.clear();

	wcs output,mtl_output;

	int deleteit;
	TriObject* tri=GetTriObjectFromNode(node->GetMaxNode(),deleteit);
	bool canconvert=tri?true:false;
	if(!canconvert)
		return false;

	//Mat4 os_tm=toMat4(*node->GetMaxNode()->EvalWorldState(0).GetTM());				// object state, not object space

	//Mat4 vert_trans=toMat4(node->GetMaxNode()->GetObjTMAfterWSM(0))*toMat4(node->GetMaxNode()->GetNodeTM(0)).inverse()*Mat4::rotation(Vec3::xAxis,-90);
	Mat4 vert_trans=toMat4(node->GetMaxNode()->GetObjTMAfterWSM(0))*ROTXNEG90_4*getWorldTM(node).inverse();
	Mat3 norm_trans=((Mat3)vert_trans).inverse().transpose();

	//Mat4 vert_trans=toMat4(node->GetMaxNode()->GetObjectTM(0))*toMat4(node->GetMaxNode()->GetNodeTM(0)).inverse()*Mat4::rotation(Vec3::xAxis,-90);
	//Mat3 norm_trans=(Mat3)vert_trans;

	Mat3 test_flip_mat=(Mat3)toMat4(node->GetMaxNode()->GetObjTMAfterWSM(0));
	bool flip=false;
	if((Vec3(test_flip_mat[0])^Vec3(test_flip_mat[1]))*Vec3(test_flip_mat[2])<0)
	{
		flip=true;
	}

	int i,j;

	//Mesh& mesh=tri->mesh;
	Mesh mesh=tri->mesh;

	if(flip)
	{
		for(i=0;i<mesh.getNumFaces();++i)
			mesh.FlipNormal(i);
	}


	vector<Mtl*> face_mtl;
	face_mtl.resize(mesh.getNumFaces());

	UniqueMap<Mtl*> used_mtl_map;

	vector<Mtl*> used_mtl;

	for(i=0;i<mesh.getNumFaces();++i)
	{
		face_mtl[i]=getSubMaterial(getNodeExportMaterial(node),mesh.getFaceMtlIndex(i));
		if(!used_mtl_map.count(face_mtl[i]))
		{
			used_mtl_map[face_mtl[i]]=true;
			used_mtl.push_back(face_mtl[i]);
		}
	}

	if(true)
	{
		Mtl* parent=getNodeExportMaterial(node);
		for(i=0;i<(int)used_mtl.size()-1;++i)
			for(j=i;j<(int)used_mtl.size()-1;++j)
				if(getSubMaterialIndex(parent,used_mtl[j])>getSubMaterialIndex(parent,used_mtl[j+1]))
					swap(used_mtl[j],used_mtl[j+1]);
	}
	map<Mtl*,int> used_mtl_w_index;
	for(i=0;i<(int)used_mtl.size();++i)
		used_mtl_w_index[used_mtl[i]]=i;

	vector< vector<int> > mtl_sorted_face_idx;
	mtl_sorted_face_idx.resize(used_mtl.size());
	for(i=0;i<(int)face_mtl.size();++i)
		mtl_sorted_face_idx[used_mtl_w_index[face_mtl[i]]].push_back(i);

	mesh.buildRenderNormals();

	map<RNormal*,int> map_rnor_noridx;
	vector< Point3 > normals;
	vector< INT3 > face_noridx;
	for(i=0;i<(int)mesh.getNumFaces();++i)
	{
		Face& f=mesh.faces[i];
		int face_idx=-1;
		INT3 ni;
		for(int j=0;j<3;++j)
		{
			RNormal* rnor=NULL;
			if(f.smGroup)
				rnor=getFaceVertexNormal(mesh,i,j);
			if(!rnor)
			{
				if(face_idx==-1)
				{
					face_idx=(int)normals.size();
					normals.push_back(mesh.getFaceNormal(i));
				}
				ni[j]=face_idx;
			}
			else
			{
				if(!map_rnor_noridx.count(rnor))
				{
					map_rnor_noridx[rnor]=(int)normals.size();
					normals.push_back(rnor->getNormal());
				}
				ni[j]=map_rnor_noridx[rnor];
			}
		}
		face_noridx.push_back(ni);
	}

	if(flip)
	{
		for(i=0;i<(int)normals.size();++i)
			normals[i]=Point3(-normals[i].x,-normals[i].y,-normals[i].z);
	}

	UniqueMap<__int64> used_map_vertex_idx;
	for(i=0;i<(int)used_mtl.size();++i)
	{
		markUsedMapVertices(mesh,used_mtl[i],ID_DI,mtl_sorted_face_idx[i],used_map_vertex_idx);
		markUsedMapVertices(mesh,used_mtl[i],ID_BU,mtl_sorted_face_idx[i],used_map_vertex_idx);
		markUsedMapVertices(mesh,used_mtl[i],ID_SI,mtl_sorted_face_idx[i],used_map_vertex_idx);
	}
	map<__int64,int> map_vertex_idx_map=used_map_vertex_idx.toMapWithIndex();
	vector<__int64> map_vertex_idx=used_map_vertex_idx.toVector();

	// actual export begins

	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_verts %d\r\n\r\n",mesh.getNumVerts());
	else
		mesh_writer.write((int)mesh.getNumVerts());
	for(i=0;i<mesh.getNumVerts();++i)
	{
		Point3 p;
		p=mesh.getVert(i);
		Vec3 v(p.x,p.y,p.z);
		v*=vert_trans;
		if(ExportOptions::text_version && false)
			output+=FORMATW(L"set_vert %d %f %f %f\r\n",i,v.x,v.y,v.z);
		else
			mesh_writer.write(&v,sizeof(v));
	}

	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_norms %d\r\n\r\n",normals.size());
	else
		mesh_writer.write((int)normals.size());
	for(i=0;i<(int)normals.size();++i)
	{
		//normals[i].Normalize();
		Vec3 n=Vec3(normals[i].x,normals[i].y,normals[i].z);
		n*=norm_trans;
		n.normalize();
		if(ExportOptions::text_version && false)
			output+=FORMATW(L"set_norm %d %f %f %f\r\n",i,n[0],n[1],n[2]);
		else
			mesh_writer.write(&n,sizeof(n));
	}

	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_map_verts %d\r\n\r\n",map_vertex_idx.size());
	else
		mesh_writer.write((int)map_vertex_idx.size());
	for(i=0;i<(int)map_vertex_idx.size();++i)
	{
		Point3 p=getMapVertex(mesh,(int)(map_vertex_idx[i]>>32),(int)map_vertex_idx[i]);
		Vec2 mv(p.x,p.y);
		if(ExportOptions::text_version && false)
			output+=FORMATW(L"set_map_vert %d %f %f\r\n",i,p.x,p.y);
		else
			mesh_writer.write(&mv,sizeof(mv));
	}

	int total_faces=0;

	for(i=0;i<(int)mtl_sorted_face_idx.size();++i)
		total_faces+=(int)mtl_sorted_face_idx[i].size();

	int tri_idx;
	
	tri_idx=0;
	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_faces %d\r\n\r\n",total_faces);
	else
		mesh_writer.write((int)total_faces);
	for(i=0;i<(int)mtl_sorted_face_idx.size();++i)
		for(j=0;j<(int)mtl_sorted_face_idx[i].size();++j)
		{
			int fi=mtl_sorted_face_idx[i][j];
			Face& f=mesh.faces[fi];
			int n0,n1,n2;

			n0=face_noridx[fi][0];
			n1=face_noridx[fi][1];
			n2=face_noridx[fi][2];

			if(ExportOptions::text_version && false)
				output+=FORMATW(L"set_face %d %d %d %d %d %d %d\r\n",tri_idx,f.v[0],f.v[1],f.v[2],n0,n1,n2);
			else
			{
				mesh_writer.write((int)f.v[0]);
				mesh_writer.write((int)f.v[1]);
				mesh_writer.write((int)f.v[2]);
				mesh_writer.write((int)n0);
				mesh_writer.write((int)n1);
				mesh_writer.write((int)n2);
			}
			++tri_idx;
		}

	int diffuse_faces=0;
	int normal_faces=0;
	int selfill_faces=0;
	for(i=0;i<(int)used_mtl.size();++i)
	{
		int mapchannel;
		if(findTexMapChannel(used_mtl[i],ID_DI,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
				diffuse_faces+=(int)mtl_sorted_face_idx[i].size();
		if(findTexMapChannel(used_mtl[i],ID_BU,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
				normal_faces+=(int)mtl_sorted_face_idx[i].size();
		if(findTexMapChannel(used_mtl[i],ID_SI,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
				normal_faces+=(int)mtl_sorted_face_idx[i].size();
	}

	map<int,int> submesh_diffuse_start;
	map<int,int> submesh_normal_start;
	map<int,int> submesh_selfill_start;

	tri_idx=0;
	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_map_faces %d\r\n\r\n",diffuse_faces+normal_faces+selfill_faces);
	else
		mesh_writer.write((int)(diffuse_faces+normal_faces+selfill_faces));
	for(i=0;i<(int)used_mtl.size();++i)
	{
		submesh_diffuse_start[i]=-1;
		int mapchannel;
		if(findTexMapChannel(used_mtl[i],ID_DI,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
			{
				submesh_diffuse_start[i]=tri_idx;

				int v0,v1,v2;
				for(j=0;j<(int)mtl_sorted_face_idx[i].size();++j)
				{
					TVFace& f=mesh.Map(mapchannel).tf[mtl_sorted_face_idx[i][j]];
					v0=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[0]];
					v1=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[1]];
					v2=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[2]];

					if(ExportOptions::text_version && false)
						output+=FORMATW(L"set_map_face %d %d %d %d\r\n",tri_idx,v0,v1,v2);
					else
					{
						mesh_writer.write((int)v0);
						mesh_writer.write((int)v1);
						mesh_writer.write((int)v2);
					}
					++tri_idx;
				}
			}
	}

	for(i=0;i<(int)used_mtl.size();++i)
	{
		submesh_normal_start[i]=-1;
		int mapchannel;
		if(findTexMapChannel(used_mtl[i],ID_BU,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
			{
				submesh_normal_start[i]=tri_idx;

				int v0,v1,v2;
				for(j=0;j<(int)mtl_sorted_face_idx[i].size();++j)
				{
					TVFace& f=mesh.Map(mapchannel).tf[mtl_sorted_face_idx[i][j]];
					v0=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[0]];
					v1=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[1]];
					v2=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[2]];

					if(ExportOptions::text_version && false)
						output+=FORMATW(L"set_map_face %d %d %d %d\r\n",tri_idx,v0,v1,v2);
					else
					{
						mesh_writer.write((int)v0);
						mesh_writer.write((int)v1);
						mesh_writer.write((int)v2);
					}
					++tri_idx;
				}
			}
	}

	for(i=0;i<(int)used_mtl.size();++i)
	{
		submesh_selfill_start[i]=-1;
		int mapchannel;
		if(findTexMapChannel(used_mtl[i],ID_SI,mapchannel))
			if(haveMapChannel(mesh,mapchannel))
			{
				submesh_selfill_start[i]=tri_idx;

				int v0,v1,v2;
				for(j=0;j<(int)mtl_sorted_face_idx[i].size();++j)
				{
					TVFace& f=mesh.Map(mapchannel).tf[mtl_sorted_face_idx[i][j]];
					v0=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[0]];
					v1=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[1]];
					v2=map_vertex_idx_map[(((__int64)mapchannel)<<32)|f.t[2]];

					if(ExportOptions::text_version && false)
						output+=FORMATW(L"set_map_face %d %d %d %d\r\n",tri_idx,v0,v1,v2);
					else
					{
						mesh_writer.write((int)v0);
						mesh_writer.write((int)v1);
						mesh_writer.write((int)v2);
					}
					++tri_idx;
				}
			}
	}

	tri_idx=0;
	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_submeshes %d\r\n\r\n",mtl_sorted_face_idx.size());
	else
		mesh_writer.write((int)mtl_sorted_face_idx.size());
	for(i=0;i<(int)mtl_sorted_face_idx.size();++i)
	{
		if(ExportOptions::text_version && false)
			//output+=FORMATW(L"set_submesh %d %d %d %d %d %d %d\r\n",i,tri_idx,mtl_sorted_face_idx[i].size(),submesh_diffuse_start[i],submesh_normal_start[i],submesh_selfill_start[i],getSubMaterialIndex(getNodeExportMaterial(node),used_mtl[i]));
			output+=FORMATW(L"set_submesh %d %d %d %d %d %d %d\r\n",i,tri_idx,mtl_sorted_face_idx[i].size(),submesh_diffuse_start[i],submesh_normal_start[i],submesh_selfill_start[i],used_mtl_w_index[used_mtl[i]]);
		else
		{
			mesh_writer.write((int)tri_idx);
			mesh_writer.write((int)mtl_sorted_face_idx[i].size());
			mesh_writer.write((int)submesh_diffuse_start[i]);
			mesh_writer.write((int)submesh_normal_start[i]);
			mesh_writer.write((int)submesh_selfill_start[i]);
			//mesh_writer.write((int)getSubMaterialIndex(getNodeExportMaterial(node),used_mtl[i]));
			mesh_writer.write((int)used_mtl_w_index[used_mtl[i]]);
		}
		tri_idx+=(int)mtl_sorted_face_idx[i].size();
	}

	//mtl_output+=FORMATW(L"resize_mtls %d\r\n\r\n",max(getSubMaterialCount(getNodeExportMaterial(node)),1));
	mtl_output+=FORMATW(L"resize_mtls %d\r\n\r\n",max((int)used_mtl.size(),1));

	//for(i=0;i<(int)max(getSubMaterialCount(getNodeExportMaterial(node)),1);++i)
	for(i=0;i<(int)max((int)used_mtl.size(),1);++i)
	{
		try
		{
			//mtl_output+=FORMATW(L"set_mtl_render_type %d \"%s\"			// \"unlit\", \"lit\", \"transparent\", \"reflection\"\r\n",i,L"unlit");
			//mtl_output+=FORMATW(L"set_mtl_render_method %d \"%s\"			// \"unlit\", \"lit\", \"selfill\", \"toon\", \"reflection\", \"normalmapping\" \r\n",i,L"unlit");
			mtl_output+=FORMATW(L"set_mtl_render_type %d \"%s\"			// \"unlit\", \"lit\", \"transparent\", \"reflection\"\r\n",i,L"lit");
			mtl_output+=FORMATW(L"set_mtl_render_method %d \"%s\"			// \"unlit\", \"lit\", \"selfill\", \"toon\", \"reflection\", \"normal_mapping\", \"sphere_env_mapping\" \r\n",i,L"lit");
			mtl_output+=L"\r\n";
			if(i<(int)used_mtl.size()&&used_mtl[i]!=NULL)
			{
				wcs main_mtl_name=MBSTOWCS((char*)getNodeExportMaterial(node)->GetName());
				while((int)main_mtl_name.find(' ')>=0)
					main_mtl_name[main_mtl_name.find(' ')]='_';

				//Mtl* mtl=getSubMaterial(getNodeExportMaterial(node),i);
				Mtl* mtl=used_mtl[i];

				int submaterial_index = getSubMaterialIndex(getNodeExportMaterial(node),mtl);

				Color c;
				c=mtl->GetAmbient();
				if(ExportOptions::export_white_ambient_diffuse_color&&findTexmap(mtl,ID_DI))
					c[0]=c[1]=c[2]=1.0f;
				mtl_output+=FORMATW(L"set_mtl_ambient %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				c=mtl->GetDiffuse();
				if(ExportOptions::export_white_ambient_diffuse_color&&findTexmap(mtl,ID_DI))
					c[0]=c[1]=c[2]=1.0f;
				mtl_output+=FORMATW(L"set_mtl_diffuse %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				c=mtl->GetSpecular();
				mtl_output+=FORMATW(L"set_mtl_specular %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				mtl_output+=FORMATW(L"set_mtl_specular_level %d %f\r\n",i,mtl->GetShinStr());
				mtl_output+=FORMATW(L"set_mtl_glossiness %d %f\r\n",i,mtl->GetShininess());
				StdMat* smtl=(StdMat*)mtl;
				mtl_output+=FORMATW(L"set_mtl_two_side %d %d\r\n",i,smtl->GetTwoSided());
				mtl_output+=FORMATW(L"set_mtl_blend_type %d \"%s\"\r\n",i,(smtl->GetTransparencyType()==TRANSP_FILTER)?L"filter":((smtl->GetTransparencyType()==TRANSP_ADDITIVE)?L"additive":L"subtractive"));
				mtl_output+=FORMATW(L"set_mtl_opacity %d %f\r\n",i,smtl->GetOpacity(g_interface->GetTime()));
				mtl_output+=FORMATW(L"set_mtl_selfill %d %f\r\n",i,smtl->GetSelfIllum(g_interface->GetTime()));
				Texmap* texmap;
				texmap=findTexmap(mtl,ID_DI);
				if(texmap)
				{
					const int OUTPUT_TEX_RES = 256;

					wcs rel_path;

					Texmap* alpha_texmap=findTexmap(mtl,ID_OP);
					if(!alpha_texmap)
					{
						bool copy_fail=false;
						if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
						{
							wcs src_path=MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
							wcs filename=src_path.substr(src_path.rfind('\\')+1);
							rel_path=ExportOptions::texture_dir+L"/"+filename;

							copy_fail=CopyFileW(src_path.c_str(),(export_texture_dir+filename).c_str(),FALSE)?false:true;
						}
						if(texmap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0)||copy_fail)
						{
							wcs filename=FORMATW(L"%s_%d_diffuse.png",main_mtl_name.c_str(),submaterial_index);
							rel_path=ExportOptions::texture_dir+L"/"+filename;

							fipImage diffuse=outputTexmap(texmap,OUTPUT_TEX_RES,false);

							diffuse.save(WCSTOMBS(export_texture_dir+filename).c_str());
						}
					}
					else
					{
						wcs filename=FORMATW(L"%s_%d_diffuse.png",main_mtl_name.c_str(),submaterial_index);
						rel_path=ExportOptions::texture_dir+L"/"+filename;

						bool done=false;
						while(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0)&&alpha_texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
						{
							fipImage diffuse,alpha;
							if(!diffuse.load(((BitmapTex*)texmap)->GetMapName()))
							{
								syslog<<L"cannot get " + MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
								break;
							}
							if(!alpha.load(((BitmapTex*)alpha_texmap)->GetMapName()))
							{
								syslog<<L"cannot get " + MBSTOWCS(((BitmapTex*)alpha_texmap)->GetMapName());
								break;
							}

							diffuse.convertTo32Bits();
							alpha.convertTo32Bits();

							diffuse.rescale(max(diffuse.getWidth(),alpha.getWidth()),max(diffuse.getHeight(),alpha.getHeight()),FILTER_BOX);
							alpha.rescale(max(diffuse.getWidth(),alpha.getWidth()),max(diffuse.getHeight(),alpha.getHeight()),FILTER_BOX);

							if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
							{
								fipImage diffuse2;
								diffuse.getChannel(diffuse2,FICC_ALPHA);
								diffuse2.convertTo32Bits();
								diffuse=diffuse2;
							}
							if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
							{
								fipImage alpha2;
								alpha.getChannel(alpha2,FICC_ALPHA);
								alpha=alpha2;
							}
							else
								alpha.convertTo8Bits();

							diffuse.setChannel(alpha,FICC_ALPHA);

							diffuse.save(WCSTOMBS(export_texture_dir+filename).c_str());

							done = true;
							break;
						}
						if(!done)
						{
							fipImage diffuse=outputTexmap(texmap,OUTPUT_TEX_RES,false);
							fipImage alpha=outputTexmap(alpha_texmap,OUTPUT_TEX_RES,false);

							if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID,0))
							{
								if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
								{
									fipImage diffuse2;
									diffuse.getChannel(diffuse2,FICC_ALPHA);
									diffuse2.convertTo32Bits();
									diffuse=diffuse2;
								}
							}
							if(alpha_texmap->ClassID()==Class_ID(BMTEX_CLASS_ID,0))
							{
								if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
								{
									fipImage alpha2;
									alpha.getChannel(alpha2,FICC_ALPHA);
									alpha=alpha2;
								}
								else
									alpha.convertTo8Bits();
							}
							else
								alpha.convertTo8Bits();

							diffuse.setChannel(alpha,FICC_ALPHA);

							diffuse.save(WCSTOMBS(export_texture_dir+filename).c_str());
						}
					}

					mtl_output+=FORMATW(L"set_mtl_diffuse_map %d \"%s\"\r\n",i,ESCSTR(rel_path).c_str());
					int filter_type=0;
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
						filter_type=((BitmapTex*)texmap)->GetFilterType()==FILTER_NADA?2:(((BitmapTex*)texmap)->GetFilterType()==FILTER_SAT?1:0);
					if(alpha_texmap)
						filter_type=1;
					mtl_output+=FORMATW(L"set_mtl_diffuse_map_filter %d %d\r\n",i,filter_type);
					int tiling=texmap->GetTextureTiling();
					mtl_output+=FORMATW(L"set_mtl_diffuse_map_u_wrap %d %d\r\n",i,(tiling&U_WRAP)?0:1);
					mtl_output+=FORMATW(L"set_mtl_diffuse_map_v_wrap %d %d\r\n",i,(tiling&V_WRAP)?0:1);
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
					{
						StdUVGen* uvgen=((BitmapTex*)texmap)->GetUVGen();
						if(uvgen)
						{
							//listAnimatables(texmap);
							//listAnimatables(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"));

							writeUVGen(i,L"diffuse",uvgen,mtl_output);
							writeUVGenAnimData(node,i,L"diffuse",texmap,uvgen,mtl_output);
						}
					}
				}
				texmap=findTexmap(mtl,ID_BU);
				if(texmap)
				{
					const int OUTPUT_TEX_RES = 256;

					wcs rel_path;

					bool copy_fail=false;

					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
					{
						wcs src_path=MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
						wcs filename=src_path.substr(src_path.rfind('\\')+1);
						rel_path=ExportOptions::texture_dir+L"/"+filename;

						copy_fail=CopyFileW(src_path.c_str(),(export_texture_dir+filename).c_str(),FALSE)?false:true;
					}
					if(texmap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0)||copy_fail)
					{
						wcs filename=FORMATW(L"%s_%d_normal.png",main_mtl_name.c_str(),submaterial_index);
						rel_path=ExportOptions::texture_dir+L"/"+filename;

						fipImage normal=outputTexmap(texmap,OUTPUT_TEX_RES,false);

						normal.save(WCSTOMBS(export_texture_dir+filename).c_str());
					}

					mtl_output+=FORMATW(L"set_mtl_normal_map %d \"%s\"\r\n",i,ESCSTR(rel_path).c_str());
					int filter_type=0;
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
						filter_type=((BitmapTex*)texmap)->GetFilterType()==FILTER_NADA?2:(((BitmapTex*)texmap)->GetFilterType()==FILTER_SAT?1:0);
					mtl_output+=FORMATW(L"set_mtl_normal_map_filter %d %d\r\n",i,filter_type);
					int tiling=texmap->GetTextureTiling();
					mtl_output+=FORMATW(L"set_mtl_normal_map_u_wrap %d %d\r\n",i,(tiling&U_WRAP)?0:1);
					mtl_output+=FORMATW(L"set_mtl_normal_map_v_wrap %d %d\r\n",i,(tiling&V_WRAP)?0:1);
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
					{
						StdUVGen* uvgen=((BitmapTex*)texmap)->GetUVGen();
						if(uvgen)
						{
							writeUVGen(i,L"normal",uvgen,mtl_output);
							writeUVGenAnimData(node,i,L"normal",texmap,uvgen,mtl_output);
						}
					}
				}
				texmap=findTexmap(mtl,ID_SI);
				if(texmap)
				{
					const int OUTPUT_TEX_RES = 256;

					wcs rel_path;

					bool copy_fail=false;

					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
					{
						wcs src_path=MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
						wcs filename=src_path.substr(src_path.rfind('\\')+1);
						rel_path=ExportOptions::texture_dir+L"/"+filename;

						copy_fail=CopyFileW(src_path.c_str(),(export_texture_dir+filename).c_str(),FALSE)?false:true;
					}
					if(texmap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0)||copy_fail)
					{
						wcs filename=FORMATW(L"%s_%d_selfill.png",main_mtl_name.c_str(),submaterial_index);
						rel_path=ExportOptions::texture_dir+L"/"+filename;

						fipImage selfill=outputTexmap(texmap,OUTPUT_TEX_RES,false);

						selfill.save(WCSTOMBS(export_texture_dir+filename).c_str());
					}

					mtl_output+=FORMATW(L"set_mtl_selfill_map %d \"%s\"\r\n",i,ESCSTR(rel_path).c_str());
					int filter_type=0;
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
						filter_type=((BitmapTex*)texmap)->GetFilterType()==FILTER_NADA?2:(((BitmapTex*)texmap)->GetFilterType()==FILTER_SAT?1:0);
					if(!ExportOptions::force_lightmap_filter_linear)
						mtl_output+=FORMATW(L"set_mtl_selfill_map_filter %d %d\r\n",i,filter_type);
					else
						mtl_output+=FORMATW(L"set_mtl_selfill_map_filter %d %d\r\n",i,1);
					int tiling=texmap->GetTextureTiling();
					mtl_output+=FORMATW(L"set_mtl_selfill_map_u_wrap %d %d\r\n",i,(tiling&U_WRAP)?0:1);
					mtl_output+=FORMATW(L"set_mtl_selfill_map_v_wrap %d %d\r\n",i,(tiling&V_WRAP)?0:1);
					if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
					{
						StdUVGen* uvgen=((BitmapTex*)texmap)->GetUVGen();
						if(uvgen)
						{
							writeUVGen(i,L"selfill",uvgen,mtl_output);
							writeUVGenAnimData(node,i,L"selfill",texmap,uvgen,mtl_output);
						}
					}
				}

				mtl_output+=FORMATW(L"set_mtl_reflect %d %d\r\n",i,smtl->MapEnabled(ID_RL));
				mtl_output+=FORMATW(L"set_mtl_reflect_amount %d %f\r\n",i,smtl->GetTexmapAmt(ID_RL,g_interface->GetTime()));

				//Animatable* a=smtl;
				/*
				Animatable* a=getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters");
				for(j=0;j<a->NumSubs();++j)
					MessageBoxW(NULL,MBSTOWCS(a->SubAnimName(j).data()).c_str(),L"",0);*/

				//listAnimatables(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"));

				Animatable* animatable;
				
				animatable=getSubAnim(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"),L"Ambient Color");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							Color c=smtl->GetAmbient(timevalue);
							writer.write((REAL)c.r);
							writer.write((REAL)c.g);
							writer.write((REAL)c.b);
						}
					};
					wcs name=wcs(L"mtl_am_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_am_controller \"%s\" %d \"%s\"",name.c_str(),i,rel_path.c_str());
				}

				animatable=getSubAnim(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"),L"Diffuse Color");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							Color c=smtl->GetDiffuse(timevalue);
							writer.write((REAL)c.r);
							writer.write((REAL)c.g);
							writer.write((REAL)c.b);
						}
					};
					wcs name=wcs(L"mtl_di_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_di_controller \"%s\" %d \"%s\"",name.c_str(),i,rel_path.c_str());
				}

				animatable=getSubAnim(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"),L"Specular Color");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							Color c=smtl->GetSpecular(timevalue);
							writer.write((REAL)c.r);
							writer.write((REAL)c.g);
							writer.write((REAL)c.b);
						}
					};
					wcs name=wcs(L"mtl_sp_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_sp_controller \"%s\" %d \"%s\"",name.c_str(),i,rel_path.c_str());
				}

				animatable=getSubAnim(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"),L"Specular Level");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							writer.write((REAL)smtl->GetShinStr(timevalue));
						}
					};
					wcs name=wcs(L"mtl_sl_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_sl_controller \"%s\" %d \"%s\"",name.c_str(),i,rel_path.c_str());
				}

				animatable=getSubAnim(getSubAnim(getSubAnim(smtl,L"Shader Basic Parameters"),L"Parameters"),L"Glossiness");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							writer.write((REAL)smtl->GetShininess(timevalue));
						}
					};
					wcs name=wcs(L"mtl_gl_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_gl_controller \"%s\" %d \"%s\"",name.c_str(),i,rel_path.c_str());
				}

				animatable=getSubAnim(getSubAnim(smtl,L"Extended Parameters"),L"Opacity");
				if(animatable)
				{
					class exporter: public AnimDataExporter
					{
					private:
						StdMat*					smtl;
					public:
						exporter(StdMat* m):smtl(m) {}
					public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)
						{
							writer.write((REAL)smtl->GetOpacity(timevalue));
						}
					};
					wcs name=wcs(L"mtl_op_")+MBSTOWCS(node->GetName())+FORMATW(L"_%d",i);
					wcs rel_path=ExportOptions::animdata_dir+L"/"+name+L".bin";
					wcs path=export_animdata_dir + L"/" + name + L".bin";
					if(exportAnimData(GetKeyControlInterface(animatable),exporter(smtl),path))
						mtl_output += FORMATW(L"create_op_controller \"%s\" %d \"%s\"\r\n",name.c_str(),i,rel_path.c_str());
				}
			}
			else
			{
				COLORREF color=node->GetMaxNode()->GetWireColor();
				//mtl_output+=FORMATW(L"set_mtl_ambient %d %f %f %f\r\n",i,0.5f,0.5f,0.5f);
				//mtl_output+=FORMATW(L"set_mtl_diffuse %d %f %f %f\r\n",i,0.5f,0.5f,0.5f);
				mtl_output+=FORMATW(L"set_mtl_ambient %d %f %f %f\r\n",i,(color&0xff)/255.0f,((color>>8)&0xff)/255.0f,((color>>16)&0xff)/255.0f);
				mtl_output+=FORMATW(L"set_mtl_diffuse %d %f %f %f\r\n",i,(color&0xff)/255.0f,((color>>8)&0xff)/255.0f,((color>>16)&0xff)/255.0f);
				mtl_output+=FORMATW(L"set_mtl_specular %d %f %f %f\r\n",i,1.0f,1.0f,1.0f);
				mtl_output+=FORMATW(L"set_mtl_specular_level %d %f\r\n",i,0.0f);
				mtl_output+=FORMATW(L"set_mtl_glossiness %d %f\r\n",i,0.1f);
				mtl_output+=FORMATW(L"set_mtl_two_side %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_blend_type %d \"%s\"\r\n",i,L"filter");
				mtl_output+=FORMATW(L"set_mtl_opacity %d %f\r\n",i,1.0f);
				mtl_output+=FORMATW(L"set_mtl_selfill %d %f\r\n",i,0.0f);
				mtl_output+=FORMATW(L"set_mtl_diffuse_map %d \"\"\r\n",i);
				mtl_output+=FORMATW(L"set_mtl_diffuse_map_filter %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_diffuse_map_u_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_diffuse_map_v_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_normal_map %d \"\"\r\n",i);
				mtl_output+=FORMATW(L"set_mtl_normal_map_filter %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_normal_map_u_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_normal_map_v_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_selfill_map %d \"\"\r\n",i);
				mtl_output+=FORMATW(L"set_mtl_selfill_map_filter %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_selfill_map_u_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_selfill_map_v_wrap %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_reflect %d %d\r\n",i,0);
				mtl_output+=FORMATW(L"set_mtl_reflect_amount %d %f\r\n",i,1.0f);
			}
		}
		catch(...)
		{
			MessageBoxW(NULL,FORMATW(L"caught an exception during exporting %d material",i).c_str(),L"",0);
		}
		mtl_output+=FORMATW(L"set_mtl_layer_index %d %d			// manual transparent drawing order sorting, the lower the closer, integer only\r\n",i,0);
		mtl_output+=FORMATW(L"set_mtl_toon %d %d\r\n",i,ExportOptions::default_toon?1:0);
		mtl_output+=FORMATW(L"set_mtl_toon_map %d \"%s\"\r\n",i,ExportOptions::default_toon_map.c_str());
		mtl_output+=FORMATW(L"set_mtl_toon_outline_width %d %f\r\n",i,ExportOptions::default_toon_outline_width);
		mtl_output+=FORMATW(L"set_mtl_toon_outline_color %d %f %f %f\r\n",i,ExportOptions::default_toon_outline_color_r,ExportOptions::default_toon_outline_color_g,ExportOptions::default_toon_outline_color_b);
		mtl_output+=L"\r\n";
	}
	if(deleteit)
		tri->DeleteMe();

	if(ExportOptions::text_version)
	{
		mesh_writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		mesh_writer.write((void*)output.c_str(),(int)output.length()*2);

		mtl_writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		mtl_writer.write((void*)mtl_output.c_str(),(int)mtl_output.length()*2);
	}
	else
	{
		mtl_writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		mtl_writer.write((void*)mtl_output.c_str(),(int)mtl_output.length()*2);
	}

	return true;
}

map<Object*, wcs> map_mesh_filepath;
map<Mtl*, wcs> map_mtl_filepath;

struct EXPORTED_SKIN
{
	Object*				obj;
	Modifier*			mod;

	bool				operator < (const EXPORTED_SKIN& right) const { return memcmp(this, &right, sizeof(*this)) < 0; }
};
map<EXPORTED_SKIN, wcs> map_skin_filepath;

void clearMeshNodeCache()
{
	map_mesh_filepath.clear();
	map_mtl_filepath.clear();
	map_skin_filepath.clear();
}

class TriObjectDeleter
{
private:
	TriObject*	_tri;
	int			_deleteIt;
public:
	TriObjectDeleter(TriObject* tri,int deleteIt)
	{
		_tri=tri;
		_deleteIt=deleteIt;
	}
	~TriObjectDeleter()
	{
		if(_deleteIt)
			_tri->DeleteMe();
	}
};

class v3cmp	{public:	bool		operator () (const Vec3& e0, const Vec3& e1)	const {	return memcmp(&e0, &e1, sizeof(e0)) < 0;	}};
class v2cmp	{public:	bool		operator () (const Vec2& e0, const Vec2& e1)	const {	return memcmp(&e0, &e1, sizeof(e0)) < 0;	}};

void collectMappedVertices(Mesh& mesh, Mtl* mtl, int texmap_idx, const vector<size_t>& array_face, map<Vec2, size_t, v2cmp>& map_mvert)
{
	if(!mtl)
		return;
	int j, mapchannel;
	if(findTexMapChannel(mtl, texmap_idx, mapchannel) || (texmap_idx == ID_DI && findTexMapChannel(mtl, ID_OP, mapchannel)))
		if(haveMapChannel(mesh, mapchannel))
		{
			UVVert* tv = mesh.Map(mapchannel).tv;
			TVFace* tf = mesh.Map(mapchannel).tf;
			for(j = 0; j < (int)array_face.size(); ++j)
			{
				TVFace& f = tf[array_face[j]];
				for(size_t i = 0; i < 3; ++i)
				{
					const UVVert& uv = tv[f.t[i]];
					Vec2 v(uv.x, uv.y);
					map_mvert[v] = 0;
				}
			}
		}
}

void collectMappedFaces(Mesh& mesh, Mtl* mtl, int texmap_idx, const vector<size_t>& array_face, const map<Vec2, size_t, v2cmp>& map_mvert, vector<INT3>& array_mface)
{
	if(!mtl)
		return;
	int j, mapchannel;
	if(findTexMapChannel(mtl, texmap_idx, mapchannel) || (texmap_idx == ID_DI && findTexMapChannel(mtl, ID_OP, mapchannel)))
		if(haveMapChannel(mesh, mapchannel))
		{
			UVVert* tv = mesh.Map(mapchannel).tv;
			TVFace* tf = mesh.Map(mapchannel).tf;
			for(j = 0; j < (int)array_face.size(); ++j)
			{
				TVFace& f = tf[array_face[j]];
				INT3 f2;
				for(size_t i = 0; i < 3; ++i)
				{
					const UVVert& uv = tv[f.t[i]];
					Vec2 v(uv.x, uv.y);

					Z_VERIFY( map_mvert.count(v) );

					int32_t idx = (int32_t)map_mvert.find(v)->second;
					f2[i] = idx;
				}
				array_mface.push_back(f2);
			}
		}
}

wcs genTextureFilename(const fipImage& img)
{
	//return FORMATW(L"%08x.tga", gen_crc(img.accessPixels(), (int)img.getScanWidth() * img.getHeight()));
	return MBSTOWCS(gen_md5(img.accessPixels(), (int)img.getScanWidth() * img.getHeight())) + L".tga";
}

bool isImageFormatSupported(const wcs& filename)
{
	size_t index = filename.find_last_of('.');
	if(index != wcs::npos)
	{
		wcs ext = filename.substr(index + 1);
		if(ext == L"bmp" || ext == L"jpg" || ext == L"gif" || ext == L"tga" || ext == L"png" || ext == L"psd")
			return true;
	}
	return false;
}

bool exportFileTexture(const wcs& src_path, wcs& exported_filepath)
{
	wcs src_path2 = REPLACE(src_path, L"/", L"\\");
	wcs filename = src_path2.substr(src_path2.rfind('\\') + 1);

	wcs dst_filename = filename;

	bool convert = false;
	bool down_scale = false;
	bool pow2ize = false;

	if(ExportOptions::convert_psd_to_tga)
	{
		if(TOLOWER(dst_filename.substr(dst_filename.rfind('.') + 1)) == L"psd")
		{
			dst_filename = dst_filename.substr(0, dst_filename.rfind('.') + 1) + L"tga";
			convert = true;
		}
	}

	exported_filepath = getTextureDirREL() + L"/" + dst_filename;

	wcs abs_exported_filepath = export_base_dir + L"/" + exported_filepath;

	if(src_path2.find(':') == wcs::npos || !PathFileExistsW(src_path2.c_str()))
	{
		wcs cur_filedir = MBSTOWCS(g_interface->GetCurFilePath().data());
		cur_filedir = cur_filedir.substr(0, cur_filedir.rfind('\\') + 1);
		src_path2 = cur_filedir + filename;
	}
	if(PathFileExistsW(src_path2.c_str()))
	{
		fipImage img;
		if(img.loadU(src_path2.c_str()))
		{
			if((uint32_t)img.getWidth() > ExportOptions::max_tex_size || (uint32_t)img.getHeight() > ExportOptions::max_tex_size)
				down_scale = true;
			if(ExportOptions::make_tex_size_power_of_2 && (!IS_POW2((uint32_t)img.getWidth()) || !IS_POW2((uint32_t)img.getHeight())))
				pow2ize = true;
		}
	}

	if(!convert && !down_scale && !pow2ize)
		return !!CopyFileW(src_path2.c_str(), abs_exported_filepath.c_str(), FALSE);
	else
	{
		fipImage img;
		if(!img.loadU(src_path2.c_str()))
			return false;

		if(down_scale || pow2ize)
		{
			uint32_t dx = (uint32_t)img.getWidth();
			uint32_t dy = (uint32_t)img.getHeight();

			if(down_scale)
			{
				if(dx > ExportOptions::max_tex_size)
				{
					REAL ratio = (REAL)ExportOptions::max_tex_size / dx;
					dx = ExportOptions::max_tex_size;
					dy = (uint32_t)(dy * ratio);
				}
				if(dy > ExportOptions::max_tex_size)
				{
					REAL ratio = (REAL)ExportOptions::max_tex_size / dy;
					dy = ExportOptions::max_tex_size;
					dx = (uint32_t)(dx * ratio);
				}
			}
			if(pow2ize)
			{
				dx = min(NEXT_POW2(dx), NEXT_POW2(ExportOptions::max_tex_size));
				dy = min(NEXT_POW2(dy), NEXT_POW2(ExportOptions::max_tex_size));
			}

			img.rescale(dx, dy, FILTER_BILINEAR);
		}

		img.saveU(abs_exported_filepath.c_str());
		return true;
	}
	return false;
}

mbs getBitmapTexMapName(BitmapTex* texmap)
{
#ifdef MAX_RELEASE_R12
	return IFileResolutionManager::GetInstance()->GetFullFilePath(texmap->GetMapName(), MaxSDK::AssetManagement::kBitmapAsset).data();
#else
	return IPathConfigMgr::GetPathConfigMgr()->GetFullFilePath(texmap->GetMapName(), false).data();
#endif
}

wcs getBitmapTexMapNameW(BitmapTex* texmap)
{
	return MBSTOWCS(getBitmapTexMapName(texmap));
}

bool exportTexture(Texmap* texmap, wcs& exported_filepath)
{
	const int OUTPUT_TEX_RES = max(1, min(ExportOptions::max_tex_size, ExportOptions::generated_tex_size));

	bool copy_fail = false;

	if(texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
	{
		wcs src_path = getBitmapTexMapNameW((BitmapTex*)texmap);

		return exportFileTexture(src_path, exported_filepath);
	}
	else
	{
		fipImage img = outputTexmap(texmap, OUTPUT_TEX_RES, false);

		wcs filename = genTextureFilename(img);
		exported_filepath = getTextureDirREL() + L"/" + filename;

		img.saveU((getTextureDirABS() + L"/" + filename).c_str());
	}

	return true;
}

bool exportDiffuseTexture(Mtl* mtl, wcs& exported_filepath)
{
	Texmap* texmap = findTexmap(mtl, ID_DI);
	Texmap* alpha_texmap = findTexmap(mtl, ID_OP);
	if(texmap)
	{
		const int OUTPUT_TEX_RES = max(1, min(ExportOptions::max_tex_size, ExportOptions::generated_tex_size));

		wcs rel_path;

		if(!alpha_texmap)
		{
			bool copy_fail = false;
			if(texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				wcs src_path = getBitmapTexMapNameW((BitmapTex*)texmap);

				return exportFileTexture(src_path, exported_filepath);
			}
			else
			{
				fipImage diffuse = outputTexmap(texmap, OUTPUT_TEX_RES, false);

				wcs filename = genTextureFilename(diffuse);
				exported_filepath = getTextureDirREL() + L"/" + filename;

				BOOL ret = diffuse.saveU((getTextureDirABS() + L"/" + filename).c_str());

				return true;
			}
			return !copy_fail;
		}
		else
		{
			bool done = false;
			while(texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0) && alpha_texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				if(getBitmapTexMapName((BitmapTex*)texmap) == getBitmapTexMapName((BitmapTex*)alpha_texmap))
				{
					if(!((BitmapTex*)texmap)->GetAlphaAsRGB(false) && ((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
					{
						wcs src_path = getBitmapTexMapNameW((BitmapTex*)texmap);

						if(exportFileTexture(src_path, exported_filepath))
						{
							done = true;
							break;
						}
					}
				}

				fipImage diffuse, alpha;
				if(!diffuse.loadU(getBitmapTexMapNameW((BitmapTex*)texmap).c_str()))
				{
					syslog << L"cannot get " + MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
					break;
				}

				if(!alpha.loadU(getBitmapTexMapNameW((BitmapTex*)alpha_texmap).c_str()))
				{
					syslog << L"cannot get " + MBSTOWCS(((BitmapTex*)alpha_texmap)->GetMapName());
					break;
				}

				diffuse.convertTo32Bits();
				alpha.convertTo32Bits();

				diffuse.rescale(max(diffuse.getWidth(), alpha.getWidth()), max(diffuse.getHeight(), alpha.getHeight()), FILTER_BOX);
				alpha.rescale(max(diffuse.getWidth(), alpha.getWidth()), max(diffuse.getHeight(), alpha.getHeight()), FILTER_BOX);

				if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
				{
					fipImage diffuse2;
					diffuse.getChannel(diffuse2, FICC_ALPHA);
					diffuse2.convertTo32Bits();
					diffuse = diffuse2;
				}
				if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
				{
					fipImage alpha2;
					alpha.getChannel(alpha2, FICC_ALPHA);
					alpha = alpha2;
				}
				else
				{
					alpha.convertTo24Bits();
					alpha.convertTo8Bits();
				}

				diffuse.setChannel(alpha, FICC_ALPHA);

				wcs filename = genTextureFilename(diffuse);
				exported_filepath = getTextureDirREL() + L"/" + filename;

				diffuse.saveU((getTextureDirABS() + L"/" + filename).c_str());

				done = true;
				break;
			}
			if(!done)
			{
				fipImage diffuse = outputTexmap(texmap, OUTPUT_TEX_RES, false);
				fipImage alpha = outputTexmap(alpha_texmap, OUTPUT_TEX_RES, false);

				if(texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
				{
					if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
					{
						fipImage diffuse2;
						diffuse.getChannel(diffuse2, FICC_ALPHA);
						diffuse2.convertTo32Bits();
						diffuse = diffuse2;
					}
				}
				if(alpha_texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
				{
					if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
					{
						fipImage alpha2;
						alpha.getChannel(alpha2, FICC_ALPHA);
						alpha = alpha2;
					}
					else
						alpha.convertTo8Bits();
				}
				else
					alpha.convertTo8Bits();

				diffuse.setChannel(alpha, FICC_ALPHA);

				wcs filename = genTextureFilename(diffuse);
				exported_filepath = getTextureDirREL() + L"/" + filename;

				diffuse.saveU((getTextureDirABS() + L"/" + filename).c_str());
			}
		}
	}
	else if(alpha_texmap)
	{
		const int OUTPUT_TEX_RES = max(1, min(ExportOptions::max_tex_size, ExportOptions::generated_tex_size));

		bool copy_fail = true;
		if(alpha_texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
		{
			//fipImage img;
			//if(img.loadU(getBitmapTexMapNameW((BitmapTex*)alpha_texmap).c_str()))
			//{
			//	if(true)
			//	{
			//		fipImage alpha;
			//		alpha = img;
			//		alpha.convertTo8Bits();
			//		img.setSize(FIT_BITMAP, img.getWidth(), img.getHeight(), 32);
			//		memset(img.accessPixels(), 0xff, (DWORD)img.getScanWidth() * img.getHeight());
			//		img.setChannel(alpha, FICC_ALPHA);
			//	}

			//	wcs filename = genTextureFilename(img);
			//	
			//	exported_filepath = getTextureDirREL() + L"/" + filename;

			//	copy_fail = !img.saveU((getTextureDirABS() + L"/" + filename).c_str());
			//}
			if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
			{
				wcs src_path = getBitmapTexMapNameW((BitmapTex*)alpha_texmap);

				copy_fail = !exportFileTexture(src_path, exported_filepath);
			}
		}
		if(alpha_texmap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0) || copy_fail)
		{
			fipImage img = outputTexmap(alpha_texmap, OUTPUT_TEX_RES, false);
			if(true)
			{
				fipImage alpha;
				img.getChannel(alpha, FICC_RED);
				img.setSize(FIT_BITMAP, img.getWidth(), img.getHeight(), 32);
				memset(img.accessPixels(), 0xff, (DWORD)img.getScanWidth() * img.getHeight());
				img.setChannel(alpha, FICC_ALPHA);
			}

			wcs filename = genTextureFilename(img);
			exported_filepath = getTextureDirREL() + L"/" + filename;

			BOOL ret = img.saveU((getTextureDirABS() + L"/" + filename).c_str());

			return true;
		}
		return !copy_fail;
	}
	return true;
}

void writeTextureInfo(zTiXmlElement* e, Texmap* texmap)
{
	TimeValue cur_time = GetCOREInterface()->GetTime();

	if(texmap)
		texmap = (texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) ? texmap : NULL;
	StdUVGen* uvgen = texmap ? ((BitmapTex*)texmap)->GetUVGen() : NULL;

	e->SetAttribute("channel", texmap ? (int32_t)(texmap->GetMapChannel() - 1) : (int32_t)0);
	if(texmap)
	{
		switch(((BitmapTex*)texmap)->GetFilterType())
		{
		case FILTER_NADA:
			e->SetAttribute("min_filter", "NEAREST");
			e->SetAttribute("mag_filter", "NEAREST");
			e->SetAttribute("mip_filter", "NONE");
			break;
		case FILTER_SAT:
			e->SetAttribute("min_filter", "LINEAR");
			e->SetAttribute("mag_filter", "LINEAR");
			e->SetAttribute("mip_filter", "NONE");
			break;
		case FILTER_PYR:
		default:
			e->SetAttribute("min_filter", "LINEAR");
			e->SetAttribute("mag_filter", "LINEAR");
			e->SetAttribute("mip_filter", "LINEAR");
			break;
		}
	}
	else
	{
		e->SetAttribute("min_filter", "LINEAR");
		e->SetAttribute("mag_filter", "LINEAR");
		e->SetAttribute("mip_filter", "LINEAR");
	}
	if(uvgen)
		e->SetAttribute("u_wrap", (uvgen->GetTextureTiling() & U_WRAP) ? ((uvgen->GetTextureTiling() & U_MIRROR) ? "MIRROR" : "REPEAT") : ((uvgen->GetTextureTiling() & U_MIRROR) ? "MIRROR_ONCE" : "CLAMP"));
	else
		e->SetAttribute("u_wrap", "REPEAT");

	if(uvgen)
		e->SetAttribute("v_wrap", (uvgen->GetTextureTiling() & V_WRAP) ? ((uvgen->GetTextureTiling() & V_MIRROR) ? "MIRROR" : "REPEAT") : ((uvgen->GetTextureTiling() & V_MIRROR) ? "MIRROR_ONCE" : "CLAMP"));
	else
		e->SetAttribute("v_wrap", "REPEAT");

	e->SetAttribute("w_wrap", "REPEAT");

	if(uvgen)
	{
		e->SetAttribute("u_offset", uvgen->GetUOffs(cur_time));
		e->SetAttribute("v_offset", uvgen->GetVOffs(cur_time));
		e->SetAttribute("u_tiling", uvgen->GetUScl(cur_time));
		e->SetAttribute("v_tiling", uvgen->GetVScl(cur_time));
		e->SetAttribute("u_angle", uvgen->GetUAng(cur_time));
		e->SetAttribute("v_angle", uvgen->GetVAng(cur_time));
		e->SetAttribute("w_angle", uvgen->GetWAng(cur_time));
	}
	else
	{
		e->SetAttribute("u_offset", 0.0f);
		e->SetAttribute("v_offset", 0.0f);
		e->SetAttribute("u_tiling", 1.0f);
		e->SetAttribute("v_tiling", 1.0f);
		e->SetAttribute("u_angle", 0.0f);
		e->SetAttribute("v_angle", 0.0f);
		e->SetAttribute("w_angle", 0.0f);
	}
}

void writeUVGenAnimData(zTiXmlElement* e, Texmap* texmap, KFWriter& kfwriter, const wcs& animdata_misc_path)
{
	StdUVGen* uvgen;
	if(texmap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0) || !(uvgen = ((BitmapTex*)texmap)->GetUVGen()))
		return;

	class uoff: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		uoff(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetUOffs(t);	}
	};
	class voff: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		voff(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetVOffs(t);	}
	};
	class uscl: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		uscl(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetUScl(t);	}
	};
	class vscl: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		vscl(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetVScl(t);	}
	};
	class uang: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		uang(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetUAng(t);	}
	};
	class vang: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		vang(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetVAng(t);	}
	};
	class wang: public KeyGetter<REAL>
	{
	public:
		StdUVGen*					gen;
		wang(StdUVGen* gen_):gen(gen_) {}
		REAL						get(TimeValue t)			{	return gen->GetWAng(t);	}
	};

	size_t chunk_idx;

	Animatable* animatable = NULL;

	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"U Offset");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, uoff(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("u_offset"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"V Offset");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, voff(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("v_offset"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"U Tiling");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, uscl(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("u_tiling"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"V Tiling");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, vscl(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("v_tiling"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"U Angle");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, uang(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("u_angle"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"V Angle");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, vang(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("v_angle"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
	animatable = getSubAnim(getSubAnim(getSubAnim(texmap, L"Coordinates"), L"Parameters"), L"W Angle");
	if(animatable)
	{
		if(kfwriter.exportChunk(animatable, wang(uvgen), chunk_idx))
		{
			zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("w_angle"));
			e2.SetAttribute("src", animdata_misc_path);
			e2.SetAttribute("chunk", (int32_t)chunk_idx);
		}
	}
}

bool exportMaterial(IGameNode* gamenode, wcs& exported_mtl, KFWriter& kfwriter, const wcs& animdata_misc_path)
{
	Mtl* main_mtl = getNodeExportMaterial(gamenode);

	if(main_mtl && map_mtl_filepath.count(main_mtl))
	{
		exported_mtl = map_mtl_filepath[main_mtl];
		return true;
	}

	TimeValue cur_time = GetCOREInterface()->GetTime();

	TiXmlDocument doc;
	doc.InsertEndChild(TiXmlDeclaration("1.0", "utf-8", ""));

	TiXmlHandle root(doc.InsertEndChild(TiXmlElement("mtls")));

	size_t num_mtls = max(1, getSubMaterialCount(main_mtl));
	for(size_t i = 0; i < num_mtls; ++i)
	{
		Mtl* m = getSubMaterial(main_mtl, (MtlID)i);
		zTiXmlElement* e;

		zTiXmlElement& ent_mtl = *(zTiXmlElement*)root.ToElement()->InsertEndChild(TiXmlElement("mtl"));

		if(m && m->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
		{
			StdMat* mtl = (StdMat*)m;

			ent_mtl.SetAttribute("ref_name", mtl->GetName().data());

			Config cfg(L"");
			getMaterialExtendedProperties(mtl, cfg);

			bool transparent = (mtl->GetOpacity(cur_time) != 1.0f || mtl->GetTransparencyType() != TRANSP_FILTER || mtl->SubTexmapOn(ID_FI));
			if(cfg.exists(L"blend_override"))
			{
				switch(cfg.getInt32(L"blend_override"))
				{
				case 0:		// auto
					break;
				case 1:		// enable
					transparent = true;
					break;
				case 2:		// disable
					transparent = false;
					break;
				}
			}

			if(transparent)
				ent_mtl.SetAttribute("type", "transparent");
			else
				ent_mtl.SetAttribute("type", "solid");

			ent_mtl.SetAttribute("method", "standard");

			Color c;
			Texmap* texmap;

			c = getMtlAmbientColor(mtl, cur_time);
			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("ambient"));
			e->SetAttribute("value", Vec3(c.r, c.g, c.b));

			c = getMtlDiffuseColor(mtl, cur_time);
			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("diffuse"));
			e->SetAttribute("value", Vec3(c.r, c.g, c.b));

			c = getMtlSpecularColor(mtl, cur_time);
			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("specular"));
			e->SetAttribute("value", Vec3(c.r, c.g, c.b));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("specular_level"));
			e->SetAttribute("value", mtl->GetShinStr(cur_time));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("glossiness"));
			e->SetAttribute("value", mtl->GetShininess(cur_time));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("opacity"));
			e->SetAttribute("value", getMtlOpacity(mtl, cur_time));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("lighting"));
			e->SetAttribute("value", mtl->GetSelfIllum(cur_time) == 0.0f && !findTexmap(mtl, ID_SI));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("two_sided"));
			e->SetAttribute("value", !!mtl->GetTwoSided());

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("alpha_test"));
			if(cfg.exists(L"alpha_test_override"))
			{
				switch(cfg.getInt32(L"alpha_test_override"))
				{
				case 0:		// auto
					e->SetAttribute("enabled", "AUTO");
					break;
				case 1:		// enable
					e->SetAttribute("enabled", 1);
					break;
				case 2:		// disable
					e->SetAttribute("enabled", 0);
					break;
				}
			}
			else
				e->SetAttribute("enabled", "AUTO");
			e->SetAttribute("func", "GREATER");
			if(cfg.exists(L"alpha_test_ref"))
			{
				e->SetAttribute("ref", cfg.getFloat(L"alpha_test_ref"));
			}
			else
			{
				if(!transparent)
					e->SetAttribute("ref", 13.0f / 255);
				else
					e->SetAttribute("ref", 0.0f / 255);
			}

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("blend"));
			if(transparent)
			{
				e->SetAttribute("enabled", true);
				switch(mtl->GetTransparencyType())
				{
				case TRANSP_ADDITIVE:
					e->SetAttribute("op", "ADD");
					e->SetAttribute("sbf", "SRC_ALPHA");
					e->SetAttribute("dbf", "ONE");
					break;
				case TRANSP_SUBTRACTIVE:
					e->SetAttribute("op", "SUBTRACT");
					e->SetAttribute("sbf", "SRC_ALPHA");
					e->SetAttribute("dbf", "ONE");
					break;
				case TRANSP_FILTER:
				default:
					e->SetAttribute("op", "ADD");
					e->SetAttribute("sbf", "SRC_ALPHA");
					e->SetAttribute("dbf", "ONE_MINUS_SRC_ALPHA");
					break;
				}
			}
			else
			{
				e->SetAttribute("enabled", false);

				e->SetAttribute("op", "ADD");
				e->SetAttribute("sbf", "SRC_ALPHA");
				e->SetAttribute("dbf", "ONE_MINUS_SRC_ALPHA");
			}

			if(cfg.exists(L"blend_mode_override"))
			{
				switch(cfg.getInt32(L"blend_mode_override"))
				{
				case 0:		// auto
					break;
				case 1:
					{
						if(true)
						{
							int32_t op = cfg.getInt32(L"blend_op");
							if(op < 0 || op >= 5)
								op = 0;
							const char* blend_op_str[5] = {
								"ADD",
								"SUBTRACT",
								"REVERSE_SUBTRACT",
								"MIN",
								"MAX"
							};
							e->SetAttribute("op", blend_op_str[op]);
						}
						if(true)
						{
							int32_t sbf = cfg.getInt32(L"src_blend_factor");
							int32_t dbf = cfg.getInt32(L"dst_blend_factor");
							if(sbf < 0 || sbf >= 11)
								sbf = 1;
							if(dbf < 0 || dbf >= 11)
								dbf = 0;
							const char* blend_factor_str[11] = {
								"ZERO",
								"ONE",
								"SRC_COLOR",
								"ONE_MINUS_SRC_COLOR",
								"SRC_ALPHA",
								"ONE_MINUS_SRC_ALPHA",
								"DEST_ALPHA",
								"ONE_MINUS_DEST_ALPHA",
								"DEST_COLOR",
								"ONE_MINUS_DEST_COLOR",
								"SRC_ALPHA_SAT"
							};
							e->SetAttribute("sbf", blend_factor_str[sbf]);
							e->SetAttribute("dbf", blend_factor_str[dbf]);
						}
					}
					break;
				}
			}

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("reflect"));
			e->SetAttribute("value", 0.0f);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("layer_index"));
			if(cfg.exists(L"layer_index"))
				e->SetAttribute("value", (int32_t)cfg.getInt32(L"layer_index"));
			else
				e->SetAttribute("value", (int32_t)0);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("sep_opacity_mapping"));
			e->SetAttribute("value", (int32_t)0);

			if(!ExportOptions::skip_textures)
			{
				texmap = findTexmap(mtl, ID_DI);
				if(texmap || findTexmap(mtl, ID_OP))
				{
					wcs filepath;
					if(exportDiffuseTexture(mtl, filepath))
					{
						Texmap* alpha_texmap = findTexmap(mtl, ID_OP);

						zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("diffuse_map"));

						e2->SetAttribute("src", filepath);

						writeTextureInfo(e2, texmap);
						if(alpha_texmap)
						{
							e2->SetAttribute("min_filter", "LINEAR");
							e2->SetAttribute("mag_filter", "LINEAR");
							e2->SetAttribute("mip_filter", "NONE");
						}
						if(cfg.exists(L"diffuse_filter_override"))
						{
							switch(cfg.getInt32(L"diffuse_filter_override"))
							{
							case 0:		// auto
								break;
							case 1:		// linear - linear - linear
								e2->SetAttribute("min_filter", "LINEAR");
								e2->SetAttribute("mag_filter", "LINEAR");
								e2->SetAttribute("mip_filter", "LINEAR");
								break;
							case 2:		// linear - linear - none
								e2->SetAttribute("min_filter", "LINEAR");
								e2->SetAttribute("mag_filter", "LINEAR");
								e2->SetAttribute("mip_filter", "NONE");
								break;
							case 3:		// nearest - nearest - none
								e2->SetAttribute("min_filter", "NEAREST");
								e2->SetAttribute("mag_filter", "NEAREST");
								e2->SetAttribute("mip_filter", "NONE");
								break;
							}
						}

						if(texmap)
						{
							zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
							writeUVGenAnimData(e3, texmap, kfwriter, animdata_misc_path);
							if(!e3->FirstChild())
								e2->RemoveChild(e3);
						}
						else if(alpha_texmap)
						{
							zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
							writeUVGenAnimData(e3, alpha_texmap, kfwriter, animdata_misc_path);
							if(!e3->FirstChild())
								e2->RemoveChild(e3);
						}
					}
				}

				if(cfg.exists(L"sep_opacity_mapping") && cfg.getInt32(L"sep_opacity_mapping"))
				{
					texmap = findTexmap(mtl, ID_OP);
					if(texmap)
					{
						wcs filepath;
						if(exportTexture(texmap, filepath))
						{
							zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("opacity_map"));

							e2->SetAttribute("src", filepath);

							writeTextureInfo(e2, texmap);

							zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
							writeUVGenAnimData(e3, texmap, kfwriter, animdata_misc_path);
							if(!e3->FirstChild())
								e2->RemoveChild(e3);
						}
					}
				}

				texmap = findTexmap(mtl, ID_BU);
				if(texmap)
				{
					wcs filepath;
					if(exportTexture(texmap, filepath))
					{
						zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("normal_map"));

						e2->SetAttribute("src", filepath);

						writeTextureInfo(e2, texmap);

						zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
						writeUVGenAnimData(e3, texmap, kfwriter, animdata_misc_path);
						if(!e3->FirstChild())
							e2->RemoveChild(e3);
					}
				}
				texmap = findTexmap(mtl, ID_SI);
				if(texmap)
				{
					wcs filepath;
					if(exportTexture(texmap, filepath))
					{
						zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("selfill_map"));

						e2->SetAttribute("src", filepath);

						writeTextureInfo(e2, texmap);
						if(ExportOptions::force_lightmap_filter_linear)
						{
							e2->SetAttribute("min_filter", "LINEAR");
							e2->SetAttribute("mag_filter", "LINEAR");
							e2->SetAttribute("mip_filter", "NONE");
						}

						zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
						writeUVGenAnimData(e3, texmap, kfwriter, animdata_misc_path);
						if(!e3->FirstChild())
							e2->RemoveChild(e3);
					}
				}
				texmap = findTexmap(mtl, ID_SP);
				if(texmap)
				{
					wcs filepath;
					if(exportTexture(texmap, filepath))
					{
						zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("specular_map"));

						e2->SetAttribute("src", filepath);

						writeTextureInfo(e2, texmap);

						zTiXmlElement* e3 = (zTiXmlElement*)e2->InsertEndChild(TiXmlElement("animdata"));
						writeUVGenAnimData(e3, texmap, kfwriter, animdata_misc_path);
						if(!e3->FirstChild())
							e2->RemoveChild(e3);
					}
				}
				texmap = findTexmap(mtl, ID_RL);
				if(texmap)
				{
					wcs filepath;
					if(exportTexture(texmap, filepath))
					{
						zTiXmlElement* e2 = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("env_map"));

						e2->SetAttribute("src", filepath);

						writeTextureInfo(e2, texmap);
					}
				}
			}
		}
		else
		{
			// export defaults

			ent_mtl.SetAttribute("type", "solid");
			ent_mtl.SetAttribute("method", "standard");

			COLORREF c = gamenode->GetMaxNode()->GetWireColor();

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("ambient"));
			e->SetAttribute("value", Vec3(((c >> 0) & 0xff) / 255.0f, ((c >> 8) & 0xff) / 255.0f, ((c >> 16) & 0xff) / 255.0f));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("diffuse"));
			e->SetAttribute("value", Vec3(((c >> 0) & 0xff) / 255.0f, ((c >> 8) & 0xff) / 255.0f, ((c >> 16) & 0xff) / 255.0f));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("specular"));
			e->SetAttribute("value", Vec3(0.9f, 0.9f, 0.9f));

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("specular_level"));
			e->SetAttribute("value", 0.3f);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("glossiness"));
			e->SetAttribute("value", 0.1f);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("opacity"));
			e->SetAttribute("value", 1.0f);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("lighting"));
			e->SetAttribute("value", true);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("two_sided"));
			e->SetAttribute("value", false);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("alpha_test"));
			e->SetAttribute("func", "GEQUAL");
			e->SetAttribute("ref", 0.0f);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("blend"));
			e->SetAttribute("enabled", false);
			e->SetAttribute("op", "ADD");
			e->SetAttribute("sbf", "ONE");
			e->SetAttribute("dbf", "ZERO");

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("reflect"));
			e->SetAttribute("value", false);

			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("layer_index"));
			e->SetAttribute("value", (int32_t)0);
		}
		e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("toon_shade"));
		e->SetAttribute("map_src", ExportOptions::default_toon_map);
		e->SetAttribute("outline_width", ExportOptions::default_toon_outline_width);
		e->SetAttribute("outline_color", Vec3(ExportOptions::default_toon_outline_color_r, ExportOptions::default_toon_outline_color_g, ExportOptions::default_toon_outline_color_b));
		if(m && m->ClassID() == Class_ID(DMTL_CLASS_ID,0))
		{
			e = (zTiXmlElement*)ent_mtl.InsertEndChild(TiXmlElement("animdata"));

			StdMat* mtl = (StdMat*)m;

			// animation data
			Animatable* animatable;
			size_t chunk_idx;

			//animatable = getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Ambient Color");
			animatable = getMtlAmbientColorAnimatable(mtl);
			if(animatable)
			{
				class exp: public KeyGetter<Vec3>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					//Vec3					get(TimeValue t) {return toVec3(mtl->GetAmbient(t));}
					Vec3					get(TimeValue t) {return toVec3(getMtlAmbientColor(mtl, t));}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("ambient"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}
			//animatable = getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Diffuse Color");
			animatable = getMtlDiffuseColorAnimatable(mtl);
			if(animatable)
			{
				class exp: public KeyGetter<Vec3>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					//Vec3					get(TimeValue t) {return toVec3(mtl->GetDiffuse(t));}
					Vec3					get(TimeValue t) {return toVec3(getMtlDiffuseColor(mtl, t));}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("diffuse"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}
			//animatable = getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Specular Color");
			animatable = getMtlSpecularColorAnimatable(mtl);
			if(animatable)
			{
				class exp: public KeyGetter<Vec3>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					//Vec3					get(TimeValue t) {return toVec3(mtl->GetSpecular(t));}
					Vec3					get(TimeValue t) {return toVec3(getMtlSpecularColor(mtl, t));}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("specular"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}
			animatable = getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Specular Level");
			if(animatable)
			{
				class exp: public KeyGetter<REAL>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					REAL					get(TimeValue t) {return mtl->GetShinStr(t);}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("specular_level"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}
			animatable = getSubAnim(getSubAnim(getSubAnim(mtl, L"Shader Basic Parameters"), L"Parameters"), L"Glossiness");
			if(animatable)
			{
				class exp: public KeyGetter<REAL>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					REAL					get(TimeValue t) {return mtl->GetShininess(t);}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("glossiness"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}
			if(true)
			{
				Texmap* texmap = findTexmap(mtl, ID_OP);
				if(texmap && texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
				{
					//listAnimatables(getSubAnim(getSubAnim(texmap, L"Output"), L"Parameters"));
				}
			}
			//animatable = getSubAnim(getSubAnim(mtl, L"Extended Parameters"), L"Opacity");
			animatable = getMtlOpacityAnimatable(mtl);
			if(animatable)
			{
				class exp: public KeyGetter<REAL>
				{
				public:
					StdMat*					mtl;
					exp(StdMat* mtl_): mtl(mtl_) {}
					//REAL					get(TimeValue t) {return mtl->GetOpacity(t);}
					REAL					get(TimeValue t) {return getMtlOpacity(mtl, t);}
				};
				if(kfwriter.exportChunk(animatable, exp(mtl), chunk_idx))
				{
					zTiXmlElement& e2 = *(zTiXmlElement*)e->InsertEndChild(TiXmlElement("opacity"));
					e2.SetAttribute("src", animdata_misc_path);
					e2.SetAttribute("chunk", (int32_t)chunk_idx);
				}
			}

			if(!e->FirstChildElement())
				ent_mtl.RemoveChild(e);
		}
	}

	wcs filepath;
	wcs filename;
	if(main_mtl)
		filename = L"mtl_" + MBSTOWCS(main_mtl->GetName().data()) + L".xml";
	else
		filename = L"mtl_" + MBSTOWCS(gamenode->GetMaxNode()->GetName()) + L".xml";

	filepath = getMeshDirABS() + L"/" + filename;

	if(true)
	{
		FILE* fp = NULL;
		_wfopen_s(&fp, filepath.c_str(), L"w");
		if(fp)
		{
			doc.SaveFile(fp);
			fclose(fp);

			exported_mtl = getMeshDirREL() + L"/" + filename;

			if(main_mtl)
				map_mtl_filepath[main_mtl] = exported_mtl;

			return true;
		}
	}

	return false;
}

template<class T>
T&			advance_iter(T& iter, size_t off)
{
	std::advance(iter, off);
	return iter;
}

bool exportMeshNode(IGameNode* gamenode, wcs& exported_mesh, wcs& exported_mtl, EXPORTED_MORPHER& exported_morpher, wcs& exported_skin, KFWriter& kfwriter, const wcs& animdata_misc_path)
{
#pragma message("\t")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("!!!!!!!!!!!!!!!                                                       WARNING!!!")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("!! ")
#pragma message("!! well, just don't know where to put this")
#pragma message("!! we still miss the check if the vertex counts of those skin and morpher are still the same as the mesh's we exporting")
#pragma message("!! because there may be cases you skin then you apply a turbosmooth. but normally this shouldn't as artists should aware of this or we will include it in doc somehow")
#pragma message("!! ")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("\t")

	TimeValue cur_time = GetCOREInterface()->GetTime();

	INode* node = gamenode->GetMaxNode();

	Object* obj = node->GetObjOrWSMRef();

	if(false)
	{
		syslog << FORMATA("\r\n\r\nName: %s\r\nINode: 0x%08x\r\nGetObjectRef(): 0x%08x\r\nGetObjOrWSMRef(): 0x%08x\r\nskin modifier: 0x%08x\r\n",
			node->GetName(),
			node,
			node->GetObjectRef(),
			node->GetObjOrWSMRef(),
			(gamenode->GetIGameObject() && gamenode->GetIGameObject()->GetIGameSkin()) ? gamenode->GetIGameObject()->GetIGameSkin()->GetMaxModifier() : NULL
			);
		if((gamenode->GetIGameObject() && gamenode->GetIGameObject()->GetIGameSkin()))
		{
			mbs s = "\r\n\r\n";
			Tab<INode*> l;
			gamenode->GetIGameObject()->GetIGameSkin()->EffectedNodes(l);
			for(size_t i = 0; i < (size_t)l.Count(); ++i)
				s += FORMATA("%d 0x%08x\r\n", i, l[i]);
			syslog <<  s;
		}
	}

	if(!obj)
		return false;

	if(map_mesh_filepath.count(obj))
	{
		exported_mesh = map_mesh_filepath[obj];
		if(!exportMaterial(gamenode, exported_mtl, kfwriter, animdata_misc_path))
			return false;
		if(!gamenode->GetIGameObject() || !gamenode->GetIGameObject()->GetIGameSkin())
			return true;

		EXPORTED_SKIN s = {obj, gamenode->GetIGameObject()->GetIGameSkin()->GetMaxModifier()};
		if(map_skin_filepath.count(s))
			exported_skin = map_skin_filepath[s];

		return true;
	}

	Mat4 objstate_tm;
	int deleteIt;
	TriObject* tri = GetTriObjectFromNode(node, deleteIt, objstate_tm);
	if(!tri)
		return false;
	if(!tri->mesh.getNumFaces())
		return false;

	TriObjectDeleter trideleter(tri, deleteIt);

	Mesh mesh = tri->mesh;

	Mat4 extra_offset = Mat4::identity;
	if(true)
	{
		Mat4 tm = getWorldTM(node, cur_time);
		Vec3 pos, scale; z3D::Core::Quat rot;
		tm.factorSRT(scale, rot, pos);

		// no use, just sit here to get noticed
		Mat4 shear = Mat4::scaling(scale).inverse() * tm * (rot.asMat4() * Mat4::translation(pos)).inverse();

		Mat4 tm2 = Mat4::scaling(scale) * rot.asMat4() * Mat4::translation(pos);
		extra_offset = tm * tm2.inverse();

		// *** DETECT SUCH EXTRA OFFSET AND REPORT ***
	}

	Mat4 vert_trans = toMat4(node->GetObjTMAfterWSM(cur_time)) * ROTXNEG90_4 * getWorldTM(node, cur_time).inverse();
	Mat3 norm_trans = ((Mat3)vert_trans).inverse().transpose();

	vert_trans *= extra_offset;

	bool obj_space_flipped = false;

	if(true)
	{
		Mat4 m = vert_trans.transpose();
		if((Vec3(m[0]) ^ Vec3(m[1])) * Vec3(m[2]) < 0)
			obj_space_flipped = true;
	}

	mesh.buildRenderNormals();

	if(obj_space_flipped)
	{
		for(int i = 0; i < mesh.getNumFaces(); ++i)
		{
			Point3 n;
			n = mesh.getFaceNormal(i);
			mesh.FlipNormal(i);
			mesh.setFaceNormal(i, n);
		}
	}

	mesh.buildRenderNormals();

	Mtl* mtl = getNodeExportMaterial(gamenode);
	vector< vector<size_t> > submesh_face;
	vector<size_t> submesh_submtl;
	if(true)
	{
		// most of the bulk work is to ensure that every time we open the file then export the result is the same, because memory addresses are different each time thus possibly different ordering in map<> structures
		vector<size_t> face_submtl;
		map<size_t, size_t> map_submtl_submesh;
		for(size_t i = 0; i < (size_t)mesh.getNumFaces(); ++i)
		{
			size_t idx = (size_t)getSubMaterialIndex(mtl, getSubMaterial(mtl, mesh.getFaceMtlIndex((int)i)));
			face_submtl.push_back(idx);
			map_submtl_submesh[idx] = 0;
		}
		size_t c = 0;
		for(map<size_t, size_t>::iterator iter = map_submtl_submesh.begin(); iter != map_submtl_submesh.end(); ++iter, ++c)
			iter->second = c;
		submesh_face.resize(c);
		submesh_submtl.resize(c);
		for(map<size_t, size_t>::iterator iter = map_submtl_submesh.begin(); iter != map_submtl_submesh.end(); ++iter)
			submesh_submtl[iter->second] = iter->first;
		for(size_t i = 0; i < (size_t)mesh.getNumFaces(); ++i)
		{
			size_t idx = (size_t)getSubMaterialIndex(mtl, getSubMaterial(mtl, mesh.getFaceMtlIndex((int)i)));
			submesh_face[map_submtl_submesh[idx]].push_back(i);
		}
	}

#pragma pack(push)
#pragma pack(1)
	class vnorm
	{
	public:
		uint32_t		vert;
		uint32_t		sm;
		Vec3			norm;
	public:
		vnorm()			{memset(this, 0, sizeof(*this));}
	public:
		bool			operator < (const vnorm& right) const {if(vert < right.vert) return true; if(vert > right.vert) return false; if(sm < right.sm) return true; if(sm > right.sm) return false; return memcmp(this, &right, sizeof(*this)) < 0;}
		bool			operator == (const vnorm& right) const {return memcmp(this, &right, sizeof(*this)) == 0;}
	};
#pragma pack(pop)

	// onto normals

	Z_STATIC_ASSERT( sizeof(vnorm().sm) == sizeof(mesh.faces[0].smGroup), WRONG_ASSUMPTION );

	map<vnorm, size_t> map_normal_idx;
	vector<vnorm> face_3normals;
	for(size_t i = 0; i < (size_t)mesh.getNumFaces(); ++i)
	{
		RNormal* rnor;
		for(size_t j = 0; j < 3; ++j)
		{
			Point3 p;

			if(!(rnor = getFaceVertexNormal(mesh, (int)i, (int)j)))
				p = mesh.getFaceNormal((int)i);
			else
				p = rnor->getNormal();

			Vec3 v(p.x, p.y, p.z);
			v *= norm_trans;
			v.normalize_safe();

			vnorm n;
			n.vert = mesh.faces[i].v[j];
			n.sm = (uint32_t)mesh.faces[i].smGroup;
			n.norm = v;

			map_normal_idx[n] = 0;

			face_3normals.push_back(n);
		}
	}
	if(true)
	{
		size_t c = 0;
		for(map<vnorm, size_t>::iterator iter = map_normal_idx.begin(); iter != map_normal_idx.end(); ++iter, ++c)
			iter->second = c;
	}

#pragma pack(push)
#pragma pack(1)
	class vcolor
	{
	public:
		uint32_t		vert;
		Vec4			color;
	public:
		vcolor()		{memset(this, 0, sizeof(*this));}
	public:
		bool			operator < (const vcolor& right) const {if(vert < right.vert) return true; if(vert > right.vert) return false; return memcmp(&color, &right.color, sizeof(color)) < 0;}
	};
#pragma pack(pop)

	// onto vertex colors
	map<vcolor, size_t> map_color_idx;
	vector<vcolor> face_3colors;
	if(mesh.mapSupport(0))
	{
		for(size_t i = 0; i < (size_t)mesh.getNumFaces(); ++i)
		{
			const Face& f = mesh.faces[i];
			const TVFace& f2 = mesh.mapFaces(0)[i];
			for(size_t j = 0; j < 3; ++j)
			{
				const Point3& p = mesh.mapVerts(0)[f2.t[j]];
				Vec4 c(p.x, p.y, p.z, 1.0f);
				if(mesh.mapSupport(MAP_ALPHA))
					c.w = mesh.mapVerts(MAP_ALPHA)[mesh.mapFaces(MAP_ALPHA)[i].t[j]].x;

				vcolor vc;
				vc.vert = f.v[j];
				vc.color = c;

				map_color_idx[vc] = 0;
				face_3colors.push_back(vc);
			}
		}
		if(true)
		{
			size_t c = 0;
			for(map<vcolor, size_t>::iterator iter = map_color_idx.begin(); iter != map_color_idx.end(); ++iter, ++c)
				iter->second = c;
		}
	}

	size_t mesh_vertex_count = (size_t)mesh.getNumVerts();

	// actual export begins

	MemWriter writer;

	if(true)
	{
		writer.write("ZMSH", 4);
		writer.write((uint32_t)1);

		const size_t MAX_MCHANNELS = 8;									// this must be synced with LoadedMesh::MAX_MCHANNELS

		writer.write((uint32_t)mesh.getNumFaces());						// no. of faces

		writer.write((uint32_t)mesh.getNumVerts());						// no. of verts
		writer.write((uint32_t)map_normal_idx.size());					// no. of norms
		writer.write((uint32_t)map_color_idx.size());					// no. of colors
		size_t mvert_count = 0;
		size_t mface_count = 0;
		if(true)
		{
			for(int i = 1; i < mesh.getNumMaps(); ++i)
			{
				if(i - 1 > 8)
					continue;
				if(!haveMapChannel(mesh, i))
					continue;
				mvert_count += (size_t)mesh.Map(i).getNumVerts();
				mface_count += (size_t)mesh.getNumFaces();
			}
		}
		writer.write((uint32_t)mvert_count);									// no. of map verts
		writer.write((uint32_t)mface_count);									// no. of map faces

		writer.write((uint32_t)submesh_submtl.size());							// no. of submeshes

		// vertices
		for(size_t i = 0; i < (size_t)mesh.getNumVerts(); ++i)
		{
			const Point3& p = mesh.getVert((int)i);
			Vec3 v(p.x, p.y, p.z);
			v *= vert_trans;
			writer.write(&v, sizeof(v));
		}

		// normals
		for(map<vnorm, size_t>::iterator iter = map_normal_idx.begin(); iter != map_normal_idx.end(); ++iter)
			writer.write(&iter->first.norm, sizeof(Vec3));

		// vertex colors
		for(map<vcolor, size_t>::iterator iter = map_color_idx.begin(); iter != map_color_idx.end(); ++iter)
			writer.write(&iter->first.color, sizeof(Vec4));

		// map verts
		if(true)
		{
			for(int i = 1; i < mesh.getNumMaps(); ++i)
			{
				if(i - 1 > MAX_MCHANNELS)
					continue;
				if(!haveMapChannel(mesh, i))
					continue;
				MeshMap& mm = mesh.Map(i);
				for(int j = 0; j < mm.getNumVerts(); ++j)
				{
					const UVVert& uv = mm.tv[j];
					writer.write((float)uv.x);
					writer.write((float)uv.y);
				}
			}
		}

		// faces
		for(size_t i = 0; i < submesh_face.size(); ++i)
		{
			const vector<size_t>& a = submesh_face[i];
			for(size_t j = 0; j < a.size(); ++j)
			{
				size_t fi = a[j];
				const Face& f = mesh.faces[fi];
				for(size_t k = 0; k < 3; ++k)
					writer.write((uint32_t)f.v[k]);
				for(size_t k = 0; k < 3; ++k)
				{
					const vnorm& n = face_3normals[fi * 3 + k];
					Z_VERIFY( map_normal_idx.count(n) );
					size_t idx = map_normal_idx[n];
					writer.write((uint32_t)idx);
				}
			}
		}

		// vertex colored faces
		if(map_color_idx.size())
		{
			for(size_t i = 0; i < submesh_face.size(); ++i)
			{
				const vector<size_t>& a = submesh_face[i];
				for(size_t j = 0; j < a.size(); ++j)
				{
					size_t fi = a[j];
					for(size_t k = 0; k < 3; ++k)
					{
						const vcolor& c = face_3colors[fi * 3 + k];
						Z_VERIFY( map_color_idx.count(c) );
						size_t idx = map_color_idx[c];
						writer.write((uint32_t)idx);
					}
				}
			}
		}

		// map faces
		if(true)
		{
			for(int i = 1; i < mesh.getNumMaps(); ++i)
			{
				if(i - 1 > MAX_MCHANNELS)
					continue;
				if(!haveMapChannel(mesh, i))
					continue;
				MeshMap& mm = mesh.Map(i);
				for(size_t j = 0; j < submesh_face.size(); ++j)
				{
					const vector<size_t>& faces = submesh_face[j];
					for(size_t k = 0; k < faces.size(); ++k)
					{
						const TVFace& f = mm.tf[faces[k]];
						writer.write((uint32_t)f.t[0]);
						writer.write((uint32_t)f.t[1]);
						writer.write((uint32_t)f.t[2]);
					}
				}
			}
		}

		// map channels
		if(true)
		{
			size_t c = 0;
			size_t chc = 0;
			for(size_t i = 0; i < MAX_MCHANNELS; ++i)
			{
				if(!haveMapChannel(mesh, (int)(i + 1)))
					continue;
				++chc;
			}
			writer.write((uint32_t)chc);									// write no. of channels
			chc = 0;
			for(size_t i = 0; i < MAX_MCHANNELS; ++i)
			{
				if(!haveMapChannel(mesh, (int)(i + 1)))
					continue;
				MeshMap& mm = mesh.Map((int)(i + 1));
				writer.write((int32_t)i);
				writer.write((int32_t)c);									// vertex offset
				writer.write((int32_t)mm.getNumVerts());					// vertex count
				writer.write((int32_t)(chc * mesh.getNumFaces()));			// face offset
				if(true)
				{
					bool need_export_tangent = false;
					for(size_t j = 0; j < submesh_submtl.size(); ++j)
					{
						Mtl* submtl = getSubMaterial(mtl, (MtlID)submesh_submtl[j]);
						if(!submtl)
							continue;
						Texmap* texmap = findTexmap(submtl, ID_BU);
						if(!texmap || texmap->GetUVWSource() != UVWSRC_EXPLICIT || texmap->GetMapChannel() != (int)(i + 1))
							continue;
						need_export_tangent = true;
						break;
					}
					writer.write((int32_t)need_export_tangent);				// embedded tangent information
				}

				c += (size_t)mm.getNumVerts();
				++chc;
			}
		}

		// embedded tangents
		if(true)
		{
			vector<vector<int>> vflookup(mesh.getNumVerts());
			for(int i = 0; i < mesh.getNumFaces(); ++i)
			{
				vflookup[mesh.faces[i].v[0]].push_back(i * 3 + 0);
				vflookup[mesh.faces[i].v[1]].push_back(i * 3 + 1);
				vflookup[mesh.faces[i].v[2]].push_back(i * 3 + 2);
			}

			for(size_t i = 0; i < MAX_MCHANNELS; ++i)
			{
				if(!haveMapChannel(mesh, (int)(i + 1)))
					continue;
				MeshMap& mm = mesh.Map((int)(i + 1));
				if(true)
				{
					bool need_export_tangent = false;
					for(size_t j = 0; j < submesh_submtl.size(); ++j)
					{
						Mtl* submtl = getSubMaterial(mtl, (MtlID)submesh_submtl[j]);
						if(!submtl)
							continue;
						Texmap* texmap = findTexmap(submtl, ID_BU);
						if(!texmap || texmap->GetUVWSource() != UVWSRC_EXPLICIT || texmap->GetMapChannel() != (int)(i + 1))
							continue;
						need_export_tangent = true;
						break;
					}
					if(need_export_tangent)
					{
						vector<Vec4> tangents;
						vector<Vec4> tangents2;
						for(size_t j = 0; j < submesh_face.size(); ++j)
						{
							const vector<size_t>& faces = submesh_face[j];
							for(size_t k = 0; k < faces.size(); ++k)
							{
								size_t f = faces[k];

								Vec3 vv[3] = {
									toVec3(mesh.getVert(mesh.faces[f].v[0])) * vert_trans,
									toVec3(mesh.getVert(mesh.faces[f].v[1])) * vert_trans,
									toVec3(mesh.getVert(mesh.faces[f].v[2])) * vert_trans
								};
								Vec2 uu[3] = {
									Vec2(mm.tv[mm.tf[f].t[0]].x, mm.tv[mm.tf[f].t[0]].y),
									Vec2(mm.tv[mm.tf[f].t[1]].x, mm.tv[mm.tf[f].t[1]].y),
									Vec2(mm.tv[mm.tf[f].t[2]].x, mm.tv[mm.tf[f].t[2]].y)
								};
								if(mm.tf[f].t[0] == mm.tf[f].t[1] || mm.tf[f].t[1] == mm.tf[f].t[2] || mm.tf[f].t[2] == mm.tf[f].t[0])
								{
									syslog << FORMATW(L"normal mapping tangent calculation: degenerate mapping face found in face #%d, object \"%s\", channel #%d, indices of the mapping vertices: %d %d %d.   PLZ FIX IT", f, MBSTOWCS(node->GetName()).c_str(), (size_t)(i + 1), (size_t)mm.tf[f].t[0], (size_t)mm.tf[f].t[1], (size_t)mm.tf[f].t[2]);
								}

								Vec3 e0 = vv[1] - vv[0];
								Vec3 e1 = vv[2] - vv[0];

								Vec3 u0 = uu[1] - uu[0];
								Vec3 u1 = uu[2] - uu[0];

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

								tangents.push_back(Vec4(T, handedness));
								tangents.push_back(Vec4(T, handedness));
								tangents.push_back(Vec4(T, handedness));
							}
						}

						if(true)
						{
							size_t face_start = 0;
							for(size_t i = 0; i < submesh_face.size(); ++i)
							{
								const vector<size_t>& faces = submesh_face[i];
								map<int, int> face_index;								// max mesh face index to this submesh face index
								for(size_t k = 0; k < faces.size(); ++k)
									face_index[(int)faces[k]] = (int)k;

								for(size_t j = 0; j < faces.size(); ++j)
									for(size_t k = 0; k < 3; ++k)
									{
										const Vec4& tangent = tangents[face_start + j * 3 + k];

										Vec3 tangent_sum = *(Vec3*)&tangent;

										int face_idx = (int)faces[j];

										uint32_t corner = (uint32_t)(face_idx * 3 + k);

										const vector<int>& e = vflookup[mesh.faces[face_idx].v[k]];
										for(size_t l = 0; l < e.size(); ++l)
										{
											int cor;
											if((cor = e[l]) == corner)														// check referring the same corner
												continue;

											int face_idx2 = cor / 3;
											if(!face_index.count(face_idx2))												// check same submesh
												continue;

											if(map_normal_idx[face_3normals[face_idx * 3 + k]] != map_normal_idx[face_3normals[cor]])		// check same normal idx (if so, so is the vertex idx)
												continue;

											int face_cor2 = cor % 3;
											if(mm.tf[face_idx].t[k] != mm.tf[face_idx2].t[face_cor2])						//check same normal map uv idx
												continue;

											const Vec4& tangent2 = tangents[face_start + face_index[face_idx2] * 3 + face_cor2];
											if(tangent.w != tangent2.w)														// check same tangent space handedness
												continue;

											if((*(Vec3*)&tangent).angle(*(Vec3*)&tangent2) > 90.0f)							// check if the directions differ too much
												continue;

											tangent_sum += *(Vec3*)&tangent2;
										}

										if(true)
										{
											const Vec3& N = face_3normals[faces[j] * 3 + k].norm.normal_safe();

											tangent_sum = tangent_sum - N * (N * tangent_sum);								// orthogonalize to respect the vertex normal
										}

										tangent_sum.normalize_safe();

										tangents2.push_back(Vec4(tangent_sum.normal_safe(), tangent.w));					// now add that averaged tangent
									}
								face_start += faces.size();
							}
						}
						for(size_t i = 0; i < tangents2.size(); ++i)
							writer.write(&tangents2[i], sizeof(tangents2[i]));
					}
				}
			}
		}

		// submeshes
		if(true)
		{
			size_t c = 0;
			for(size_t i = 0; i < submesh_submtl.size(); ++i)
			{
				writer.write((uint32_t)submesh_face[i].size());						// face_count
				writer.write((uint32_t)c);											// face_start
				writer.write((uint32_t)submesh_submtl[i]);							// mtl index

				c += submesh_face[i].size();
			}
		}
	}

	wcs filename = wcs(L"mesh_") + MBSTOWCS(gamenode->GetName()) + L".bin";
	wcs filepath = getMeshDirABS() + L"/" + filename;

	FileSystem::getFileSystem(L"WIN32")->saveFile(filepath, writer.retrieveAll());

	exported_mesh = getMeshDirREL() + L"/" + filename;

	map_mesh_filepath[obj] = exported_mesh;

	if(!exportMaterial(gamenode, exported_mtl, kfwriter, animdata_misc_path))
		return false;

	if(true)
	{
#pragma message("\t")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("!!!!!!!!!!!!!!!                                                       WARNING!!!")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("!! ")
#pragma message("!! IT IS COMPILED TO SUPPORT MORPHER")
#pragma message("!! MAKE SURE YOU MANUALLY COMPILE THE MORPHER PLUGIN YOURSELF WITH NO MODIFICATION TO ANY STL PREPROCESSOR DEFINITION SUCH AS _SECURE_SCL=0")
#pragma message("!! OTHERWISE, WHENEVER THE EXPORTER ENCOUNTER ANY MORPHER IT WILL CRASH")
#pragma message("!! ")
#pragma message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#pragma message("\t")
		for(ptrdiff_t i = (ptrdiff_t)(gamenode->GetIGameObject()->GetNumModifiers() - 1); i >= 0 ; --i)
		{
			if(!gamenode->GetIGameObject()->GetIGameModifier(i)->GetMaxModifier()->IsEnabled())
				continue;
			if(gamenode->GetIGameObject()->GetIGameModifier(i)->GetMaxModifier()->ClassID() == MR3_CLASS_ID)
			{
				MorphR3* morph = dynamic_cast<MorphR3*>(gamenode->GetIGameObject()->GetIGameModifier(i)->GetMaxModifier());

				// the definitive check to (hopefully) avoid crashing when the morpher is not compiled to be compatible with our exporter plugin
				if(morph->chanBank.size() != 100)
				{
					syslog << "!!!!! MORPHER.DLM IS NOT BINARY COMPATIBLE WITH THIS EXPORTER PLUGIN, skipping the morpher modifier, please consult relevant people for this issue. its dangerous to go on without restarting max";
					continue;
				}

				vector<pair<size_t, float>> orig_chans_weight;

				for(size_t j = 0; j < morph->chanBank.size(); ++j)
					if(morph->chanBank[j].mActive && morph->chanBank[j].mActiveOverride && !morph->chanBank[j].mInvalid)
					{
						morphChannel* ch = &morph->chanBank[j];
						float weight;
						Interval validity;
						if(!ch->cblock->GetValue(0, cur_time, weight, validity))
							continue;
						// backup and reset the original weight to 0.0
						orig_chans_weight.push_back(pair<size_t, float>(j, weight));
						ch->cblock->SetValue(0, cur_time, 0.0f);
					}

				if(orig_chans_weight.size() < 2)
					break;

				vector<Vec3> base_pos;
				vector<Vec3> base_nor;

				list<pair<size_t, vector<Vec3> > > chan_pos;
				list<pair<size_t, vector<Vec3> > > chan_nor;

				// now, get the base vertices
				if(true)
				{
					int deleteIt;
					TriObject* tri = GetTriObjectFromNode(node, deleteIt);
					if(tri && tri->mesh.getNumFaces())
					{
						TriObjectDeleter trideleter(tri, deleteIt);

						Mesh& mesh = tri->mesh;

						if(mesh.getNumVerts() != mesh_vertex_count)
							break;

						mesh.buildRenderNormals();

						if(obj_space_flipped)
						{
							for(int i = 0; i < mesh.getNumFaces(); ++i)
							{
								Point3 n;
								n = mesh.getFaceNormal(i);
								mesh.FlipNormal(i);
								mesh.setFaceNormal(i, n);
							}
						}

						mesh.buildRenderNormals();

						for(size_t i = 0; i < (size_t)mesh.getNumVerts(); ++i)
						{
							const Point3& p = mesh.getVert((int)i);
							Vec3 v(p.x, p.y, p.z);
							v *= vert_trans;

							base_pos.push_back(v);
						}

						for(map<vnorm, size_t>::iterator iter = map_normal_idx.begin(); iter != map_normal_idx.end(); ++iter)
						{
							Point3 p;

							RNormal* rnor;
							if(!(rnor = getVertexRNormal(mesh, iter->first.vert, iter->first.sm)))
							{
								vector<vnorm>::const_iterator iter2 = find(face_3normals.begin(), face_3normals.end(), iter->first);
								Z_VERIFY( iter2 != face_3normals.end() );

								p = mesh.getFaceNormal((iter2 - face_3normals.begin()) / 3);
							}
							else
								p = rnor->getNormal();

							Vec3 v(p.x, p.y, p.z);
							v *= norm_trans;
							v.normalize_safe();

							base_nor.push_back(v);
						}
					}
				}

				// get each channel vertices
				for(size_t j = 0; j < morph->chanBank.size(); ++j)
				{
					if(morph->chanBank[j].mActive && morph->chanBank[j].mActiveOverride && !morph->chanBank[j].mInvalid)
					{
						for(size_t k = 0; k < morph->chanBank.size(); ++k)
							if(morph->chanBank[k].mActive && morph->chanBank[k].mActiveOverride && !morph->chanBank[k].mInvalid)
							{
								if(j == k)
									morph->chanBank[k].cblock->SetValue(0, cur_time, 100.0f);
								else
									morph->chanBank[k].cblock->SetValue(0, cur_time, 0.0f);
							}
						int deleteIt;
						TriObject* tri = GetTriObjectFromNode(node, deleteIt);
						if(tri && tri->mesh.getNumFaces())
						{
							TriObjectDeleter trideleter(tri, deleteIt);

							Mesh& mesh = tri->mesh;

							if(mesh.getNumVerts() != mesh_vertex_count)
								continue;

							mesh.buildRenderNormals();

							if(obj_space_flipped)
							{
								for(int i = 0; i < mesh.getNumFaces(); ++i)
								{
									Point3 n;
									n = mesh.getFaceNormal(i);
									mesh.FlipNormal(i);
									mesh.setFaceNormal(i, n);
								}
							}

							mesh.buildRenderNormals();

							chan_pos.push_back(pair<size_t, vector<Vec3> >(j, vector<Vec3>()));
							chan_nor.push_back(pair<size_t, vector<Vec3> >(j, vector<Vec3>()));

							vector<Vec3>& the_chan_pos = (--chan_pos.end())->second;
							vector<Vec3>& the_chan_nor = (--chan_nor.end())->second;

							for(size_t i = 0; i < (size_t)mesh.getNumVerts(); ++i)
							{
								const Point3& p = mesh.getVert((int)i);
								Vec3 v(p.x, p.y, p.z);
								v *= vert_trans;

								the_chan_pos.push_back(v - base_pos[the_chan_pos.size()]);
							}

							for(map<vnorm, size_t>::iterator iter = map_normal_idx.begin(); iter != map_normal_idx.end(); ++iter)
							{
								Point3 p;

								RNormal* rnor;
								if(!(rnor = getVertexRNormal(mesh, iter->first.vert, iter->first.sm)))
								{
									vector<vnorm>::const_iterator iter2 = find(face_3normals.begin(), face_3normals.end(), iter->first);
									Z_VERIFY( iter2 != face_3normals.end() );

									p = mesh.getFaceNormal((iter2 - face_3normals.begin()) / 3);
								}
								else
									p = rnor->getNormal();

								Vec3 v(p.x, p.y, p.z);
								v *= norm_trans;
								v.normalize_safe();

								the_chan_nor.push_back(v - base_nor[the_chan_nor.size()]);
							}
						}
					}
				}

				if(!chan_pos.size())
					break;

				if(true)
				{
					MemWriter writer;

					writer.write("ZMPH", 4);
					writer.write((uint32_t)1);

					writer.write((uint32_t)base_pos.size());
					writer.write((uint32_t)base_nor.size());

					writer.write((uint32_t)chan_pos.size());

					writer.write(&base_pos[0], sizeof(base_pos[0]) * base_pos.size());
					writer.write(&base_nor[0], sizeof(base_nor[0]) * base_nor.size());

					for(size_t i = 0; i < chan_pos.size(); ++i)
					{
						const vector<Vec3>& pos = advance_iter(chan_pos.begin(), i)->second;
						const vector<Vec3>& nor = advance_iter(chan_nor.begin(), i)->second;

						writer.write(&pos[0], sizeof(pos[0]) * pos.size());
						writer.write(&nor[0], sizeof(nor[0]) * nor.size());
					}

					wcs filename = wcs(L"morph_") + MBSTOWCS(gamenode->GetName()) + L".bin";
					wcs filepath = getMeshDirABS() + L"/" + filename;

					FileSystem::getFileSystem(L"WIN32")->saveFile(filepath, writer.retrieveAll());

					exported_morpher.filepath = getMeshDirREL() + L"/" + filename;
				}

				// restore original settings
				for(size_t j = 0; j < orig_chans_weight.size(); ++j)
					morph->chanBank[orig_chans_weight[j].first].cblock->SetValue(0, cur_time, orig_chans_weight[j].second);

				// get the keys
				for(size_t j = 0; j < chan_pos.size(); ++j)
				{
					size_t chan = advance_iter(chan_pos.begin(), j)->first;
					class exp: public KeyGetter<REAL>
					{
					public:
						IParamBlock*				cblock;
						exp(IParamBlock* cblock_): cblock(cblock_) {}
						REAL					get(TimeValue t)
						{
							float weight;
							Interval validity;
							cblock->GetValue(0, t, weight, validity);

							return weight / 100.0f;
						}
					};
					size_t chunk_idx = -1;
					kfwriter.exportChunk(morph->chanBank[chan].cblock->GetController(0), exp(morph->chanBank[chan].cblock), chunk_idx);

					if(true)
					{
						float weight;
						Interval validity;
						morph->chanBank[chan].cblock->GetValue(0, cur_time, weight, validity);

						exported_morpher.chans.push_back(pair<float, size_t>(weight / 100.0f, chunk_idx));
					}
				}

				exported_morpher.node = gamenode->GetMaxNode();

				break;
			}
		}
	}

	// if no skin, just return
	if(!gamenode->GetIGameObject() || !gamenode->GetIGameObject()->GetIGameSkin() || !gamenode->GetIGameObject()->GetIGameSkin()->GetMaxModifier() || !gamenode->GetIGameObject()->GetIGameSkin()->GetMaxModifier()->IsEnabled())
		return true;

	writer.clear();

	writer.write("ZSKN", 4);
	writer.write((uint32_t)1);

	IGameSkin* gameskin = gamenode->GetIGameObject()->GetIGameSkin();
	Modifier* skin_modifier = gameskin->GetMaxModifier();

	size_t skinned_verts = 0;
	map<INode*, size_t> bone_index;
	for(size_t i = 0; i < (size_t)mesh.getNumVerts(); ++i)
	{
		if(!gameskin->GetNumberOfBones((int)i))
			continue;

		++skinned_verts;

		for(size_t j = 0; j < (size_t)gameskin->GetNumberOfBones((int)i); ++j)
		{
			if(!gameskin->GetBone((int)i, (int)j))						// yes, there is such a case
			{
				--skinned_verts;
				break;
			}
			bone_index[gameskin->GetBone((int)i, (int)j)] = 0;
		}
	}

	if(skinned_verts != (size_t)mesh.getNumVerts())
	{
		// no choice, we have to give up here and display a warning
		syslog << FORMATA("%s: cannot export skin, because not all vertices are skinned", node->GetName());
		return true;
	}

	if(true)
	{
		size_t c = 0;
		for(map<INode*, size_t>::iterator iter = bone_index.begin(); iter != bone_index.end(); ++iter, ++c)
			iter->second = c;
	}

	// start exporting ...

	// write inverse of init tm
	if(true)
	{
		Mat4 inv_init_world_tm = getWorldTM(node, cur_time).inverse();
		writer.write(&inv_init_world_tm, sizeof(inv_init_world_tm));
	}

	// write no. of vertices, normals
	writer.write((uint32_t)mesh.getNumVerts());
	writer.write((uint32_t)map_normal_idx.size());

	if(true)
	{
		typedef multimap<REAL, size_t, greater<REAL>> BONEWEIGHTS;

		class write_bone_weight
		{
		private:
			MemWriter&		_writer;
		public:
			write_bone_weight(MemWriter& writer)
				: _writer(writer)
			{}
		public:
			void			operator() (const pair<REAL, size_t>& p)
			{
				_writer.write(p.second);
				_writer.write(p.first);
			}
		};

		size_t next_offset;

		next_offset = writer.offset();
		writer.write((uint32_t)0);

		if(true)
		{
			size_t offset_subtraction = writer.offset();
			vector<int32_t> offsets;

			for(size_t i = 0; i < (size_t)mesh.getNumVerts(); ++i)
			{
				offsets.push_back((int32_t)(writer.offset() - offset_subtraction));
				writer.write((uint32_t)gameskin->GetNumberOfBones((int)i));

				if(!gameskin->GetNumberOfBones((int)i))
					continue;

				BONEWEIGHTS boneweights;

				for(size_t j = 0; j < (size_t)gameskin->GetNumberOfBones((int)i); ++j)
					boneweights.insert(pair<REAL, size_t>(gameskin->GetWeight((int)i, (int)j), (size_t)bone_index[gameskin->GetBone((int)i, (int)j)]));

				for_each<BONEWEIGHTS::const_iterator, write_bone_weight&>(boneweights.begin(), boneweights.end(), write_bone_weight(writer));
			}

			if(true)
			{
				size_t offset = writer.offset();
				writer.seek(next_offset);
				writer.write((uint32_t)offset);
				writer.seek(offset);
			}

			if(offsets.size())
				writer.write(&*offsets.begin(), sizeof(offsets[0]) * offsets.size());

			for(map<vnorm, size_t>::iterator iter = map_normal_idx.begin(); iter != map_normal_idx.end(); ++iter)
			{
				const vnorm& vn = iter->first;
				writer.write(offsets[vn.vert]);
			}
		}

		// write used bones
		writer.write((uint32_t)bone_index.size());
		for(map<INode*, size_t>::iterator iter = bone_index.begin(); iter != bone_index.end(); ++iter)
		{
			Mat4 m = getWorldTM(node, cur_time) * getWorldTM(iter->first, cur_time).inverse();
			writer.write(&m, sizeof(m));
		}
		for(map<INode*, size_t>::iterator iter = bone_index.begin(); iter != bone_index.end(); ++iter)
			writer.write(MBSTOWCS(iter->first->GetName()));

		wcs filename = wcs(L"skin_") + MBSTOWCS(gamenode->GetName()) + L".bin";
		wcs filepath = getMeshDirABS() + L"/" + filename;

		FileSystem::getFileSystem(L"WIN32")->saveFile(filepath, writer.retrieveAll());

		exported_skin = getMeshDirREL() + L"/" + filename;

		EXPORTED_SKIN s = {obj, skin_modifier};
		map_skin_filepath[s] = exported_skin;
	}

	return true;
}

bool exportSkeleton(IGameNode* root,MemWriter& writer)
{
	writer.clear();

	wcs output;
	Matrix3 trans;
	trans.SetRotateX(-PI*0.5f);

	vector<IGameNode*> array_node;
	enumSkeletonNodes(root,array_node);
	int i,j;
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];

		output+=FORMATW(L"create_bone \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str());
		if(i!=0)
			output+=FORMATW(L"set_bone_parent \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),MBSTOWCS(array_node[i]->GetNodeParent()->GetName()).c_str());
		IGameObject* obj=array_node[i]->GetIGameObject();
		obj->InitializeData();
		if(obj)
		{
			Box3 bb;
			obj->GetMaxObject()->GetDeformBBox(g_interface->GetTime(),bb,NULL);
			Matrix3 objtm=node->GetMaxNode()->GetObjectTM(0);
			Matrix3 nodetm=node->GetMaxNode()->GetNodeTM(0);
			nodetm.Invert();
			objtm*=nodetm;
			bb=bb*objtm;
			bb=bb*trans;
			output+=FORMATW(L"set_bone_local_box \"%s\" %f %f %f %f %f %f\r\n",MBSTOWCS(node->GetName()).c_str(),bb.Min()[0],bb.Min()[1],bb.Min()[2],bb.Max()[0],bb.Max()[1],bb.Max()[2]);
		}
		output+=L"\r\n";
	}
	output+=L"\r\n\r\n";
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];

		output+=FORMATW(L"set_current_bone \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str());
		output+=L"\r\n";
		try
		{
			vector<TimeValue> key_timevalue;

			getNodePositionKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			output+=FORMATW(L"resize_pos_keys %d\r\n",key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);
				output+=FORMATW(L"set_pos_key %d %f %f %f\r\n",key_timevalue[j],pos[0],pos[1],pos[2]);
			}
			output+=L"\r\n";

			getNodeRotationKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			output+=FORMATW(L"resize_rot_keys %d\r\n",key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);
				output+=FORMATW(L"set_rot_key %d %f %f %f %f\r\n",key_timevalue[j],rot[0],rot[1],rot[2],rot[3]);
			}
			output+=L"\r\n";

			getNodeScaleKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			output+=FORMATW(L"resize_scale_keys %d\r\n",key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);
				output+=FORMATW(L"set_scale_key %d %f %f %f\r\n",key_timevalue[j],scale[0],scale[1],scale[2]);
			}
			output+=L"\r\n";

		}
		catch(...){syslog << wcs(L"exception occurs while exporting keyframe data of node ") + MBSTOWCS(node->GetName());}
		output+=L"\r\n";
	}

	if(ExportOptions::text_version)
	{
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),(int)output.length()*2);
	}

	return true;
}

/*
void exportSkeletonNEW(Interface *inter,IGameScene* gs)
{
	wstring dir=chooseFolder(L"choose output folder");
	if(!dir.length())
		return;
	export_base_dir=dir;
	export_mesh_dir=dir+L"/"+ExportOptions::mesh_dir+L"/";
	export_texture_dir=dir+L"/"+ExportOptions::texture_dir+L"/";
	export_skeleton_dir=dir+L"/"+ExportOptions::skeleton_dir+L"/";
	create_directory_hierarchy(export_base_dir);
	create_directory_hierarchy(export_mesh_dir);
	create_directory_hierarchy(export_texture_dir);
	create_directory_hierarchy(export_skeleton_dir);

	if(!chooseSkeletonRoot(inter))
		return;

	IGameNode* root=gs->GetIGameNode(inter->GetSelNode(0));
	MemWriter skl_writer;
	MemWriter mesh_writer,mtl_writer;
	if(!exportSkeleton(root,skl_writer))
		return;
	wstring path=export_skeleton_dir+MBSTOWCS(root->GetName())+L".txt";
	syslog_stl<<path;
	syslog.flush();
	skl_writer.seek(0);
	FileSystem::getFileSystem(L"WIN32").saveFile(path,&skl_writer);

	vector<IGameNode*> skeleton_nodes;
	enumSkeletonNodes(root,skeleton_nodes);
	MemWriter writer;
	int i;
	for(i=0;i<(int)skeleton_nodes.size();++i)
	{
		exportNodeMesh(skeleton_nodes[i],mesh_writer,mtl_writer);
		path=export_mesh_dir+MBSTOWCS(skeleton_nodes[i]->GetName())+L"_mesh.txt";
		wstring mtl_path=export_mesh_dir+MBSTOWCS(skeleton_nodes[i]->GetName())+L"_mtl.txt";

		mesh_writer.seek(0);
		FileSystem::getFileSystem(L"WIN32").saveFile(path,&mesh_writer);

		mtl_writer.seek(0);
		FileSystem::getFileSystem(L"WIN32").saveFile(mtl_path,&mtl_writer);
	}
}
*/
bool exportSkin(IGameNode* node,MemWriter& skin_writer)
{
	skin_writer.clear();

	wcs output;

	if(!node->GetIGameObject())
		return false;
	IGameMesh* mesh=(IGameMesh*)node->GetIGameObject();
	if(!mesh)
		return false;
	mesh->InitializeData();
	if(!mesh->IsObjectSkinned())
		return false;
	IGameSkin* skin=mesh->GetIGameSkin();
	UniqueMap<IGameNode*> used_bones;
	int skin_verts=0;
	int i,j;
	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		//int bones = skin->GetNumberOfBones(i);
		int bones = (skin->GetVertexType(i) == IGameSkin::IGAME_RIGID) ? 1: skin->GetNumberOfBones(i);

		skin_verts+=bones;
		if(bones == 0)
		{
			syslog << FORMATW(L"object: \"%s\" contains a non-skinned vertex at %d", MBSTOWCS(node->GetName()).c_str(), i);
		}
		for(j=0;j<bones;++j)
			used_bones[skin->GetIGameBone(i,j)]=true;
	}
	vector<IGameNode*> array_used_bones=used_bones.toVector();
	map<IGameNode*,int> bone_index_map;
	//map<IGameNode*,int> bone_index_map=used_bones.toMapWithIndex();
	for(i=0;i<(int)array_used_bones.size()-1;++i)
		for(j=i;j<(int)array_used_bones.size()-1;++j)
		{
			if(mbs(array_used_bones[j]->GetName()) > mbs(array_used_bones[j+1]->GetName()))
				swap(array_used_bones[j],array_used_bones[j+1]);
		}
	for(i=0;i<(int)array_used_bones.size();++i)
		bone_index_map[array_used_bones[i]]=i;

	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_used_bones %d\r\n\r\n",array_used_bones.size());
	else
		skin_writer.write((int)array_used_bones.size());
	for(i=0;i<(int)array_used_bones.size();++i)
	{
		if(ExportOptions::text_version && false)
			output+=FORMATW(L"set_used_bone %d \"%s\"\r\n",i,MBSTOWCS(array_used_bones[i]->GetName()).c_str());
		else
			skin_writer.write(MBSTOWCS(array_used_bones[i]->GetName()));
	}
	output+=L"\r\n";
	if(ExportOptions::text_version && false)
		output+=FORMATW(L"resize_skin_verts %d\r\n\r\n",skin_verts);
	else
		skin_writer.write((int)skin_verts);
	int verts=0;
	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		float sum=0.0f;

		//int bones = skin->GetNumberOfBones(i);
		int bones = (skin->GetVertexType(i) == IGameSkin::IGAME_RIGID) ? 1: skin->GetNumberOfBones(i);

		for(j=0;j<bones;++j)
			sum+=(float)skin->GetWeight(i,j);
		sum=(sum==0.0f)?1.0f:sum;
		//sum=1.0f;
		for(j=0;j<bones;++j)
		{
			if(ExportOptions::text_version && false)
				output+=FORMATW(L"set_skin_vert %d %d %d %f\r\n",verts,i,bone_index_map[skin->GetIGameBone(i,j)],skin->GetWeight(i,j));
			else
			{
				skin_writer.write((int)i);
				skin_writer.write((int)bone_index_map[skin->GetIGameBone(i,j)]);
				skin_writer.write((float)skin->GetWeight(i,j)/sum);
			}
			++verts;
		}
	}

	if(ExportOptions::text_version && false)
	{
		skin_writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		skin_writer.write((void*)output.c_str(),(int)output.length()*2);
	}

	return true;
}

bool choosePath(Interface* inter)
{
	pathdlgcallback callback;
	if(!inter->DoHitByNameDialog(&callback))
		return false;
	return true;
}


bool exportPath(IGameNode* node,MemWriter& path_writer)
{
	int path_resolution = 200;

	path_writer.clear();

	wcs output;

	if(!node->GetIGameObject())
		return false;

	Mat4 vert_trans=toMat4(node->GetMaxNode()->GetObjTMAfterWSM(0))*ROTXNEG90_4;
	Mat3 tangent_trans=(Mat3)vert_trans;

	ShapeObject* shape=(ShapeObject*)node->GetMaxNode()->EvalWorldState(0).obj;

	output+=FORMATW(L"resize_curves %d\r\n\r\n",shape->NumberOfCurves());

	int i,j;
	for(i=0;i<shape->NumberOfCurves();++i)
	{
		for(j=0;j<path_resolution+1;++j)
		{
			REAL param=((REAL)j)/((REAL)path_resolution);
			param=min(max(0.0f,param),1.0f);
			//Point3 pt=shape->InterpCurve3D(0,i,param);
			Point3 pt=shape->InterpCurve3D(0,i,param,PARAM_NORMALIZED);
			Vec3 p(pt.x,pt.y,pt.z);
			p*=vert_trans;
			//Point3 tangent=shape->TangentCurve3D(0,i,param);
			Point3 tangent=shape->TangentCurve3D(0,i,param,PARAM_NORMALIZED);
			Vec3 t(tangent.x,tangent.y,tangent.z);
			t*=tangent_trans;
			t.normalize();
			output+=FORMATW(L"add_curve_point %d %f %f %f %f %f %f %f\r\n",i,param,p.x,p.y,p.z,t.x,t.y,t.z);
		}
		output+=FORMATW(L"\r\n");
	}

	//if(ExportOptions::text_version)
	{
		path_writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		path_writer.write((void*)output.c_str(),(int)output.length()*2);
	}

	return true;
}

void exportPath()
{
	if(!choosePath(g_interface))
		return;

	wcs dir = CONVERT(chooseFolder(L"choose output folder"));
	if(!dir.length())
		return;
	export_base_dir = dir;
	export_mesh_dir = dir + L"/" + ExportOptions::mesh_dir + L"/";
	export_texture_dir = dir + L"/" + ExportOptions::texture_dir + L"/";
	//export_skeleton_dir=dir+L"/"+ExportOptions::skeleton_dir+L"/";
	create_directory_hierarchy(export_base_dir);
	create_directory_hierarchy(export_mesh_dir);
	create_directory_hierarchy(export_texture_dir);
	//create_directory_hierarchy(export_skeleton_dir);

	IGameNode* node=g_gamescene->GetIGameNode(g_interface->GetSelNode(0));

	MemWriter path_writer;
	if(!exportPath(node,path_writer))
		return;
	FileSystem::getFileSystem(L"WIN32")->saveFile(export_mesh_dir + MBSTOWCS(node->GetName()) + L"_path.txt", path_writer.retrieveAll());
}

Animatable* getSubAnim(Animatable* anim,const wcs& name)
{
	if(!anim)
		return NULL;
	int i;
	for(i=0;i<(int)anim->NumSubs();++i)
		if(anim->SubAnim(i))
			if(MBSTOWCS(anim->SubAnimName(i).data())==name)
				return anim->SubAnim(i);
	return NULL;
}

void listAnimatables(Animatable* anim)
{
	if(!anim)
		return;
	wcs s;
	for(int i = 0; i < (int)anim->NumSubs(); ++i)
		if(anim->SubAnim(i))
			s += wcs(L"|") + MBSTOWCS(anim->SubAnimName(i).data()) + L"|\n";
	MessageBoxW(GetActiveWindow(), s.c_str(), L"listAnimatables", 0);
}

void listParams(IParamBlock2* pb)
{
	if(!pb)
		return;
	if(!pb->GetDesc())
		return;
	wcs s;
	for(size_t i = 0; i < (size_t)pb->GetDesc()->Count(); ++i)
		s += wcs(L"|") + MBSTOWCS(pb->GetDesc()->GetParamDef(pb->GetDesc()->IndextoID((int)i)).int_name) + L"|\n";
	MessageBoxW(GetActiveWindow(), s.c_str(), L"listParams", 0);
}

void listParamBlocks(Animatable* animatable)
{
	if(!animatable)
		return;
	wcs s;
	for(size_t i = 0; i < (size_t)animatable->NumParamBlocks(); ++i)
		if(animatable->GetParamBlock((int)i))
			s += wcs(L"|") + MBSTOWCS(animatable->GetParamBlock((int)i)->GetDesc()->int_name) + L"|\n";
	MessageBoxW(GetActiveWindow(), s.c_str(), L"listParamBlocks", 0);
}

bool exportAnimData(IKeyControl* keyctl, AnimDataExporter& exporter, const wcs& output_filepath)
{
	if(!keyctl || keyctl->GetNumKeys() <= 0)
		return false;

	MemWriter writer;

	REAL ticks_divider = (REAL)GetTicksPerFrame();

	REAL speed = (REAL)GetFrameRate();

	ticks_divider*=speed;
	speed=1.0f;

	map<TimeValue,bool> map_timevalue;
	enumKeyTimeValue(keyctl,map_timevalue);
	vector<TimeValue> array_timevalue;
	convert(map_timevalue,array_timevalue);

	if(array_timevalue.size()>=1 && ExportOptions::sample_every_frame)
		array_timevalue=_array_restricted_timevalue;

	for(int i=0;i<(int)array_timevalue.size();++i)
	{
		writer.write((REAL)(array_timevalue[i]/ticks_divider));
		exporter.write(keyctl,array_timevalue[i],writer);
	}

	FileSystem::getFileSystem(L"WIN32")->saveFile(output_filepath, writer.retrieveAll());

	return true;
}
