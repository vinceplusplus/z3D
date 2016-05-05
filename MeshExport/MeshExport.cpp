/**********************************************************************
 *<
	FILE: MeshExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#define _WIN32_WINNT 0x0501

#include "MeshExport.h"
#include "resource.h"

vector<wcs> exported_geometry;

bool ExportOptions::text_version = true;
bool ExportOptions::export_bone_as_mesh = false;
bool ExportOptions::export_white_ambient_diffuse_color = false;
bool ExportOptions::export_white_specular_color = false;
bool ExportOptions::force_lightmap_filter_linear = false;
bool ExportOptions::sample_every_frame = false;
bool ExportOptions::convert_psd_to_tga = false;
bool ExportOptions::make_tex_size_power_of_2 = false;
bool ExportOptions::ObjectSpace = false;
bool ExportOptions::export_pos_rot = false;
wcs ExportOptions::mesh_dir;
wcs ExportOptions::texture_dir;
wcs ExportOptions::animdata_dir;
bool ExportOptions::default_toon = false;
wcs ExportOptions::default_toon_map;
float ExportOptions::default_toon_outline_width = 3.0f;
float ExportOptions::default_toon_outline_color_r = 0.75f;
float ExportOptions::default_toon_outline_color_g = 0.0f;
float ExportOptions::default_toon_outline_color_b = 0.0f;
wcs ExportOptions::default_export_dir;
uint32_t ExportOptions::generated_tex_size = 256;
uint32_t ExportOptions::max_tex_size = 1024;
bool			ExportOptions::export_current_view = false;
bool			ExportOptions::add_helper_lights = false;
bool			ExportOptions::skip_textures = false;

wcs mesh_save_path;
wcs skeleton_save_path;

int mesh_no;

Config* scene_cfg=NULL;

Interface* g_interface=NULL;
IGameScene* g_gamescene=NULL;

wcs		export_base_dir;
wcs		export_mesh_dir;
wcs		export_texture_dir;
wcs		export_animdata_dir;

vector<TimeValue> _array_restricted_timevalue;

#include "../z3D/Core/Critical/Logger.h"

class LogEnhancer : public ExtLogger
{
public:
	LogEnhancer()
	{
		z3D::Core::syslog_basic.setExtLogger(this);
	}
public:
	~LogEnhancer()
	{
		syslog_basic.setExtLogger(NULL);
	}
public:
	void			addLine(const wchar_t* line)
	{
		addLogToLogWindow(line);
	}
};

static LogEnhancer log_enhancer;

wcs		getMeshDir()
{
	if(ExportOptions::mesh_dir.length())
		return ExportOptions::mesh_dir+L"/";
	else
		return L"";
}

wcs		getTextureDir()
{
	if(ExportOptions::texture_dir.length())
		return ExportOptions::texture_dir+L"/";
	else
		return L"";
}

wcs		getAnimDataDir()
{
	if(ExportOptions::animdata_dir.length())
		return ExportOptions::animdata_dir+L"/";
	else
		return L"";
}

wcs		getMeshDirREL()
{
	return ExportOptions::mesh_dir;
}
wcs		getMeshDirABS()
{
	return getExportBaseDir() + L"/" + getMeshDirREL();
}

wcs		getTextureDirREL()
{
	return ExportOptions::texture_dir;
}
wcs		getTextureDirABS()
{
	return getExportBaseDir() + L"/" + getTextureDirREL();
}

wcs		getAnimDataDirREL()
{
	return ExportOptions::animdata_dir;
}
wcs		getAnimDataDirABS()
{
	return getExportBaseDir() + L"/" + getAnimDataDirREL();
}

wcs		getExportBaseDir()
{
	return export_base_dir;
}

void trace(const char* format,...)
{
	char s[2048];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(s, format, ap);
	va_end(ap);
	MessageBox(NULL,s,"",0);
}


map<mbs,IGameNode*> name_mapped_node;

class MeshExport : public SceneExport {
	public:
		
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		MeshExport();
		~MeshExport();		

};

#include "../z3D/Core/z_off_debug_new.h"

class MeshExportClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MeshExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MeshExport_CLASS_ID; }
	// The Skin modifier checks the category to decide whether the modifier is a Skin Gizmo.  This 
	// must not be changed
	const TCHAR* 	Category() { return GetString(IDS_PW_GIZMOCATEGORY); }

	const TCHAR*	InternalName() { return _T("MeshExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	
};

#include "../z3D/Core/z_on_debug_new.h"


static MeshExportClassDesc MeshExportDesc;
ClassDesc2* GetMeshExportDesc() { return &MeshExportDesc; }



BOOL CALLBACK MeshExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MeshExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (MeshExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}

void saveExportOptions(HWND hWnd)
{
	ExportOptions::ObjectSpace = (IsDlgButtonChecked(hWnd,IDC_CHECK1) == BST_CHECKED);
	ExportOptions::export_pos_rot = (IsDlgButtonChecked(hWnd,IDC_CHECK2) == BST_CHECKED);
	ExportOptions::text_version = (IsDlgButtonChecked(hWnd,IDC_CHECK3) == BST_CHECKED);
	ExportOptions::export_bone_as_mesh = (IsDlgButtonChecked(hWnd,IDC_CHECK5) == BST_CHECKED);
	ExportOptions::export_white_ambient_diffuse_color = (IsDlgButtonChecked(hWnd,IDC_CHECK6) == BST_CHECKED);
	ExportOptions::force_lightmap_filter_linear = (IsDlgButtonChecked(hWnd,IDC_CHECK7) == BST_CHECKED);
	ExportOptions::sample_every_frame = (IsDlgButtonChecked(hWnd,IDC_CHECK8) == BST_CHECKED);
	ExportOptions::export_white_specular_color = (IsDlgButtonChecked(hWnd,IDC_CHECK9) == BST_CHECKED);
	ExportOptions::convert_psd_to_tga = (IsDlgButtonChecked(hWnd,IDC_CHECK10) == BST_CHECKED);
	ExportOptions::make_tex_size_power_of_2 = (IsDlgButtonChecked(hWnd,IDC_CHECK11) == BST_CHECKED);
	ExportOptions::export_current_view = (IsDlgButtonChecked(hWnd,IDC_CHECK12) == BST_CHECKED);
	ExportOptions::add_helper_lights = (IsDlgButtonChecked(hWnd,IDC_CHECK13) == BST_CHECKED);
	ExportOptions::skip_textures = (IsDlgButtonChecked(hWnd,IDC_CHECK14) == BST_CHECKED);
	wchar_t s[1024];
	GetDlgItemTextW(hWnd, IDC_EDIT1, s, 1024);
	ExportOptions::mesh_dir = s;
	GetDlgItemTextW(hWnd, IDC_EDIT2, s, 1024);
	ExportOptions::texture_dir = s;
	GetDlgItemTextW(hWnd, IDC_EDIT3, s, 1024);
	ExportOptions::animdata_dir = s;
	ExportOptions::default_toon = (IsDlgButtonChecked(hWnd, IDC_CHECK4) == BST_CHECKED);
	GetDlgItemTextW(hWnd, IDC_EDIT4, s, 1024);
	ExportOptions::default_toon_map = s;
	ExportOptions::default_toon_outline_width = GetDlgItemFloat(hWnd, IDC_EDIT5);
	ExportOptions::default_toon_outline_color_r = GetDlgItemFloat(hWnd, IDC_EDIT6);
	ExportOptions::default_toon_outline_color_g = GetDlgItemFloat(hWnd, IDC_EDIT7);
	ExportOptions::default_toon_outline_color_b = GetDlgItemFloat(hWnd, IDC_EDIT8);
	GetDlgItemTextW(hWnd, IDC_EDIT9, s, 1024);
	ExportOptions::default_export_dir = s;
	ExportOptions::generated_tex_size = GetDlgItemInt(hWnd, IDC_EDIT10, NULL, FALSE);
	ExportOptions::max_tex_size = max(256, GetDlgItemInt(hWnd, IDC_EDIT11, NULL, FALSE));
}

BOOL CALLBACK exportOptions_dlgproc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

	switch(message) {
		case WM_INITDIALOG:
			CheckDlgButton(hWnd, IDC_RADIO1, BST_CHECKED);
			SetDlgItemTextW(hWnd, IDC_EDIT1, ExportOptions::mesh_dir.c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT2, ExportOptions::texture_dir.c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT3, ExportOptions::animdata_dir.c_str());
			CheckDlgButton(hWnd, IDC_CHECK3, ExportOptions::text_version ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK4, ExportOptions::default_toon ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK5, ExportOptions::export_bone_as_mesh ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK6, ExportOptions::export_white_ambient_diffuse_color ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK7, ExportOptions::force_lightmap_filter_linear ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK8, ExportOptions::sample_every_frame ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK9, ExportOptions::export_white_specular_color ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK10, ExportOptions::convert_psd_to_tga ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK11, ExportOptions::make_tex_size_power_of_2 ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK12, ExportOptions::export_current_view ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK13, ExportOptions::add_helper_lights ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK14, ExportOptions::skip_textures ? BST_CHECKED : BST_UNCHECKED);
			SetDlgItemTextW(hWnd, IDC_EDIT4, ExportOptions::default_toon_map.c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT5, FORMATW(L"%f", ExportOptions::default_toon_outline_width).c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT6, FORMATW(L"%f", ExportOptions::default_toon_outline_color_r).c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT7, FORMATW(L"%f", ExportOptions::default_toon_outline_color_g).c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT8, FORMATW(L"%f", ExportOptions::default_toon_outline_color_b).c_str());
			SetDlgItemTextW(hWnd, IDC_EDIT9, ExportOptions::default_export_dir.c_str());
			SetDlgItemInt(hWnd, IDC_EDIT10, ExportOptions::generated_tex_size, FALSE);
			SetDlgItemInt(hWnd, IDC_EDIT11, max(256, ExportOptions::max_tex_size), FALSE);
			return TRUE;
		case WM_COMMAND:
			if(wParam==IDC_OK)
			{
				saveExportOptions(hWnd);

				if(IsDlgButtonChecked(hWnd,IDC_RADIO1)==BST_CHECKED)
					EndDialog(hWnd, 1);
				if(IsDlgButtonChecked(hWnd,IDC_RADIO2)==BST_CHECKED)
					EndDialog(hWnd, 2);
				if(IsDlgButtonChecked(hWnd,IDC_RADIO3)==BST_CHECKED)
					EndDialog(hWnd, 3);
			}
			else if(wParam==IDC_CANCEL)
				EndDialog(hWnd, 0);
			else if(wParam==IDC_BUTTON1)
			{
				saveExportOptions(hWnd);

				EndDialog(hWnd,4);
			}
			else if(wParam==IDC_BUTTON3)
			{
				saveExportOptions(hWnd);

				EndDialog(hWnd,5);
			}
			else if(wParam==IDC_BUTTON4)
			{
				saveExportOptions(hWnd);

				EndDialog(hWnd,6);
			}
			else if(wParam == IDC_BUTTON5)
			{
				wchar_t buffer[1024] = L"";
				GetDlgItemTextW(hWnd, IDC_EDIT9, buffer, _countof(buffer));
				wcs s = CONVERT(chooseFolder(wstring(L"Choose Default Export Folder"), buffer, L"", (intptr_t)hWnd));
				if(s.length())
					SetDlgItemTextW(hWnd, IDC_EDIT9, s.c_str());
			}
			return TRUE;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK skeletonRoot_dlgproc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	switch(message) {
		case WM_INITDIALOG:
			{
				map<mbs,IGameNode*>::iterator iter;
				for(iter=name_mapped_node.begin();iter!=name_mapped_node.end();++iter)
				{
					SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)iter->first.c_str());
				}
			}
			return TRUE;
		case WM_COMMAND:
			{
				if(HIWORD(wParam)==LBN_DBLCLK||LOWORD(wParam)==IDOK)
				{
					EndDialog(hWnd,SendDlgItemMessage(hWnd,IDC_LIST1,LB_GETCURSEL,0,0));
					return TRUE;
				}
				else if(LOWORD(wParam)==IDCANCEL)
				{
					EndDialog(hWnd, -1);
					return TRUE;
				}
					
			}
			return FALSE;
		case WM_CLOSE:
			EndDialog(hWnd, -1);
			return TRUE;
	}
	return FALSE;
}


//--- MeshExport -------------------------------------------------------
MeshExport::MeshExport()
{

}

MeshExport::~MeshExport() 
{

}

int MeshExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *MeshExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("zm");
}

const TCHAR *MeshExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("zMesh");
}
	
const TCHAR *MeshExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("zMesh");
}

const TCHAR *MeshExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("z3D");
}

const TCHAR *MeshExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *MeshExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *MeshExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int MeshExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MeshExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MeshExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

bool matchMeshMaterial(IGameMesh* mesh,IGameMaterial* mtl,int texmap_index,int& map_channel,IGameTextureMap*& texmap)
{
	if(!mtl)
		return false;
	int i,j;
	for(i=0;i<mtl->GetNumberOfTextureMaps();++i)
	{
		IGameTextureMap* tex=mtl->GetIGameTextureMap(i);
		if(!tex)
		{
			trace("null tex: %s",mtl->GetMaterialName());
			continue;
		}
		if(tex->GetStdMapSlot()!=texmap_index)
			continue;
		if(tex->GetMaxTexmap()->GetUVWSource()!=UVWSRC_EXPLICIT)
		{
			trace("uvwsource: %s",mtl->GetMaterialName());
			continue;
		}
		int texmap_ch=tex->GetMaxTexmap()->GetMapChannel();
		if(texmap_ch<=0)
		{
			trace("channel: %s",mtl->GetMaterialName());
			continue;
		}
//		try
//		{
			if(!tex->GetBitmapFileName())
			{
				syslog<<FORMATA("WARNING: cannot get texture filename (mtl name:%s), probably not standard bitmap texture",mtl->GetMaterialName());
//				trace("null texture filename: %s",mtl->GetMaterialName());
				continue;
			}
//		}catch(...)
//		{
//			trace("bitmapfilename: %s",mtl->GetMaterialName());
//			continue;
//		}
		if(!strrchr(tex->GetBitmapFileName(),'\\'))
		{
			trace("bitmapfilename2: %s",mtl->GetMaterialName());
			continue;
		}
		Tab<int> ch=mesh->GetActiveMapChannelNum();
		for(j=0;j<ch.Count();++j)
			if(ch[j]==texmap_ch)
			{
				map_channel=texmap_ch;
				texmap=tex;
				return true;
			}
	}
	//trace("no mtl: %s",mtl->GetMaterialName());
	return false;
}

const REAL ROTXNEG90_4_SRC[4][4]={{1,0,0,0},{0,0,-1,0},{0,1,0,0},{0,0,0,1}};
const REAL ROTXNEG90_3_SRC[3][3]={{1,0,0},{0,0,-1},{0,1,0}};
const REAL ROTX90_4_SRC[4][4]={{1,0,0,0},{0,0,1,0},{0,-1,0,0},{0,0,0,1}};
const REAL ROTX90_3_SRC[3][3]={{1,0,0},{0,0,1},{0,-1,0}};
Mat4 ROTXNEG90_4(ROTXNEG90_4_SRC);
Mat3 ROTXNEG90_3(ROTXNEG90_3_SRC);
Mat4 ROTX90_4(ROTX90_4_SRC);
Mat3 ROTX90_3(ROTX90_3_SRC);

Vec3 toVec3(Point3 p)
{
	return Vec3(p[0],p[1],p[2]);
}

Mat4 toMat4(GMatrix m)
{
	REAL v[4][4];
	v[0][0]=m.GetRow(0).x;v[0][1]=m.GetRow(0).y;v[0][2]=m.GetRow(0).z;v[0][3]=m.GetRow(0).w;
	v[1][0]=m.GetRow(1).x;v[1][1]=m.GetRow(1).y;v[1][2]=m.GetRow(1).z;v[1][3]=m.GetRow(1).w;
	v[2][0]=m.GetRow(2).x;v[2][1]=m.GetRow(2).y;v[2][2]=m.GetRow(2).z;v[2][3]=m.GetRow(2).w;
	v[3][0]=m.GetRow(3).x;v[3][1]=m.GetRow(3).y;v[3][2]=m.GetRow(3).z;v[3][3]=m.GetRow(3).w;
	return Mat4(v);
}

Mat4 getLocalTM(IGameNode* node, TimeValue t)
{
	Mat4 m = toMat4(node->GetWorldTM(t));
	if(isCameraNode(node) || isLightNode(node))
		m = ROTXNEG90_4 * m;
	if(node->GetNodeParent())
	{
		IGameNode* parent = node->GetNodeParent();
		Mat4 m2 = toMat4(parent->GetWorldTM(t));
		if(isCameraNode(parent) || isLightNode(parent))
			m2 = ROTXNEG90_4 * m2;
		m = m * m2.inverse();
	}
	return m;
}

wcs toWstr(const mbs& s)
{
	wchar_t* buf;
	int size=0;
	size=MultiByteToWideChar(CP_ACP,0,s.c_str(),(int)s.length(),NULL,0)+1;
	buf=new wchar_t[size];
	MultiByteToWideChar(CP_ACP,0,s.c_str(),(int)s.length(),buf,size);
	buf[size-1]=0;
	wcs wstr(buf);
	delete[] buf;
	return wstr;
}

void exportMesh(IGameMesh* mesh,IGameMaterial* mtl,const char* path,IGameNode* igamenode)
{
	// file format
	//
	// 4 bytes identifier - "MSH"
	// number of vertices - int
	// number of normals - int
	// number of map vertices - int
	// number of faces - int
	// number of materials - int
	// array of material -	ambient color(4 floats), diffuse color(4 floats),
	//						specular color(4 floats), shininess(float),
	//						start face index(int), face count(int), diffuse map indicator(int),
	//						diffuse map filename(wstring, depends on diffuse map indicator),
	//						bump map indicator, bump map filename(wstring, depends on bump map indicator)
	// array of vertices - triple float per vertex
	// array of normals - triple float per normal
	// array of map vertices - triple float per map vertex
	// array of face vertex - triple int per face
	// array of face normal - triple int per face
	// array of face map vertex indexes of each material - 3 int per diffuse(depends on diffuse map indicator), 3 int per bump(depends on bump map indicator) for each material

	if(!mtl)
		return;

	Mat4 inv_world=toMat4(igamenode->GetWorldTM()).inverse();
	Mat3 inv_world_rot=inv_world;

	wcs wpath=toWstr(path);
	wcs wdir=wpath;
	if(wcsrchr(wdir.c_str(),'\\'))
	{
		*(wchar_t*)wcsrchr(wdir.c_str(),'\\')=0;
		wcs wdir2(wdir.c_str());
		wdir=wdir2;
	}

	mesh->SetCreateOptimizedNormalList();
	if(!mesh->InitializeData())
		return;
	int i,j;
	MemWriter writer,buffer;
	writer.write("MSH");
	writer.write(mesh->GetNumberOfVerts());
	writer.write(mesh->GetNumberOfNormals());
	syslog<<FORMATW(L"Vertex Count: %d", mesh->GetNumberOfVerts());
	syslog<<FORMATW(L"Normal Count: %d", mesh->GetNumberOfNormals());
	Tab<int> matid=mesh->GetActiveMatIDs();
	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		Point3 v=mesh->GetVertex(i);
		Vec3 p;
		if(ExportOptions::ObjectSpace)
			p=toVec3(v)*inv_world;
		else
			p=toVec3(v);
		buffer.write(p[0]);buffer.write(p[1]);buffer.write(p[2]);
	}
	for(i=0;i<mesh->GetNumberOfNormals();++i)
	{
		Point3 n=mesh->GetNormal(i);
		Vec3 p;
		if(ExportOptions::ObjectSpace)
			p=toVec3(n)*inv_world_rot;
		else
			p=toVec3(n);
		buffer.write(p[0]);buffer.write(p[1]);buffer.write(p[2]);
	}
	int face_count=0;
	vector<IGameMaterial*> mat_array;
	for(j=0;j<matid.Count();++j)
	{
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		face_count+=face_array.Count();
		if(face_array.Count())
			mat_array.push_back(mesh->GetMaterialFromFace(face_array[0]));
	}
	map<int,int> used_ch;
	for(i = 0;i < (int)mat_array.size();++i)
	{
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		try
		{
			diffuse_map=matchMeshMaterial(mesh,mat_array[i],ID_DI,diffuse_ch,diffuse_tex);
		}catch(...){syslog << "Exception matching diffuse texmap";}
		try
		{
			bump_map=matchMeshMaterial(mesh,mat_array[i],ID_BU,bump_ch,bump_tex);
		}catch(...){syslog << "Exception matching bump texmap";}
		if(diffuse_map)
			used_ch[diffuse_ch]=diffuse_ch;
		if(bump_map)
			used_ch[bump_ch]=bump_ch;
	}
	map<int,int>::iterator iter;
	int map_vertices=0;
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		used_ch[iter->first]=map_vertices;
		map_vertices+=mesh->GetNumberOfMapVerts(iter->first);
	}
	writer.write(map_vertices);
	writer.write(face_count);
	syslog << FORMATW(L"Map Vertex Count: %d",map_vertices);
	syslog << FORMATW(L"Face Count: %d",face_count);
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		for(i=0;i<mesh->GetNumberOfMapVerts(iter->first);++i)
		{
			Point3 p=mesh->GetMapVertex(iter->first,i);
			buffer.write(p[0]);buffer.write(p[1]);
		}
	}
	for(j=0;j<matid.Count();++j)
	{
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		for(i=0;i<face_array.Count();++i)
		{
			FaceEx* face=face_array[i];
			buffer.write((uint32_t)face->vert[0]);buffer.write((uint32_t)face->vert[1]);buffer.write((uint32_t)face->vert[2]);
		}
	}
	for(j=0;j<matid.Count();++j)
	{
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		for(i=0;i<face_array.Count();++i)
		{
			FaceEx* face=face_array[i];
			buffer.write((uint32_t)face->norm[0]);buffer.write((uint32_t)face->norm[1]);buffer.write((uint32_t)face->norm[2]);
		}
	}
	writer.write(matid.Count());
	syslog << FORMATW(L"Material Count: %d", matid.Count());
	face_count=0;
	for(j=0;j<matid.Count();++j)
	{
		IGameMaterial* mtl=mat_array[j];
		Color c;
		c=mtl->GetMaxMaterial()->GetAmbient();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		c=mtl->GetMaxMaterial()->GetDiffuse();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		c=mtl->GetMaxMaterial()->GetSpecular();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		writer.write(mtl->GetMaxMaterial()->GetShininess());
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		writer.write(face_count);
		writer.write(face_array.Count());
		face_count+=face_array.Count();
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		diffuse_map=matchMeshMaterial(mesh,mat_array[j],ID_DI,diffuse_ch,diffuse_tex);
		bump_map=matchMeshMaterial(mesh,mat_array[j],ID_BU,bump_ch,bump_tex);
		if(diffuse_map)
		{
			writer.write((int)1);
/*			wstring source_map=toWstr(diffuse_tex->GetBitmapFileName());
			wstring mapname;
			if(strrchr(diffuse_tex->GetBitmapFileName(),'\\'))
				mapname=toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1);
			else
				mapname=source_map;*/
			writer.write(getTextureDir()+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1));
			BOOL ret=CopyFileW(toWstr(diffuse_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+(wcsrchr(toWstr(diffuse_tex->GetBitmapFileName()).c_str(),'\\')+1)).c_str(),FALSE);
			if(!ret)
				syslog << FORMATA("WARNING: cannot copy diffuse map: %s", diffuse_tex->GetBitmapFileName());
//			CopyFileW(toWstr(diffuse_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(diffuse_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[diffuse_ch];
				buffer.write(index[0]+offset);buffer.write(index[1]+offset);buffer.write(index[2]+offset);
			}
		}
		else
			writer.write((int)0);
		if(bump_map)
		{
			writer.write((int)1);
			writer.write(getTextureDir()+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1));
			BOOL ret=CopyFileW(toWstr(bump_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+(wcsrchr(toWstr(bump_tex->GetBitmapFileName()).c_str(),'\\')+1)).c_str(),FALSE);
			if(!ret)
				syslog << FORMATA("WARNING: cannot copy bump map: %s", diffuse_tex->GetBitmapFileName());
//			CopyFileW(toWstr(bump_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(bump_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[bump_ch];
				buffer.write(index[0]+offset);buffer.write(index[1]+offset);buffer.write(index[2]+offset);
			}
		}
		else
			writer.write((int)0);
	}
	writer.write(buffer.retrieveAll());

	FileSystem::getFileSystem(L"WIN32")->saveFile(mesh_save_path, writer.retrieveAll());
//	FileSystem::getFileSystem(L"WIN32")->saveFile(wpath,&writer);
	exported_geometry.push_back(getMeshDir()+(wcsrchr(wpath.c_str(),'\\')+1));
	if(ExportOptions::export_pos_rot)
	{
		scene_cfg->setWString(FORMATW(L"mesh%d_path",mesh_no),(getMeshDir()+(wcsrchr(wpath.c_str(),'\\')+1)).c_str());
		Vec3 pos;
		z3D::Core::Quat rot;
		if(ExportOptions::ObjectSpace)
		{
			Mat4 m=toMat4(igamenode->GetWorldTM());
			pos=Vec3(m[3]);
			rot=z3D::Core::Quat(m);
		}
		scene_cfg->setFloat(FORMATW(L"mesh%d_posx",mesh_no),pos[0]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_posy",mesh_no),pos[1]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_posz",mesh_no),pos[2]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_rotx",mesh_no),rot[0]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_roty",mesh_no),rot[1]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_rotz",mesh_no),rot[2]);
		scene_cfg->setFloat(FORMATW(L"mesh%d_rotw",mesh_no),rot[3]);
		++mesh_no;
	}
}

void to(IGameNode* root,map<IGameNode*,int>& skel_node)
{
	vector<IGameNode*> skel_node2;
	skel_node2.push_back(root);
	skel_node[root]=0;
	int i,j;
	for(i=0;i<(int)skel_node.size();++i)
	{
		IGameNode* node=skel_node2[i];
		for(j=0;j<node->GetChildCount();++j)
		{
			skel_node2.push_back(node->GetNodeChild(j));
			skel_node[node->GetNodeChild(j)]=(int)skel_node.size();
		}
	}
}

void to(Tab<IGameNode*> root,map<IGameNode*,int>& skel_node,vector<IGameNode*>& indexed_skel_node)
{
	int k;
	for(k=0;k<root.Count();++k)
		to(root[k],skel_node);
	indexed_skel_node.resize(skel_node.size());
	map<IGameNode*,int>::iterator iter;
	for(iter=skel_node.begin();iter!=skel_node.end();++iter)
		indexed_skel_node[iter->second]=iter->first;
}

void exportSkeleton(IGameScene* gs,Tab<IGameNode*> root,const char* path)
{
	wcs wpath=toWstr(path);
	wcs wdir=wpath;
	if(wcsrchr(wdir.c_str(),'\\'))
	{
		*(wchar_t*)wcsrchr(wdir.c_str(),'\\')=0;
		wcs wdir2(wdir.c_str());
		wdir=wdir2;
	}

	MemWriter writer;
	writer.write("SKN",4);
	map<IGameNode*,int> skel_node;
	vector<IGameNode*> indexed_skel_node;
	int i,j,k;
//	for(k=0;k<root.Count();++k)
//		to(root[k],skel_node);
	to(root,skel_node,indexed_skel_node);
	writer.write((int)skel_node.size());
	for(k=0;k<(int)indexed_skel_node.size();++k)
	{
		IGameNode* node=indexed_skel_node[k];
		IGameNode* parent=node->GetNodeParent();
		writer.write(toWstr(node->GetName()));
		if(	parent)
			writer.write(skel_node[parent]);
		else
			writer.write((int)-1);
	}
	writer.write(root.Count());
	for(k=0;k<root.Count();++k)
		writer.write(skel_node[root[k]]);
	int start_frame=gs->GetSceneStartTime()/gs->GetSceneTicks();
	int end_frame=gs->GetSceneEndTime()/gs->GetSceneTicks();
	int ticks=gs->GetSceneTicks();
	int num_frames=end_frame-start_frame+1;
	writer.write(start_frame);
	writer.write(num_frames);
//	for(j=0;j<num_frames;++j)
	{
//		gs->SetStaticFrame(start_frame+j);
		for(i=0;i<(int)indexed_skel_node.size();++i)
			for(j=0;j<num_frames;++j)
			{
				IGameNode* node=indexed_skel_node[i];
				Mat4 m=toMat4(node->GetLocalTM((start_frame+j)*ticks));
				Vec3 pos;
				z3D::Core::Quat rot;
				Vec3 scale;
				m.factorSRT(scale,rot,pos);
				writer.write(rot[0]);writer.write(rot[1]);writer.write(rot[2]);writer.write(rot[3]);
				writer.write(pos[0]);writer.write(pos[1]);writer.write(pos[2]);
			}
	}
/*	int max_keys=0;
	int max_listsubctrls=0;
	for(i=0;i<(int)indexed_skel_node.size();++i)
	{
		IGameNode* node=indexed_skel_node[i];
		IGameControl* ctrl=node->GetIGameControl();
		if(ctrl)
		{
//			try
			{
				string s="";

				max_keys=max(max_keys,ctrl->GetIGameKeyCount(IGAME_ROT));
				IGameKeyTab keys;
				Control* c=ctrl->GetMaxControl(IGAME_ROT);
				if(c)
				{
					IKeyControl* key_ctrl=GetKeyControlInterface(c);
					if(key_ctrl)
					{
						char dummy[1024];
						IKey& k=(IKey&)dummy;
						if(key_ctrl->GetNumKeys())
						{
							int i;
							for(i=0;i<key_ctrl->GetNumKeys();++i)
							{
								key_ctrl->GetKey(i,&k);
								s+=FORMATA("%d ",k.time);
							}
							trace(s.c_str());
						}
					}
				}
			}//catch(...){}
		}
	}
	trace("%d %d",max_listsubctrls,max_keys);*/
//	FileSystem::getFileSystem(L"WIN32")->saveFile(wpath,&writer);
	FileSystem::getFileSystem(L"WIN32")->saveFile(skeleton_save_path, writer.retrieveAll());
}

IGameNode* traceRoot(IGameNode* node)
{
	if(node->GetNodeParent())
		return traceRoot(node->GetNodeParent());
	return node;
}

void exportSkinnedMesh(IGameMesh* mesh,IGameMaterial* mtl,Tab<IGameNode*> root,const char* path)
{
	map<IGameNode*,int> skel_node;
	int i,j,k;
	for(k=0;k<root.Count();++k)
		to(root[k],skel_node);

	wcs wpath=toWstr(path);
	wcs wdir=wpath;
	if(wcsrchr(wdir.c_str(),'\\'))
	{
		*(wchar_t*)wcsrchr(wdir.c_str(),'\\')=0;
		wcs wdir2(wdir.c_str());
		wdir=wdir2;
	}
	mesh->SetUseWeightedNormals();
	if(!mesh->InitializeData())
		return;
	if(!mesh->InitializeData())
		return;
	MemWriter writer,buffer,face_buffer;
	writer.write("SMH");
	writer.write(toWstr(root[0]->GetName()));
	writer.write(mesh->GetNumberOfVerts());

	map<int,__int64> vert_face_map;
	for(i=0;i<mesh->GetNumberOfFaces();++i)
	{
		vert_face_map[mesh->GetFace(i)->vert[0]]=(((__int64)i)<<32)|0;
		vert_face_map[mesh->GetFace(i)->vert[1]]=(((__int64)i)<<32)|1;
		vert_face_map[mesh->GetFace(i)->vert[2]]=(((__int64)i)<<32)|2;
	}

	int num_verts=0;
	IGameSkin* skin=mesh->GetIGameSkin();
	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		int bones=skin->GetNumberOfBones(i);
		num_verts+=bones;
		for(j=0;j<bones;++j)
		{
			IGameNode* node=skin->GetIGameBone(i,j);
			REAL weight=skin->GetWeight(i,j);
			Point3 p=mesh->GetVertex(i);
			Vec3 p2(p.x,p.y,p.z);
			p2*=toMat4(node->GetWorldTM()).inverse();
			p=mesh->GetNormal((int)(vert_face_map[i]>>32),(int)vert_face_map[i]);
			Vec3 pos,scale;
			z3D::Core::Quat rot;
			toMat4(node->GetWorldTM()).factorSRT(scale,rot,pos);
			Vec3 n2(p.x,p.y,p.z);
			n2*=rot.asMat3().inverse();
			buffer.write(i);
			buffer.write(p2[0]);buffer.write(p2[1]);buffer.write(p2[2]);
			buffer.write(n2[0]);buffer.write(n2[1]);buffer.write(n2[2]);
			buffer.write(skel_node[node]);
			buffer.write(weight);
		}
	}
	//trace("%d %d",mesh->GetNumberOfVerts(),num_verts);
	writer.write(num_verts);
//	num_verts=0;
/*	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		int bones=skin->GetNumberOfBones(i);
		num_verts+=bones;
		for(j=0;j<bones;++j)
		{
			IGameNode* node=skin->GetIGameBone(i,j);
			REAL weight=skin->GetWeight(i,j);
			Point3 p=mesh->GetVertex(i);
			Vec3 p2(p.x,p.y,p.z);
			p2*=toMat4(node->GetWorldTM()).inverse();
			p=mesh->GetNormal((int)(vert_face_map[i]>>32),(int)vert_face_map[i]);
			Vec3 pos,scale;
			z3D::Core::Quat rot;
			toMat4(node->GetWorldTM()).factorSRT(scale,rot,pos);
			Vec3 n2(p.x,p.y,p.z);
			n2*=rot.asMat3().inverse();
			buffer.write(i);
			buffer.write(p2[0]);buffer.write(p2[1]);buffer.write(p2[2]);
			buffer.write(n2[0]);buffer.write(n2[1]);buffer.write(n2[2]);
			buffer.write(skel_node[node]);
			buffer.write(weight);
		}
	}*/
//	trace("%d",num_verts);
	Tab<int> matid=mesh->GetActiveMatIDs();
	int face_count=0;
	vector<IGameMaterial*> mat_array;
	for(j=0;j<matid.Count();++j)
	{
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		face_count+=face_array.Count();
		if(face_array.Count())
			mat_array.push_back(mesh->GetMaterialFromFace(face_array[0]));
		for(i=0;i<face_array.Count();++i)
		{
			FaceEx* face=face_array[i];
			face_buffer.write((uint32_t)face->vert[0]);face_buffer.write((uint32_t)face->vert[1]);face_buffer.write((uint32_t)face->vert[2]);
		}
	}
	map<int,int> used_ch;
	for(i=0;i<(int)mat_array.size();++i)
	{
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		diffuse_map=matchMeshMaterial(mesh,mat_array[i],ID_DI,diffuse_ch,diffuse_tex);
		bump_map=matchMeshMaterial(mesh,mat_array[i],ID_BU,bump_ch,bump_tex);
		if(diffuse_map)
			used_ch[diffuse_ch]=diffuse_ch;
		if(bump_map)
			used_ch[bump_ch]=bump_ch;
	}
	map<int,int>::iterator iter;
	int map_vertices=0;
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		used_ch[iter->first]=map_vertices;
		map_vertices+=mesh->GetNumberOfMapVerts(iter->first);
	}
	writer.write(map_vertices);
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		for(i=0;i<mesh->GetNumberOfMapVerts(iter->first);++i)
		{
			Point3 p=mesh->GetMapVertex(iter->first,i);
			buffer.write(p[0]);buffer.write(p[1]);
		}
	}
	buffer.write(face_buffer.retrieveAll());
	writer.write(face_count);
	writer.write(matid.Count());
	face_count=0;
	for(j=0;j<matid.Count();++j)
	{
		IGameMaterial* mtl=mat_array[j];
		Color c;
		c=mtl->GetMaxMaterial()->GetAmbient();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		c=mtl->GetMaxMaterial()->GetDiffuse();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		c=mtl->GetMaxMaterial()->GetSpecular();
		writer.write(c.r);writer.write(c.g);writer.write(c.b);writer.write(1.0f);
		writer.write(mtl->GetMaxMaterial()->GetShininess());
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		writer.write(face_count);
		writer.write(face_array.Count());
		face_count+=face_array.Count();
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		diffuse_map=matchMeshMaterial(mesh,mat_array[j],ID_DI,diffuse_ch,diffuse_tex);
		bump_map=matchMeshMaterial(mesh,mat_array[j],ID_BU,bump_ch,bump_tex);
		if(diffuse_map)
		{
			writer.write((int)1);
			writer.write(getTextureDir()+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1));
			CopyFileW(toWstr(diffuse_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(diffuse_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[diffuse_ch];
				buffer.write(index[0]+offset);buffer.write(index[1]+offset);buffer.write(index[2]+offset);
			}
		}
		else
			writer.write((int)0);
		if(bump_map)
		{
			writer.write((int)1);
			writer.write(getTextureDir()+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1));
			CopyFileW(toWstr(bump_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(bump_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[bump_ch];
				buffer.write(index[0]+offset);buffer.write(index[1]+offset);buffer.write(index[2]+offset);
			}
		}
		else
			writer.write((int)0);
	}
	writer.write(buffer.retrieveAll());

	FileSystem::getFileSystem(L"WIN32")->saveFile(mesh_save_path, writer.retrieveAll());
	exported_geometry.push_back(wcsrchr(mesh_save_path.c_str(), '\\') + 1);
//	FileSystem::getFileSystem(L"WIN32")->saveFile(wpath,&writer);
//	exported_geometry.push_back(wcsrchr(wpath.c_str(),'\\')+1);
}

//typedef int INT3[3];
typedef struct
{
	Vec4			ambient;
	Vec4			diffuse;
	Vec4			specular;
	REAL			shininess;
	int				face_start;
	int				face_count;
	int				diffuse_indicator;
	int				bump_indicator;
	wcs				diffuse_map_filename;
	wcs				bump_map_filename;
	vector<INT3>	diffuse_map_index;
	vector<INT3>	bump_map_index;
} MTL;

void export1Mesh(IGameMesh* mesh,vector<Vec3>& vert,vector<Vec3>& norm,vector<Vec2>& map_vert,vector<INT3>& face_vert,vector<INT3>& face_norm,vector<MTL>& mtl,const wcs& wdir)
{
	mesh->SetCreateOptimizedNormalList();
	if(!mesh->InitializeData())
		return;
	int vert_offset=(int)vert.size();
	int norm_offset=(int)norm.size();
	int map_vert_offset=(int)map_vert.size();
	int face_vert_offset=(int)face_vert.size();
	int face_norm_offset=(int)face_norm.size();
	int i,j;
	for(i=0;i<mesh->GetNumberOfVerts();++i)
	{
		Point3 p=mesh->GetVertex(i);
		vert.push_back(Vec3(p.x,p.y,p.z));
	}
	for(i=0;i<mesh->GetNumberOfNormals();++i)
	{
		Point3 p=mesh->GetNormal(i);
		norm.push_back(Vec3(p.x,p.y,p.z));
	}
	Tab<int> matid=mesh->GetActiveMatIDs();
	vector<IGameMaterial*> mat_array;
	for(j=0;j<matid.Count();++j)
	{
		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		if(face_array.Count())
			mat_array.push_back(mesh->GetMaterialFromFace(face_array[0]));
		for(i=0;i<face_array.Count();++i)
		{
			INT3 v;
			v[0]=face_array[i]->vert[0]+vert_offset;
			v[1]=face_array[i]->vert[1]+vert_offset;
			v[2]=face_array[i]->vert[2]+vert_offset;
			face_vert.push_back(v);
			v[0]=face_array[i]->norm[0]+norm_offset;
			v[1]=face_array[i]->norm[1]+norm_offset;
			v[2]=face_array[i]->norm[2]+norm_offset;
			face_norm.push_back(v);
		}
	}
	map<int,int> used_ch;
	for(i=0;i<(int)mat_array.size();++i)
	{
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		diffuse_map=matchMeshMaterial(mesh,mat_array[i],ID_DI,diffuse_ch,diffuse_tex);
		bump_map=matchMeshMaterial(mesh,mat_array[i],ID_BU,bump_ch,bump_tex);
		if(diffuse_map)
			used_ch[diffuse_ch]=diffuse_ch;
		if(bump_map)
			used_ch[bump_ch]=bump_ch;
	}
	map<int,int>::iterator iter;
	int map_vertices=0;
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		used_ch[iter->first]=map_vertices;
		map_vertices+=mesh->GetNumberOfMapVerts(iter->first);
	}
	for(iter=used_ch.begin();iter!=used_ch.end();++iter)
	{
		for(i=0;i<mesh->GetNumberOfMapVerts(iter->first);++i)
		{
			Point3 p=mesh->GetMapVertex(iter->first,i);
			map_vert.push_back(Vec2(p[0],p[1]));
		}
	}
	int face_count=0;
	for(j=0;j<matid.Count();++j)
	{
		IGameMaterial* mat=mat_array[j];
		MTL m;
		Color c;
		c=mat->GetMaxMaterial()->GetAmbient();
		m.ambient=Vec4(c.r,c.g,c.b);
		c=mat->GetMaxMaterial()->GetDiffuse();
		m.diffuse=Vec4(c.r,c.g,c.b);
		c=mat->GetMaxMaterial()->GetSpecular();
		m.specular=Vec4(c.r,c.g,c.b);
		m.shininess=mat->GetMaxMaterial()->GetShininess();

		Tab<FaceEx*> face_array=mesh->GetFacesFromMatID(matid[j]);
		m.face_start=face_vert_offset+face_count;
		m.face_count=face_array.Count();
		face_count+=face_array.Count();
		bool diffuse_map=false;
		bool bump_map=false;
		int diffuse_ch;
		int bump_ch;
		IGameTextureMap* diffuse_tex;
		IGameTextureMap* bump_tex;
		diffuse_map=matchMeshMaterial(mesh,mat_array[j],ID_DI,diffuse_ch,diffuse_tex);
		bump_map=matchMeshMaterial(mesh,mat_array[j],ID_BU,bump_ch,bump_tex);
		if(diffuse_map)
		{
			m.diffuse_indicator=1;
			m.diffuse_map_filename=getTextureDir()+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1);
			CopyFileW(toWstr(diffuse_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+toWstr(strrchr(diffuse_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(diffuse_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[diffuse_ch];
				INT3 v;
				v[0]=index[0]+offset+map_vert_offset;
				v[1]=index[1]+offset+map_vert_offset;
				v[2]=index[2]+offset+map_vert_offset;
				m.diffuse_map_index.push_back(v);
			}
		}
		else
			m.diffuse_indicator=0;
		if(bump_map)
		{
			m.bump_indicator=1;
			m.bump_map_filename=getTextureDir()+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1);
			CopyFileW(toWstr(bump_tex->GetBitmapFileName()).c_str(),(wdir+L"\\"+getTextureDir()+toWstr(strrchr(bump_tex->GetBitmapFileName(),'\\')+1)).c_str(),FALSE);
			for(i=0;i<face_array.Count();++i)
			{
				int index[3];
				mesh->GetMapFaceIndex(bump_ch,face_array[i]->meshFaceIndex,(DWORD*)index);
				int offset=used_ch[bump_ch];
				INT3 v;
				v[0]=index[0]+offset+map_vert_offset;
				v[1]=index[1]+offset+map_vert_offset;
				v[2]=index[2]+offset+map_vert_offset;
				m.bump_map_index.push_back(v);
			}
		}
		else
			m.bump_indicator=0;
		mtl.push_back(m);
	}
}

void exportAs1Mesh(const vector<IGameMesh*>& mesh,const char* path)
{
	wcs wpath=toWstr(path);
	wcs wdir=wpath;
	if(wcsrchr(wdir.c_str(),'\\'))
	{
		*(wchar_t*)wcsrchr(wdir.c_str(),'\\')=0;
		wcs wdir2(wdir.c_str());
		wdir=wdir2;
	}

	vector<Vec3> vert;
	vector<Vec3> norm;
	vector<Vec2> map_vert;
	vector<INT3> face_vert;
	vector<INT3> face_norm;
	vector<MTL> mtl;

	int i,j;
	for(i=0;i<(int)mesh.size();++i)
	{
		export1Mesh(mesh[i],vert,norm,map_vert,face_vert,face_norm,mtl,wdir);
	}
	MemWriter writer;
	writer.write("MSH");
	writer.write((int)vert.size());
	writer.write((int)norm.size());
	writer.write((int)map_vert.size());
	writer.write((int)face_vert.size());
	writer.write((int)mtl.size());
	for(i=0;i<(int)mtl.size();++i)
	{
		writer.write(mtl[i].ambient[0]);writer.write(mtl[i].ambient[1]);writer.write(mtl[i].ambient[2]);writer.write(mtl[i].ambient[3]);
		writer.write(mtl[i].diffuse[0]);writer.write(mtl[i].diffuse[1]);writer.write(mtl[i].diffuse[2]);writer.write(mtl[i].diffuse[3]);
		writer.write(mtl[i].specular[0]);writer.write(mtl[i].specular[1]);writer.write(mtl[i].specular[2]);writer.write(mtl[i].specular[3]);
		writer.write(mtl[i].shininess);
		writer.write(mtl[i].face_start);
		writer.write(mtl[i].face_count);
		writer.write(mtl[i].diffuse_indicator);
		if(mtl[i].diffuse_indicator)
			writer.write(mtl[i].diffuse_map_filename);
		writer.write(mtl[i].bump_indicator);
		if(mtl[i].bump_indicator)
			writer.write(mtl[i].bump_map_filename);
	}
	for(i=0;i<(int)vert.size();++i)
	{
		writer.write(vert[i][0]);writer.write(vert[i][1]);writer.write(vert[i][2]);
	}
	for(i=0;i<(int)norm.size();++i)
	{
		writer.write(norm[i][0]);writer.write(norm[i][1]);writer.write(norm[i][2]);
	}
	for(i=0;i<(int)map_vert.size();++i)
	{
		writer.write(map_vert[i][0]);writer.write(map_vert[i][1]);
	}
	for(i=0;i<(int)face_vert.size();++i)
	{
		writer.write(face_vert[i][0]);writer.write(face_vert[i][1]);writer.write(face_vert[i][2]);
	}
	for(i=0;i<(int)face_norm.size();++i)
	{
		writer.write(face_norm[i][0]);writer.write(face_norm[i][1]);writer.write(face_norm[i][2]);
	}
	for(i=0;i<(int)mtl.size();++i)
	{
		if(mtl[i].diffuse_indicator)
		{
			for(j=0;j<mtl[i].face_count;++j)
			{
				writer.write(mtl[i].diffuse_map_index[j][0]);
				writer.write(mtl[i].diffuse_map_index[j][1]);
				writer.write(mtl[i].diffuse_map_index[j][2]);
			}
		}
		if(mtl[i].bump_indicator)
		{
			for(j=0;j<mtl[i].face_count;++j)
			{
				writer.write(mtl[i].bump_map_index[j][0]);
				writer.write(mtl[i].bump_map_index[j][1]);
				writer.write(mtl[i].bump_map_index[j][2]);
			}
		}
	}
//	FileSystem::getFileSystem(L"WIN32").saveFile(wpath,&writer);
	FileSystem::getFileSystem(L"WIN32")->saveFile(mesh_save_path, writer.retrieveAll());
}

int	MeshExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	//TODO: Implement the actual file Export here and 
	//		return TRUE If the file is exported properly

/*	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				MeshExportOptionsDlgProc, (LPARAM)this);*/

	//DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), GetActiveWindow(), exportOptions_dlgproc);

#ifdef NDEBUG
	#if !_SECURE_SCL
	Z_FATAL_ERROR_IF( false );
	#endif
#endif

	wcs cfg_path = MBSTOWCS(i->GetDir(APP_PLUGCFG_DIR)) + L"\\MeshExport.cfg";
	Config cfg(cfg_path);
	ExportOptions::mesh_dir = cfg.getWString(L"mesh_dir");
	ExportOptions::texture_dir = cfg.getWString(L"texture_dir");
	ExportOptions::animdata_dir = cfg.getWString(L"animdata_dir");
	ExportOptions::text_version = cfg.getInt32(L"text_version") ? true : false;
	ExportOptions::export_bone_as_mesh = cfg.getInt32(L"export_bone_as_mesh") ? true : false;
	ExportOptions::export_white_ambient_diffuse_color = cfg.getInt32(L"export_white_ambient_diffuse_color") ? true : false;
	ExportOptions::export_white_specular_color = cfg.getInt32(L"export_white_specular_color") ? true : false;
	ExportOptions::force_lightmap_filter_linear = cfg.getInt32(L"force_lightmap_filter_linear") ? true : false;
	ExportOptions::sample_every_frame = cfg.getInt32(L"sample_every_frame") ? true : false;
	ExportOptions::convert_psd_to_tga = cfg.getInt32(L"convert_psd_to_tga") ? true : false;
	ExportOptions::make_tex_size_power_of_2 = cfg.getInt32(L"make_tex_size_power_of_2") ? true : false;
	ExportOptions::export_current_view = cfg.getInt32(L"export_current_view") ? true : false;
	ExportOptions::add_helper_lights = cfg.getInt32(L"add_helper_lights") ? true : false;
	ExportOptions::skip_textures = cfg.getInt32(L"skip_textures") ? true : false;
	ExportOptions::default_toon = cfg.getInt32(L"default_toon") ? true : false;
	ExportOptions::default_toon_map = cfg.getWString(L"default_toon_map");
	ExportOptions::default_toon_outline_width = cfg.getFloat(L"default_toon_outline_width");
	ExportOptions::default_toon_outline_color_r = cfg.getFloat(L"default_toon_outline_color_r");
	ExportOptions::default_toon_outline_color_g = cfg.getFloat(L"default_toon_outline_color_g");
	ExportOptions::default_toon_outline_color_b = cfg.getFloat(L"default_toon_outline_color_b");
	ExportOptions::default_export_dir = cfg.getWString(L"default_export_dir");
	if(cfg.exists(L"generated_tex_size"))
		ExportOptions::generated_tex_size = max(1, cfg.getUInt32(L"generated_tex_size"));
	if(cfg.exists(L"max_tex_size"))
		ExportOptions::max_tex_size = max(1, cfg.getUInt32(L"max_tex_size"));

	int sel = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), i->GetMAXHWnd(), exportOptions_dlgproc);

	exported_geometry.clear();

	cfg.setWString(L"mesh_dir", ExportOptions::mesh_dir);
	cfg.setWString(L"texture_dir", ExportOptions::texture_dir);
	cfg.setWString(L"animdata_dir", ExportOptions::animdata_dir);
	cfg.setInt32(L"text_version", (int)ExportOptions::text_version);
	cfg.setInt32(L"export_bone_as_mesh", (int)ExportOptions::export_bone_as_mesh);
	cfg.setInt32(L"export_white_ambient_diffuse_color", (int)ExportOptions::export_white_ambient_diffuse_color);
	cfg.setInt32(L"export_white_specular_color", (int)ExportOptions::export_white_specular_color);
	cfg.setInt32(L"force_lightmap_filter_linear", (int)ExportOptions::force_lightmap_filter_linear);
	cfg.setInt32(L"sample_every_frame", (int)ExportOptions::sample_every_frame);
	cfg.setInt32(L"convert_psd_to_tga", (int)ExportOptions::convert_psd_to_tga);
	cfg.setInt32(L"make_tex_size_power_of_2", (int)ExportOptions::make_tex_size_power_of_2);
	cfg.setInt32(L"export_current_view", (int)ExportOptions::export_current_view);
	cfg.setInt32(L"add_helper_lights", (int)ExportOptions::add_helper_lights);
	cfg.setInt32(L"skip_textures", (int)ExportOptions::skip_textures);
	cfg.setInt32(L"default_toon", (int)ExportOptions::default_toon);
	cfg.setWString(L"default_toon_map", ExportOptions::default_toon_map);
	cfg.setFloat(L"default_toon_outline_width", ExportOptions::default_toon_outline_width);
	cfg.setFloat(L"default_toon_outline_color_r", ExportOptions::default_toon_outline_color_r);
	cfg.setFloat(L"default_toon_outline_color_g", ExportOptions::default_toon_outline_color_g);
	cfg.setFloat(L"default_toon_outline_color_b", ExportOptions::default_toon_outline_color_b);
	cfg.setWString(L"default_export_dir", ExportOptions::default_export_dir);
	cfg.setUInt32(L"generated_tex_size", ExportOptions::generated_tex_size);
	cfg.setUInt32(L"max_tex_size", ExportOptions::max_tex_size);

	cfg.flush();

	if(!sel)
		return FALSE;

#if !defined(_M_X64)
	unsigned int x86_cw = 0;
	unsigned int sse2_cw = 0;

	__control87_2(_PC_64, _MCW_PC, &x86_cw, &sse2_cw);
#endif

	char path[1024];
	strcpy_s(path, name);
	if(strrchr(path,'\\'))
		*(strrchr(path,'\\')+1)=0;

	if(ExportOptions::export_pos_rot)
	{
		if(scene_cfg)
			delete scene_cfg;
		scene_cfg=new Config(MBSTOWCS(path)+getMeshDir()+L"scene.txt");
		scene_cfg->clear();
	}
	IGameScene* gs=GetIGameInterface();

	IGameConversionManager* cm=GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	gs->InitialiseIGame(false);

	if(true)
	{
		Interval interval=i->GetAnimRange();
		_array_restricted_timevalue.clear();

		int tv_incr=GetTicksPerFrame();
		int tv=interval.Start();
		while(tv<=interval.End())
		{
			_array_restricted_timevalue.push_back(tv);
			tv+=tv_incr;
		}
	}

	//::GetCOREInterface()->SetTime(GetCOREInterface()->GetAnimRange().Start());

	BOOL bSucc=TRUE;

	g_interface=i;
	g_gamescene=gs;
	if(sel==4)
	{
		//exportSkeletonNEW(i,gs);
		syslog.flush();
	}
	else if(sel==5)
	{
		exportScene();
	}
	else if(sel==6)
	{
		exportPath();
	}
	else if(sel==1)
	{
		syslog<<L"Export Mode: export multiple meshes";
		syslog.flush();

		Tab<IGameNode*> geom=gs->GetIGameNodeByType(IGameObject::IGAME_MESH);
		create_directory_hierarchy(MBSTOWCS(path)+getMeshDir());
		create_directory_hierarchy(MBSTOWCS(path)+getTextureDir());
		int i;
		mesh_no=0;
		for(i=0;i<geom.Count();++i)
		{
			if(geom[i]->IsNodeHidden())
				continue;
			IGameMesh* mesh=(IGameMesh*)geom[i]->GetIGameObject();
			if(!mesh->InitializeData()||!geom[i]->GetNodeMaterial())
				continue;
			syslog << FORMATW(L"begin exporting mesh: %s", MBSTOWCS(geom[i]->GetName()).c_str());

			mesh_save_path=MBSTOWCS(path)+getMeshDir()+MBSTOWCS(geom[i]->GetName())+L".zm";
			exportMesh(mesh,geom[i]->GetNodeMaterial(),(mbs(path)+geom[i]->GetName()+".zm").c_str(),geom[i]);
			geom[i]->ReleaseIGameObject();

			syslog << FORMATW(L"end exporting mesh: %s", MBSTOWCS(geom[i]->GetName()).c_str());
		}
		if(ExportOptions::export_pos_rot)
		{
			scene_cfg->setInt32(L"meshes",mesh_no);
		}

		MemWriter writer;
		writer.write(mbs("ZMM"));
		writer.write((int)exported_geometry.size());
		for(i=0;i<(int)exported_geometry.size();++i)
			writer.write(exported_geometry[i]);
		FileSystem::getFileSystem(L"WIN32")->saveFile(toWstr(mbs(path))+getMeshDir()+L"meshes.zmm", writer.retrieveAll());
	}
	else if(sel==2)
	{
		map<IGameNode*,IGameNode*> effecting_node;
		map<IGameNode*,IGameNode*> root2;
		Tab<IGameNode*> root;
		Tab<IGameNode*> geom=gs->GetIGameNodeByType(IGameObject::IGAME_MESH);
		int i,j,k;
		for(i=0;i<geom.Count();++i)
		{
			int num_verts_nonskinned=0;
			IGameMesh* mesh=(IGameMesh*)geom[i]->GetIGameObject();
			if(!mesh->InitializeData()||!geom[i]->GetNodeMaterial())
				continue;
			if(!mesh->GetIGameSkin())
				continue;
			IGameSkin* skin=mesh->GetIGameSkin();
			for(j=0;j<mesh->GetNumberOfVerts();++j)
			{
				if(!skin->GetNumberOfBones(j))
					++num_verts_nonskinned;
				for(k=0;k<skin->GetNumberOfBones(j);++k)
					effecting_node[skin->GetIGameBone(j,k)]=NULL;
			}
			if(num_verts_nonskinned)
				trace("mesh %s has %d vertices non-skinned",geom[i]->GetName(),num_verts_nonskinned);
//			exportMesh(mesh,geom[i]->GetNodeMaterial(),(string(path)+geom[i]->GetName()+".zm").c_str());
			geom[i]->ReleaseIGameObject();
		}
		map<IGameNode*,IGameNode*>::iterator iter;
		for(iter=effecting_node.begin();iter!=effecting_node.end();++iter)
			root2[traceRoot(iter->first)]=NULL;
		for(iter=root2.begin();iter!=root2.end();++iter)
			root.Append(1,(IGameNode**)&iter->first);
		if(root.Count())
		{
			create_directory_hierarchy(MBSTOWCS(path)+getMeshDir());
			create_directory_hierarchy(MBSTOWCS(path)+getTextureDir());
			//create_directory_hierarchy(MBSTOWCS(path)+getSkeletonDir());
//			trace("exporting skeleton");
			//skeleton_save_path=MBSTOWCS(path)+getSkeletonDir()+MBSTOWCS(root[0]->GetName())+L".zsk";
			exportSkeleton(gs,root,(mbs(path)+root[0]->GetName()+".zsk").c_str());
//			trace("exported skeleton");
			for(i=0;i<geom.Count();++i)
			{
				IGameMesh* mesh=(IGameMesh*)geom[i]->GetIGameObject();
				if(!mesh->InitializeData()||!geom[i]->GetNodeMaterial())
					continue;
				if(!mesh->GetIGameSkin())
					continue;
//				trace("exporting geometry");
				mesh_save_path=MBSTOWCS(path)+getMeshDir()+MBSTOWCS(geom[i]->GetName())+L".zsm";
				exportSkinnedMesh(mesh,geom[i]->GetNodeMaterial(),root,(mbs(path)+geom[i]->GetName()+".zsm").c_str());
//				trace("exported geometry");
			}
		}
	}
	else if(sel==3)
	{
		Tab<IGameNode*> geom=gs->GetIGameNodeByType(IGameObject::IGAME_MESH);
		int i;
		vector<IGameMesh*> mesh;
		for(i=0;i<geom.Count();++i)
		{
			if(!geom[i]->GetNodeMaterial())
				continue;
			mesh.push_back((IGameMesh*)geom[i]->GetIGameObject());
		}
		create_directory_hierarchy(MBSTOWCS(path)+getMeshDir());
		create_directory_hierarchy(MBSTOWCS(path)+getTextureDir());
		mesh_save_path=MBSTOWCS(path)+getMeshDir()+MBSTOWCS(strrchr(name,'\\')+1);
		exportAs1Mesh(mesh,name);
	}

	gs->ReleaseIGame();

	syslog.flush();
	if(scene_cfg)
	{
		delete scene_cfg;
		scene_cfg=NULL;
	}

#if !defined(_M_X64)
	if(true)
	{
		unsigned int tmp;

		tmp = 0; __control87_2(x86_cw, _MCW_PC, &tmp, NULL);
		tmp = 0; __control87_2(sse2_cw, _MCW_PC, NULL, &tmp);
	}
#endif

	return bSucc;
}


class ExtendedSupport: public UtilityObj
{
public:
	IUtil*						iu;
	Interface*					ip;

	HWND						hMaterialPanel;
	ISpinnerControl*			spin_alpha_test_ref;
	ISpinnerControl*			spin_layer_index;
	vector<int>					material_group_items;
	MtlBase*					mtl;

	bool						initializing;

	HWND						hBitmapRelocatorPanel;

								ExtendedSupport();
	void						BeginEditParams(Interface *ip, IUtil *iu);
	void						EndEditParams(Interface *ip, IUtil *iu);
	void						DeleteThis() {}

	void						initMaterialPanel(HWND hwnd);
	void						destroyMaterialPanel(HWND hwnd);
	void						enableMaterialGroup(bool enable);

	void						editMaterial(MtlBase* mtl);
	void						commitMaterial();
	void						resetMaterial();

	void						makeSpriteUVAnim();

	bool						doesMtlExist(MtlBase* mtl);
	bool						doesMtlExistInMtl(MtlBase* mtl, Mtl* from_mtl);
	bool						doesMtlExistInScene(MtlBase* mtl, INode* node);

	void						initBitmapRelocator(HWND hwnd);
	void						destroyBitmapRelocator(HWND hwnd);
	void						relocateBitmaps();
};

static ExtendedSupport theExtendedSupport;

class ExtendedSupportClassDesc: public ClassDesc2
{
public:
	int 						IsPublic() {return 1;}
	void *						Create(BOOL loading = FALSE) {return &theExtendedSupport;}
	const TCHAR *				ClassName() {return _T("z3D Extended Support");}
	SClass_ID					SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID					ClassID() {return ExtendedSupport_CLASS_ID;}
	const TCHAR*			 	Category() {return _T("z3D Engine");}

	const TCHAR*				InternalName() { return _T("z3DExtendedSupport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE					HInstance() { return hInstance; }					// returns owning module handle
};

static ExtendedSupportClassDesc extendedSupportDesc;
ClassDesc2* GetExtendedSupportDesc() {return &extendedSupportDesc;}

static INT_PTR CALLBACK BitmapRelocatorDlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			theExtendedSupport.initBitmapRelocator(hWnd);
		}
		break;
	case WM_DESTROY:
		{
			theExtendedSupport.destroyBitmapRelocator(hWnd);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTN_RELOCATE_BITMAPS:
			{
				theExtendedSupport.relocateBitmaps();
			}
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			theExtendedSupport.ip->RollupMouseMessage(hWnd, msg, wParam, lParam); 
		}
		break;

	default:
		return FALSE;
	}
	return TRUE; 
}


static INT_PTR CALLBACK ExtendedMaterialSupportDlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			theExtendedSupport.initMaterialPanel(hWnd);
		}
		break;
	
	case WM_DESTROY:
		{
			theExtendedSupport.destroyMaterialPanel(hWnd);
		}
		break;

	case CC_SPINNER_CHANGE:
		{
			switch(LOWORD(wParam))
			{
			case IDC_SLOTSPIN_ALPHA_TEST_REF:
			case IDC_SLOTSPIN_LAYER_INDEX:
				{
					if(!theExtendedSupport.initializing)
						theExtendedSupport.commitMaterial();
				}
				break;
			}
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDOK:
				{
					theExtendedSupport.iu->CloseUtility();
				}
				break;
			case IDC_BTN_GET_ACTIVE_MATERIAL:
			case IDC_BTN_GET_SELECTED_NODE:
				{
					MtlBase* mtl = NULL;
					switch(LOWORD(wParam))
					{
					case IDC_BTN_GET_ACTIVE_MATERIAL:
						mtl = GetMtlEditInterface()->GetCurMtl();
						break;
					case IDC_BTN_GET_SELECTED_NODE:
						if(theExtendedSupport.ip->GetSelNodeCount() != 1 || !theExtendedSupport.ip->GetSelNode(0))
							MessageBox(NULL, "Please select one and only one node", "error", MB_ICONHAND);
						else
						{
							mtl = theExtendedSupport.ip->GetSelNode(0)->GetMtl();
							if(!mtl)
								MessageBox(NULL, "The node does not have a material applied", "error", MB_ICONHAND);
						}
						break;
					}
					if(!mtl)
						break;
					if(mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
					{
						theExtendedSupport.editMaterial(mtl);
					}
					else
					{
						MessageBox(NULL, "Only Standard Material is supported", "error", MB_ICONHAND);
					}
				}
				break;
			case IDC_COMBO_BLEND_OVERRIDE:
			case IDC_COMBO_ALPHA_TEST_OVERRIDE:
			case IDC_COMBO_BLEND_MODE_OVERRIDE:
			case IDC_COMBO_DIFFUSE_FILTER_OVERRIDE:
			case IDC_COMBO_SRC_BLEND_FACTOR:
			case IDC_COMBO_DST_BLEND_FACTOR:
			case IDC_COMBO_BLEND_OP:
			case IDC_CHECK_SEP_OPACITY_MAPPING:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
					{
						if(!theExtendedSupport.initializing)
							theExtendedSupport.commitMaterial();
					}
				}
				break;
			case IDC_BTN_RESET_MATERIAL_PROPERTIES:
				{
					theExtendedSupport.resetMaterial();
				}
				break;
			case IDC_BTN_MAKE_SPRITE_UV_ANIM:
				{
					theExtendedSupport.makeSpriteUVAnim();
				}
				break;

			//case IDC_APPDATA_GET:
			//	theAppDataTest.GetAppData();
			//	break;

			//case IDC_APPDATA_PUT:
			//	theAppDataTest.PutAppData();
			//	break;

			//case IDC_APPDATA_EDIT:
			//	switch (HIWORD(wParam)) {
			//		case EN_SETFOCUS:
			//			DisableAccelerators();
			//			break;
			//		case EN_KILLFOCUS:
			//			EnableAccelerators();
			//			break;
			//		}
			//	break;
			//case IDC_APPDATA_XREF:
			//	s_through_xref = IsDlgButtonChecked(hWnd, IDC_APPDATA_XREF)?true:false;
			//	break;
			}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			theExtendedSupport.ip->RollupMouseMessage(hWnd, msg, wParam, lParam); 
		}
		break;

	default:
		return FALSE;
	}
	return TRUE; 
}

void getMaterialExtendedProperties(MtlBase* mtl, Config& cfg)
{
	AppDataChunk* c = mtl->GetAppDataChunk(ExtendedSupport_AppData_CLASS_ID, UTILITY_CLASS_ID, 0);
	cfg.readFromUTF8(c ? mbs((const char*)c->data, (size_t)c->length) : mbs(""), false);
}

ExtendedSupport::ExtendedSupport()
{
	iu = NULL;
	ip = NULL;	
	hMaterialPanel = NULL;
	spin_alpha_test_ref = NULL;
	spin_layer_index = NULL;
	//spin = NULL;

	material_group_items.push_back(IDC_BTN_RESET_MATERIAL_PROPERTIES);

	material_group_items.push_back(IDC_GROUP_PROPERTIES);

	material_group_items.push_back(IDC_LBL_BLEND_OVERRIDE);
	material_group_items.push_back(IDC_COMBO_BLEND_OVERRIDE);
	material_group_items.push_back(IDC_LBL_ALPHA_TEST_OVERRIDE);
	material_group_items.push_back(IDC_COMBO_ALPHA_TEST_OVERRIDE);
	material_group_items.push_back(IDC_LBL_ALPHA_TEST_REF);
	material_group_items.push_back(IDC_SLOT_ALPHA_TEST_REF);
	material_group_items.push_back(IDC_SLOTSPIN_ALPHA_TEST_REF);
	material_group_items.push_back(IDC_LBL_LAYER_INDEX);
	material_group_items.push_back(IDC_SLOT_LAYER_INDEX);
	material_group_items.push_back(IDC_SLOTSPIN_LAYER_INDEX);

	material_group_items.push_back(IDC_LBL_BLEND_MODE_OVERRIDE);
	material_group_items.push_back(IDC_COMBO_BLEND_MODE_OVERRIDE);

	material_group_items.push_back(IDC_LBL_DIFFUSE_FILTER_OVERRIDE);
	material_group_items.push_back(IDC_COMBO_DIFFUSE_FILTER_OVERRIDE);

	material_group_items.push_back(IDC_GROUP_EXPLICIT_BLEND_MODE);

	material_group_items.push_back(IDC_LBL_SRC_BLEND_FACTOR);
	material_group_items.push_back(IDC_COMBO_SRC_BLEND_FACTOR);

	material_group_items.push_back(IDC_LBL_DST_BLEND_FACTOR);
	material_group_items.push_back(IDC_COMBO_DST_BLEND_FACTOR);

	material_group_items.push_back(IDC_LBL_BLEND_OP);
	material_group_items.push_back(IDC_COMBO_BLEND_OP);

	mtl = NULL;

	initializing = false;
}

void ExtendedSupport::BeginEditParams(Interface *ip, IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;

	mtl = NULL;

	hMaterialPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_EX_MATERIAL_PANEL),
		ExtendedMaterialSupportDlgProc,
		_T("Extended Material Support"),
		0);

	hBitmapRelocatorPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_BITMAP_RELOCATOR_PANEL),
		BitmapRelocatorDlgProc,
		_T("Bitmap Relocator"),
		0);
}

void ExtendedSupport::EndEditParams(Interface *ip, IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;

	mtl = NULL;

	ip->DeleteRollupPage(hMaterialPanel);
	ip->DeleteRollupPage(hBitmapRelocatorPanel);
	hMaterialPanel = NULL;
	hBitmapRelocatorPanel = NULL;
}

void ExtendedSupport::initMaterialPanel(HWND hWnd)
{
	hMaterialPanel = hWnd;

	if(true)
	{
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Enabled");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Disabled");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OVERRIDE, CB_SETCURSEL, 0, 0);

		RECT rt;
		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_BLEND_OVERRIDE), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_BLEND_OVERRIDE), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);
	}

	if(true)
	{
		::SendDlgItemMessageA(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Enabled");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Disabled");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_SETCURSEL, 0, 0);

		RECT rt;
		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_ALPHA_TEST_OVERRIDE), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);
	}

	spin_alpha_test_ref = SetupFloatSpinner(hWnd, IDC_SLOTSPIN_ALPHA_TEST_REF, IDC_SLOT_ALPHA_TEST_REF, 0.0f, 1.0f, 13.0f / 255.0f, 0.01f);
	spin_layer_index = SetupIntSpinner(hWnd, IDC_SLOTSPIN_LAYER_INDEX, IDC_SLOT_LAYER_INDEX, -60000, 60000, 0);

	if(true)
	{
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_MODE_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_MODE_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Explicit");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_MODE_OVERRIDE, CB_SETCURSEL, 0, 0);

		RECT rt;
		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_BLEND_MODE_OVERRIDE), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_BLEND_MODE_OVERRIDE), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);
	}

	if(true)
	{
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Linear - Linear - Linear");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Linear - Linear - None");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_ADDSTRING, 0, (LPARAM)"Nearest - Nearest - None");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_SETCURSEL, 0, 0);

		RECT rt;
		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);
	}

	if(true)
	{
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ZERO");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"INV_SRC_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_SRC_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"DEST_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_DEST_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"DEST_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_DEST_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_ALPHA_SAT");

		::SendDlgItemMessageA(hWnd, IDC_COMBO_SRC_BLEND_FACTOR, CB_SETCURSEL, 1, 0);

		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ZERO");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_SRC_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_SRC_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"DEST_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_DEST_ALPHA");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"DEST_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"ONE_MINUS_DEST_COLOR");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_ADDSTRING, 0, (LPARAM)"SRC_ALPHA_SAT");

		::SendDlgItemMessageA(hWnd, IDC_COMBO_DST_BLEND_FACTOR, CB_SETCURSEL, 0, 0);

		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_ADDSTRING, 0, (LPARAM)"ADD");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_ADDSTRING, 0, (LPARAM)"SUBTRACT");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_ADDSTRING, 0, (LPARAM)"REVERSE_SUBTRACT");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_ADDSTRING, 0, (LPARAM)"MIN");
		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_ADDSTRING, 0, (LPARAM)"MAX");

		::SendDlgItemMessageA(hWnd, IDC_COMBO_BLEND_OP, CB_SETCURSEL, 0, 0);

		RECT rt;

		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND_FACTOR), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND_FACTOR), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);

		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_DST_BLEND_FACTOR), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_DST_BLEND_FACTOR), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);

		::GetWindowRect(GetDlgItem(hWnd, IDC_COMBO_BLEND_OP), &rt);
		SetWindowPos(GetDlgItem(hWnd, IDC_COMBO_BLEND_OP), NULL, 0, 0, rt.right - rt.left, 100, SWP_NOMOVE);
	}

	CheckDlgButton(hWnd, IDC_CHECK_SEP_OPACITY_MAPPING, BST_UNCHECKED);

	enableMaterialGroup(false);
}

void ExtendedSupport::destroyMaterialPanel(HWND hWnd)
{
	ReleaseISpinner(spin_alpha_test_ref);
	ReleaseISpinner(spin_layer_index);
	//ReleaseISpinner(spin);

	hMaterialPanel = NULL;
}

void						ExtendedSupport::enableMaterialGroup(bool enable)
{
	for(size_t i = 0; i < material_group_items.size(); ++i)
		EnableWindow(GetDlgItem(hMaterialPanel, material_group_items[i]), enable);
}

void						ExtendedSupport::editMaterial(MtlBase* mtl)
{
	initializing = true;

	this->mtl = mtl;
	SetDlgItemText(hMaterialPanel, IDC_LBL_MATERIAL_NAME, mtl->GetName().data());
	enableMaterialGroup(true);
	// disable all other groups

	Config cfg(L"");
	getMaterialExtendedProperties(mtl, cfg);

	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_BLEND_OVERRIDE, CB_SETCURSEL, (WPARAM)cfg.getInt32(L"blend_override"), 0);
	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_SETCURSEL, (WPARAM)cfg.getInt32(L"alpha_test_override"), 0);
	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_BLEND_MODE_OVERRIDE, CB_SETCURSEL, (WPARAM)cfg.getInt32(L"blend_mode_override"), 0);
	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_SETCURSEL, (WPARAM)cfg.getInt32(L"diffuse_filter_override"), 0);
	spin_alpha_test_ref->SetValue(cfg.exists(L"alpha_test_ref") ? cfg.getFloat(L"alpha_test_ref") : 13.0f / 255.0f, false);
	spin_layer_index->SetValue(cfg.getInt32(L"layer_index"), false);

	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_SRC_BLEND_FACTOR, CB_SETCURSEL, (WPARAM)(cfg.exists(L"src_blend_factor") ? cfg.getInt32(L"src_blend_factor") : 4), 0);
	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_DST_BLEND_FACTOR, CB_SETCURSEL, (WPARAM)(cfg.exists(L"dst_blend_factor") ? cfg.getInt32(L"dst_blend_factor") : 5), 0);
	::SendDlgItemMessageA(hMaterialPanel, IDC_COMBO_BLEND_OP, CB_SETCURSEL, (WPARAM)cfg.getInt32(L"blend_op"), 0);

	CheckDlgButton(hMaterialPanel, IDC_CHECK_SEP_OPACITY_MAPPING, cfg.getInt32(L"sep_opacity_mapping") ? BST_CHECKED : BST_UNCHECKED);

	initializing = false;
}

void						ExtendedSupport::commitMaterial()
{
	if(!doesMtlExist(mtl))
	{
		mtl = NULL;
		enableMaterialGroup(false);
		return;
	}

	Config cfg(L"");

	int blend_override = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_BLEND_OVERRIDE, CB_GETCURSEL, 0, 0);
	int alpha_test_override = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_ALPHA_TEST_OVERRIDE, CB_GETCURSEL, 0, 0);
	REAL alpha_test_ref = spin_alpha_test_ref->GetFVal();
	int32_t layer_index = (int32_t)spin_layer_index->GetIVal();
	int blend_mode_override = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_BLEND_MODE_OVERRIDE, CB_GETCURSEL, 0, 0);
	int diffuse_filter_override = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_DIFFUSE_FILTER_OVERRIDE, CB_GETCURSEL, 0, 0);
	int src_blend_factor = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_SRC_BLEND_FACTOR, CB_GETCURSEL, 0, 0);
	int dst_blend_factor = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_DST_BLEND_FACTOR, CB_GETCURSEL, 0, 0);
	int blend_op = (int)SendDlgItemMessage(hMaterialPanel, IDC_COMBO_BLEND_OP, CB_GETCURSEL, 0, 0);
	int sep_opacity_mapping = (int)(IsDlgButtonChecked(hMaterialPanel, IDC_CHECK_SEP_OPACITY_MAPPING) == BST_CHECKED ? true : false);

	if(blend_override < 0) blend_override = 0;
	if(alpha_test_override < 0) alpha_test_override = 0;
	if(blend_mode_override < 0) blend_mode_override = 0;
	if(diffuse_filter_override < 0) diffuse_filter_override = 0;
	if(src_blend_factor < 0) src_blend_factor = 0;
	if(dst_blend_factor < 0) dst_blend_factor = 0;
	if(blend_op < 0) blend_op = 0;

	cfg.setInt32(L"blend_override", blend_override);
	cfg.setInt32(L"alpha_test_override", alpha_test_override);
	cfg.setFloat(L"alpha_test_ref", alpha_test_ref);
	cfg.setInt32(L"layer_index", layer_index);
	cfg.setInt32(L"blend_mode_override", blend_mode_override);
	cfg.setInt32(L"diffuse_filter_override", diffuse_filter_override);
	cfg.setInt32(L"src_blend_factor", src_blend_factor);
	cfg.setInt32(L"dst_blend_factor", dst_blend_factor);
	cfg.setInt32(L"blend_op", blend_op);
	cfg.setInt32(L"sep_opacity_mapping", sep_opacity_mapping);

	mtl->RemoveAppDataChunk(ExtendedSupport_AppData_CLASS_ID, UTILITY_CLASS_ID, 0);
	if(true)
	{
		mbs s = WCSTOUTF8(cfg.toWCS());
		void* buf = MAX_malloc(s.length());
		if(buf)
		{
			memcpy(buf, s.c_str(), s.length());
			mtl->AddAppDataChunk(ExtendedSupport_AppData_CLASS_ID, UTILITY_CLASS_ID, 0, (DWORD)s.length(), buf);
		}
	}
}

void						ExtendedSupport::resetMaterial()
{
	if(!mtl)
		return;

	if(!doesMtlExist(mtl))
	{
		mtl = NULL;
		enableMaterialGroup(false);
		return;
	}

	mtl->RemoveAppDataChunk(ExtendedSupport_AppData_CLASS_ID, UTILITY_CLASS_ID, 0);

	editMaterial(mtl);
}

static struct
{
	__int32			row_count;
	__int32			col_count;
	__int32			start_frame_slot;
	__int32			frame_count;
	__int32			frame_spacing;
	__int32			insert_at;

	__int32			padding;
	bool			framing_whole_texture;

} make_sprite_uv_anim_data;

static INT_PTR CALLBACK dialog3_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemInt(hwndDlg, IDC_EDIT1, 1, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT2, 1, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT3, 0, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT4, 1, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT5, 3, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT6, 0, TRUE);
			SetDlgItemInt(hwndDlg, IDC_EDIT7, 0, TRUE);

			if(true)
			{
				CheckDlgButton(hwndDlg, IDC_CHECK1, BST_UNCHECKED);

				EnableWindow(GetDlgItem(hwndDlg, IDC_GROUP_FRAME_WHOLE), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_LBL_PADDING), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT7), FALSE);
			}

			SendDlgItemMessage(hwndDlg, IDC_EDIT1, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hwndDlg, IDC_EDIT1));

			return FALSE;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				__int32 v;
				BOOL test;

				test = FALSE;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT1, &test, TRUE);
				if(v && v <= 0)
				{
					MessageBox(NULL, "Invalid No. of Rows", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.row_count = v;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT2, &test, TRUE);
				if(v && v <= 0)
				{
					MessageBox(NULL, "Invalid No. of Cols", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.col_count = v;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT3, &test, TRUE);
				if(v && v < 0)
				{
					MessageBox(NULL, "Invalid Start Frame Slot", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.start_frame_slot = v;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT4, &test, TRUE);
				if(v && v <= 0)
				{
					MessageBox(NULL, "Invalid No. of Frames", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.frame_count = v;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT5, &test, TRUE);
				if(v && v <= 0)
				{
					MessageBox(NULL, "Invalid Frame Spacing", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.frame_spacing = v;

				v = GetDlgItemInt(hwndDlg, IDC_EDIT6, &test, TRUE);
				if(v && v < 0)
				{
					MessageBox(NULL, "Invalid Append Keys @ Frame #", "Error", MB_ICONERROR);
					break;
				}
				make_sprite_uv_anim_data.insert_at = v;

				make_sprite_uv_anim_data.framing_whole_texture = false;
				if(IsDlgButtonChecked(hwndDlg, IDC_CHECK1) != BST_UNCHECKED)
				{
					make_sprite_uv_anim_data.framing_whole_texture = true;

					v = GetDlgItemInt(hwndDlg, IDC_EDIT7, &test, TRUE);
					if(v && v < 0)
					{
						MessageBox(NULL, "Invalid Padding", "Error", MB_ICONERROR);
						break;
					}
					make_sprite_uv_anim_data.padding = v;

				}

				EndDialog(hwndDlg, 1);
			}
			break;
		case IDCANCEL:
			{
				EndDialog(hwndDlg, 0);
			}
			break;
		case IDC_CHECK1:
			{
				if(IsDlgButtonChecked(hwndDlg, IDC_CHECK1) == BST_UNCHECKED)
				{
					EnableWindow(GetDlgItem(hwndDlg, IDC_GROUP_FRAME_WHOLE), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_LBL_PADDING), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT7), FALSE);
				}
				else
				{
					EnableWindow(GetDlgItem(hwndDlg, IDC_GROUP_FRAME_WHOLE), TRUE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_LBL_PADDING), TRUE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT7), TRUE);
				}
			}
			break;
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hwndDlg, 0);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void						ExtendedSupport::makeSpriteUVAnim()
{
	HMODULE module = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)dialog3_proc, &module);
	if(!module)
		return;
	if(!mtl)
		return;

	if(!doesMtlExist(mtl))
	{
		mtl = NULL;
		enableMaterialGroup(false);
		return;
	}

	if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID, 0))
		return;

	int ret = DialogBox(module, MAKEINTRESOURCE(IDD_DIALOG3), GetCOREInterface()->GetMAXHWnd(), dialog3_proc);
	if(!ret)
		return;

	Texmap* map;
	for(__int32 i = 0; i < 2; ++i)
	{
		map = NULL;
		switch(i)
		{
		case 0:
			map = findTexmap((Mtl*)mtl, ID_DI);
			break;
		case 1:
			map = findTexmap((Mtl*)mtl, ID_OP);
			break;
		}
		if(!map)
			continue;

		if(map->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
			continue;

		StdUVGen* uvgen = ((BitmapTex*)map)->GetUVGen();
		if(!uvgen)
			continue;

		int def_intan, def_outtan;

		GetCOREInterface8()->GetDefaultTangentType(def_intan, def_outtan);
		GetCOREInterface8()->SetDefaultTangentType(BEZKEY_STEP, BEZKEY_STEP, FALSE);

		SuspendAnimate();

		if(make_sprite_uv_anim_data.framing_whole_texture)
		{
			AnimateOff();

			Bitmap* bmp = ((BitmapTex*)map)->GetBitmap(GetCOREInterface()->GetTime());
			if(bmp)
			{
				int dx = bmp->Width();
				int dy = bmp->Height();
				Vec2 scl;
				scl.x = (REAL)(bmp->Width() / make_sprite_uv_anim_data.col_count - make_sprite_uv_anim_data.padding * 2) / bmp->Width();
				scl.y = (REAL)(bmp->Height() / make_sprite_uv_anim_data.row_count - make_sprite_uv_anim_data.padding * 2) / bmp->Height();
				uvgen->SetUScl(scl.x, GetCOREInterface()->GetTime());
				uvgen->SetVScl(scl.y, GetCOREInterface()->GetTime());
			}
		}

		AnimateOn();

		for(__int32 j = 0; j < make_sprite_uv_anim_data.frame_count; ++j)
		{
			Vec2 uv;
			uv.x = (REAL)((make_sprite_uv_anim_data.start_frame_slot + j) % make_sprite_uv_anim_data.col_count) / make_sprite_uv_anim_data.col_count;
			uv.y = 1.0f - (REAL)((make_sprite_uv_anim_data.start_frame_slot + j) / make_sprite_uv_anim_data.col_count) / make_sprite_uv_anim_data.row_count;

			TimeValue t = (make_sprite_uv_anim_data.insert_at + j * make_sprite_uv_anim_data.frame_spacing) * GetTicksPerFrame();

			if(!make_sprite_uv_anim_data.framing_whole_texture)
			{
				uvgen->SetUOffs(uv.x, t);
				uvgen->SetVOffs(uv.y, t);
			}
			else
			{
				Bitmap* bmp = ((BitmapTex*)map)->GetBitmap(GetCOREInterface()->GetTime());
				if(bmp)
				{
					REAL px, py;
					px = (REAL)make_sprite_uv_anim_data.padding / bmp->Width();
					py = (REAL)make_sprite_uv_anim_data.padding / bmp->Height();

					Vec3 c = (Vec3(uv.x, uv.y, 0) + Vec3(px, -py, 0)) * (Mat4::translation(-0.5f, -0.5f, 0) * Mat4::scaling(uvgen->GetUScl(GetCOREInterface()->GetTime()), uvgen->GetVScl(GetCOREInterface()->GetTime()), 1.0f) * Mat4::translation(0.5f, 0.5f, 0)).inverse();
					uvgen->SetUOffs(-c.x, t);
					uvgen->SetVOffs(-(c.y - 1.0f), t);
				}
			}
		}

		ResumeAnimate();

		GetCOREInterface8()->SetDefaultTangentType(def_intan, def_outtan, FALSE);
	}

	GetCOREInterface()->FlushUndoBuffer();
	GetSystemSetting(SYSSET_CLEAR_UNDO);

	SetSaveRequiredFlag(TRUE);
}

bool						ExtendedSupport::doesMtlExist(MtlBase* mtl)
{
	if(GetMtlEditInterface()->GetCurMtl() == mtl)
		return true;
	for(int i = 0; i < 24; ++i)
		if(GetMtlEditInterface()->GetTopMtlSlot(i) == mtl)
			return true;

	return doesMtlExistInScene(mtl, ip->GetRootNode());
}

bool						ExtendedSupport::doesMtlExistInMtl(MtlBase* mtl, Mtl* from_mtl)
{
	if(!from_mtl)
		return false;

	if(mtl == from_mtl)
		return true;

	for(int i = 0; i < from_mtl->NumSubMtls(); ++i)
		if(doesMtlExistInMtl(mtl, from_mtl->GetSubMtl(i)))
			return true;

	return false;
}

bool						ExtendedSupport::doesMtlExistInScene(MtlBase* mtl, INode* node)
{
	if(!node)
		return false;

	if(doesMtlExistInMtl(mtl, node->GetMtl()))
		return true;

	for(int i = 0; i < node->NumChildren(); ++i)
		if(doesMtlExistInScene(mtl, node->GetChildNode(i)))
			return true;
	return false;
}

void						ExtendedSupport::initBitmapRelocator(HWND hwnd)
{
}

void						ExtendedSupport::destroyBitmapRelocator(HWND hwnd)
{
}

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#include <io.h>

class BitmapRelocator : public IEnumAuxAssetsCallback
{
public:
	wcs							dirpath;

	size_t						mode;
	map<wcs, size_t>			map_src_status;
	set<wcs>					to_be_copied;
	set<wcs>					copy_failed;
	set<wcs>					copied;
public:
	BitmapRelocator()
	{
		mode = 0;
	}
public:
	bool						getFileInfo(const wcs& filepath, WIN32_FIND_DATAW& info)
	{
		HANDLE handle = FindFirstFileW(filepath.c_str(), &info);
		if(handle == INVALID_HANDLE_VALUE)
			return false;
		FindClose(handle);
		return true;
	}
public:
	void						DeclareAsset(IAssetAccessor& accessor)
	{
		if(mode == 0)
		{
#ifdef MAX_RELEASE_R12
			// this feature may be broken in this release...
			MaxSDK::Util::Path p;
			accessor.GetAsset().GetFullFilePath(p);
#else
			MaxSDK::Util::Path p = accessor.GetPath();
#endif
			const char* s = p.GetCStr();
			bool is_input = accessor.IsInputAsset();
			int asset_type = accessor.GetAssetType();
			const char* asset_desc = accessor.GetAssetDesc();
			const char* asset_client_desc = accessor.GetAssetClientDesc();
			bool retargetable = accessor.IsAssetPathWritable();

			if(!MBSTOWCS(p.GetCStr()).length())
				return;

			if(retargetable && is_input)
			{
				WIN32_FIND_DATAW src_info, dst_info;

				wcs org_src_filepath = MBSTOWCS(p.GetCStr());

#ifdef MAX_RELEASE_R12
				wcs src_filepath = MBSTOWCS(IFileResolutionManager::GetInstance()->GetFullFilePath(accessor.GetAsset()).data());
#else
				wcs src_filepath = MBSTOWCS(IPathConfigMgr::GetPathConfigMgr()->GetFullFilePath(WCSTOMBS(org_src_filepath).c_str(), false).data());
#endif
				wcs dst_filepath = dirpath + L"/" + MBSTOWCS(p.StripToLeaf().GetCStr());

				MaxSDK::Util::Path src_path(WCSTOMBS(src_filepath).c_str());
				MaxSDK::Util::Path dst_path(WCSTOMBS(dst_filepath).c_str());

				if(!getFileInfo(src_filepath, src_info))
				{
					map_src_status[org_src_filepath] = 0;
					return;
				}

				if(src_path.ResolvesEquivalent(dst_path))
				{
					map_src_status[org_src_filepath] = 1;
					return;
				}
				if(!getFileInfo(dst_filepath, dst_info))
				{
					map_src_status[org_src_filepath] = 2;
					return;
				}
				if(src_info.nFileSizeLow != dst_info.nFileSizeLow || src_info.nFileSizeHigh != dst_info.nFileSizeHigh)
				{
					map_src_status[org_src_filepath] = 3;
					return;
				}
				if(true)
				{
					FILE* src = NULL;
					FILE* dst = NULL;
					_wfopen_s(&src, src_filepath.c_str(), L"rb");
					_wfopen_s(&dst, dst_filepath.c_str(), L"rb");
					if(!src || !dst)
					{
						if(src)
							fclose(src);
						if(dst)
							fclose(dst);
						map_src_status[org_src_filepath] = 5;
						return;
					}

					char src_buffer[65536];
					char dst_buffer[65536];

					bool same = true;

					size_t len = _filelength(_fileno(src));
					for(size_t i = 0; i < len; )
					{
						size_t c = min(len - i, sizeof(src_buffer));
						fread(src_buffer, c, 1, src);
						fread(dst_buffer, c, 1, dst);

						if(memcmp(src_buffer, dst_buffer, c))
						{
							same = false;
							break;
						}

						i += c;
					}

					fclose(src);
					fclose(dst);

					if(!same)
					{
						map_src_status[org_src_filepath] = 3;
						return;
					}
					map_src_status[org_src_filepath] = 4;
				}
			}
		}
		else if(mode == 1)
		{
#ifdef MAX_RELEASE_R12
			MaxSDK::Util::Path p;
			accessor.GetAsset().GetFullFilePath(p);
#else
			MaxSDK::Util::Path p = accessor.GetPath();
#endif
			const char* s = p.GetCStr();
			bool is_input = accessor.IsInputAsset();
			int asset_type = accessor.GetAssetType();
			const char* asset_desc = accessor.GetAssetDesc();
			const char* asset_client_desc = accessor.GetAssetClientDesc();
			bool retargetable = accessor.IsAssetPathWritable();

			if(!MBSTOWCS(p.GetCStr()).length())
				return;

			if(retargetable && is_input)
			{
				wcs org_src_filepath = MBSTOWCS(p.GetCStr());

#ifdef MAX_RELEASE_R12
				wcs src_filepath = MBSTOWCS(IFileResolutionManager::GetInstance()->GetFullFilePath(accessor.GetAsset()).data());
#else
				wcs src_filepath = MBSTOWCS(IPathConfigMgr::GetPathConfigMgr()->GetFullFilePath(WCSTOMBS(org_src_filepath).c_str(), false).data());
#endif
				wcs dst_filepath = dirpath + L"/" + MBSTOWCS(p.StripToLeaf().GetCStr());

				if(!to_be_copied.count(org_src_filepath))
					return;

				if(true)
				{
					WIN32_FIND_DATAW info;
					if(getFileInfo(dst_filepath.c_str(), info))
						SetFileAttributesW(dst_filepath.c_str(), FILE_ATTRIBUTE_NORMAL);
				}
				if(!copied.count(MBSTOWCS(p.ConvertToLowerCase().StripToLeaf().GetCStr())))
				{
					if(!CopyFileW(src_filepath.c_str(), dst_filepath.c_str(), FALSE))
					{
						copy_failed.insert(org_src_filepath);
						return;
					}
					copied.insert(MBSTOWCS(p.ConvertToLowerCase().StripToLeaf().GetCStr()));
				}
#ifdef MAX_RELEASE_R12
				accessor.SetAsset(MaxSDK::AssetManagement::IAssetManager::GetInstance()->GetAsset(MaxSDK::Util::Path(WCSTOMBS(dst_filepath).c_str()), accessor.GetAsset().GetType()));
#else
				accessor.SetPath(MaxSDK::Util::Path(WCSTOMBS(dst_filepath).c_str()));
#endif
			}
		}
	}
	void						DeclareGroup(IAssetAccessor& accessor)
	{
	}
	void						EndGroup()
	{
	}
public:
	void						RecordName(MCHAR* name)
	{
	}
public:
#ifdef MAX_RELEASE_R12
	void						RecordAsset(const MaxSDK::AssetManagement::AssetUser& asset)
	{
	}
#endif
};

void						ExtendedSupport::relocateBitmaps()
{
	wcs dirpath = CONVERT(chooseFolder(L"Select a folder to relocate resources to", L"", L"", (intptr_t)ip->GetMAXHWnd()));
	if(!dirpath.length())
		return;

	BitmapRelocator relocator;
	relocator.dirpath = REPLACE(dirpath, L"\\", L"/");
	ip->EnumAuxFiles(relocator, FILE_ENUM_RENDER | FILE_ENUM_ACCESSOR_INTERFACE);

	vector<wstring> items;
	vector<wstring> descriptions;
	vector<size_t> indices;
	if(relocator.map_src_status.size())
	{
		size_t c = 0;
		for(map<wcs, size_t>::const_iterator iter = relocator.map_src_status.begin(); iter != relocator.map_src_status.end(); ++iter, ++c)
		{
			items.push_back(CONVERT(iter->first));
			switch(iter->second)
			{
			case 0:
				descriptions.push_back(L"UNAVAILABLE");
				break;
			case 1:
				descriptions.push_back(L"EXACT");
				break;
			case 2:
				descriptions.push_back(L"NEED COPY");
				indices.push_back(c);
				break;
			case 3:
				descriptions.push_back(L"!!! DIFFERENT !!! RESOLVE YOURSELF");
				break;
			case 4:
				descriptions.push_back(L"DIFFERENT PATHS ONLY");
				indices.push_back(c);
				break;
			case 5:
				descriptions.push_back(L"ERROR");
				break;
			default:
				descriptions.push_back(L"");
				break;
			}
		}
		vector<size_t> selections = selectMulti(L"Found Resources", L"Please select which resources to relocate", L"Source", L"Status", items, descriptions, indices, (intptr_t)GetCOREInterface()->GetMAXHWnd());
		if(selections.size())
		{
			for(size_t i = 0; i < selections.size(); ++i)
			{
				wcs n = CONVERT(items[selections[i]]);
				if(relocator.map_src_status.count(n))
				{
					size_t status = relocator.map_src_status[n];
					if(status == 2 || status == 4)
					{
						relocator.to_be_copied.insert(n);
						if(status == 4)
							relocator.copied.insert(MBSTOWCS(MaxSDK::Util::Path(n.c_str()).ConvertToLowerCase().StripToLeaf().GetCStr()));
					}
				}
			}

			relocator.mode = 1;

			ip->EnumAuxFiles(relocator, FILE_ENUM_RENDER | FILE_ENUM_ACCESSOR_INTERFACE);

			if(relocator.copy_failed.size())
			{
				wcs report;
				report += L"The following resources cannot be copied: \n\n";
				for(set<wcs>::const_iterator iter = relocator.copy_failed.begin(); iter != relocator.copy_failed.end(); ++iter)
					report += (*iter) + L"\n";

				MessageBoxW(GetCOREInterface()->GetMAXHWnd(), report.c_str(), L"information", 0);
			}

			FLASHWINFO info = {sizeof(FLASHWINFO), GetCOREInterface()->GetMAXHWnd(), FLASHW_ALL, 10, 0};
			FlashWindowEx(&info);
		}
	}
}

