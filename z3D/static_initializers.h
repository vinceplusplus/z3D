
#if defined(_MSC_VER)
#	if __COUNTER__ != 0
#		error this file can only be included once per application
#	endif
#endif

/*************************************************************

important static initializers must be put here
the order of initialization must be appropriate because
one initializer may depend on another

*************************************************************/

#pragma init_seg(lib)

#ifdef Z3D_ENABLE_VIDEO
	#ifndef Z3D_ENABLE_FREEIMAGE
		#define Z3D_ENABLE_FREEIMAGE
	#endif
#endif

#include "dynamic_memory_allocator_inl.h"

#ifdef Z3D_ENABLE_FREEIMAGE

#include "../FreeImage/Header.h"

class FreeImageInitializer
{
public:
	FreeImageInitializer()	{FreeImage_Initialise(1);}
	~FreeImageInitializer()	{FreeImage_DeInitialise();}
};

static FreeImageInitializer fii;

#endif

#if defined(Z3D_ENABLE_CORE)

#include "Core/Critical/Logger.h"
#include "Core/AABB.h"
#include "Core/Constants.h"
#include "Core/ResBase.h"
#include "Core/sync_spin.h"
#include "Core/mutex_fast.h"
#include "Core/mem_pool.h"
#include "Core/mem_pool_inl.h"
#include "Core/lfqueue_inl.h"
#include "Core/TypeStuff.h"
#include "Core/singleton.h"
#include "Core/Matrix2.h"
#include "Core/Matrix3.h"
#include "Core/Matrix4.h"
#include "Core/Vector2.h"
#include "Core/Vector3.h"
#include "Core/Vector4.h"
#include "Core/LOD.h"
#include "Core/TimeAnim.h"
#include "Core/AnimInst.h"
#include "Core/Timer.h"
#include "Core/TimerService.h"
#include "Core/sqlite.h"
#include "Core/CtrlBase.h"
#include "Core/LoggerSTL.h"
#include "Core/version.h"
#include "Core/fast_string.h"
#include "Core/allocators.h"
#include "Core/allocators_inl.h"
#include "Core/functor.h"
#include "Core/functor_inl.h"

#include "Core/CoreDeinit.h"

#include <limits>

namespace z3D
{
	namespace Core
	{
		// threading
		volatile uint32_t			 	sync_spin::_spin = 0;

#ifdef Z_USE_FAST_STRING
		// fast string
		Z3D_API str_allocator_pool::POOL* str_allocator_pool::pool = str_allocator_pool::POOL::create(1024);

		class str_allocator_pool_shutdown
		{
		public:
			~str_allocator_pool_shutdown()
			{
				str_allocator_pool::pool->dispose();
			}
		};

		static str_allocator_pool_shutdown str_allocator_pool_shutdown_instance;

#endif

		// version
		volatile uint64_t Z_ALIGN(8) 	version::_cur_ver = 0;
		volatile uint64_t Z_ALIGN(8) 	version::_cur_group = 0;

		Z3D_API LoggerSTL			syslog(&syslog_basic);

		// frame temp memory allocator
		Z3D_API fast_temp_mem_allocator<8 * 1048576, 8192>		g_frame_temp_mem_allocator;

		// singleton
		vector<singleton_base*>			singleton_base::_array_singleton;

		// smart pointer
		mem_pool<uint64_t>* shared_count::_pool = mem_pool<uint64_t>::create(shared_count::POOL_MAX_BLOCKS);

		// smart pointer locker
		Z3D_API vector<SPtr<RefBase>>			SPtrLocker::_sptrs;

		// type info infrastructure
		Z3D_API vector<TypeInfo*>	TypeInfo::_g_type_info;
		mutex				TypeInfo::_g_mx;
		uint32_t			TypeInfo::_g_next_id = 0;

		// some core deinitializers
		CoreDeinit			CoreDeinit::_instance;					// because it accesses TypeInfo and shared_count, it must be after those two, so that when program exits, it will get called before those two

#ifdef SPTR_THREAD_SAFE
		Z3D_API mutex		RefBaseImpl::_mx_sptr;
#endif

		// misc
		mutex_fast			mx_formata;
		mutex_fast			mx_formatw;
		vector<char>		formata_buffer(8192);
		vector<wchar_t>		formatw_buffer(8192);

		char				Timer::_cpu_id[16];
		uint64_t			Timer::_cpu_speed;
		uint8_t				Timer::_cpus;
		uint8_t				Timer::_affinities;
		uint8_t				Timer::_intel_speedstep;
		uint8_t				Timer::_timer_impl;
		uint64_t			Timer::_qpc_freq;

		static TimerService				timerService;

		LOD::INTERPOLATION				LOD::_force_interpolation = LOD::INTERPOLATION_NOT_SPECIFIED;
		LOD::TRANSITION					LOD::_force_transition = LOD::TRANSITION_NOT_SPECIFIED;

		// math thing
		Z3D_API const AABB AABB::empty_bound = AABB();

		Z3D_API const REAL EPSILON = std::numeric_limits<REAL>::epsilon();
		Z3D_API const REAL INFINITY = std::numeric_limits<REAL>::infinity();
		Z3D_API const REAL SMALLEST_INVERTIBLE_MATRIX_DET = (REAL)pow((double)std::numeric_limits<REAL>::epsilon(), 2.015625);
		Z3D_API const REAL SMALLEST_NORMALIZABLE_VECTOR_LENGTHSQ = (REAL)pow((double)std::numeric_limits<REAL>::epsilon(), 4.015625);
		Z3D_API const REAL PI = (REAL)(acos(0.0) * 2);
		Z3D_API const wchar_t UNICODE_BYTE_ORDER_MARK = 0xfeff;

		Z3D_API const REAL DEG_TO_RAD = (REAL)(acos(0.0) / 90.0);
		Z3D_API const REAL RAD_TO_DEG = (REAL)(90.0 / acos(0.0));

		Z3D_API const Mat2 Mat2::identity = Mat2::identityMatrix();

		Z3D_API const Mat3 Mat3::identity = Mat3::identityMatrix();

		Z3D_API const Mat4 Mat4::identity = Mat4::identityMatrix();

		Z3D_API const Vec2 Vec2::zero = Vec2(0.0f, 0.0f);
		Z3D_API const Vec2 Vec2::xAxis = Vec2(1.0f, 0.0f);
		Z3D_API const Vec2 Vec2::yAxis = Vec2(0.0f, 1.0f);
		Z3D_API const Vec2 Vec2::xNegAxis = Vec2(-1.0f, 0.0f);
		Z3D_API const Vec2 Vec2::yNegAxis = Vec2(0.0f, -1.0f);

		Z3D_API const Vec3 Vec3::zero = Vec3(0.0f, 0.0f, 0.0f);
		Z3D_API const Vec3 Vec3::xAxis = Vec3(1.0f, 0.0f, 0.0f);
		Z3D_API const Vec3 Vec3::yAxis = Vec3(0.0f, 1.0f, 0.0f);
		Z3D_API const Vec3 Vec3::zAxis = Vec3(0.0f, 0.0f, 1.0f);
		Z3D_API const Vec3 Vec3::xNegAxis = Vec3(-1.0f, 0.0f, 0.0f);
		Z3D_API const Vec3 Vec3::yNegAxis = Vec3(0.0f, -1.0f, 0.0f);
		Z3D_API const Vec3 Vec3::zNegAxis = Vec3(0.0f, 0.0f, -1.0f);

		Z3D_API const Vec4 Vec4::zero = Vec4(0.0, 0.0, 0.0, 1.0);
		Z3D_API const Vec4 Vec4::xAxis = Vec4(1.0, 0.0, 0.0, 1.0);
		Z3D_API const Vec4 Vec4::yAxis = Vec4(0.0, 1.0, 0.0, 1.0);
		Z3D_API const Vec4 Vec4::zAxis = Vec4(0.0, 0.0, 1.0, 1.0);
		Z3D_API const Vec4 Vec4::xNegAxis = Vec4(-1.0, 0.0, 0.0, 1.0);
		Z3D_API const Vec4 Vec4::yNegAxis = Vec4(0.0, -1.0, 0.0, 1.0);
		Z3D_API const Vec4 Vec4::zNegAxis = Vec4(0.0, 0.0, -1.0, 1.0);

		// animation
		Timer				TimeAnim::_auto_advance_timer;
		Timer				AnimInst::_auto_advance_timer;

		// sqlite
		sqlite_res			sqlite_db::_null_res;

		// type info
		IMPL_TYPEINFO_ROOT(L"z3D::Core::AnimInst", L"", AnimInst);
		IMPL_TYPEINFO_ROOT(L"z3D::Core::CtrlBase", L"", CtrlBase);

		// explicitly instantiate the functor that is used by SPtr, so we don't have to have all files include functor_inl.h
		template functor1<void, void*>;

#pragma comment(lib,"libmd5.lib")
#pragma comment(lib,"crc.lib")
#pragma comment(lib,"sqlite3.lib")
#pragma comment(lib,"zlib.lib")

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

#pragma comment(lib, "winmm.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_IO

#include "IO/FileSystem.h"
#include "IO/Win32FileSys.h"

namespace z3D
{
	namespace IO
	{
		vector<SPtr<FileSystem>>		FileSystem::_array_filesys;

		SPtr<Win32FileSystem>			Win32FileSystem::_instance = new Win32FileSystem();
		class IOInitializer	{	public:	IOInitializer()	{FileSystem::insertFileSystem(Win32FileSystem::_instance);	}	};

		static IOInitializer			initializer;

		#pragma comment(lib, "expat.lib")
	};
};

#endif


#ifdef Z3D_ENABLE_VIDEO

#include "Video/Gfx.h"
#include "Video/Tex2D.h"
#include "Video/VBO.h"
#include "Video/IBO.h"
#include "Video/Tex2DGIF.h"
#include "Video/Tex2DAVI.h"
#include "Video/TexCube.h"
#include "Video/Renderbuffer.h"
#include "Video/Img2D.h"
#include "Video/AVIReader.h"
#include "Video/AVIRenderer.h"
#if defined(Z3D_ENABLE_VIDEO_D3D)
	#include "Video/d3dGfx.h"
	#include "Video/d3dShader.h"
	#include "Video/d3dVBO.h"
	#include "Video/d3dIBO.h"
	#include "Video/d3dTex2D.h"
	#include "Video/d3dTexCube.h"
	#include "Video/d3dRenderbuffer.h"
#endif
#include "Video/TexFont.h"
#include "Video/TexFont_Internal.h"
#include "Video/ShaderUniforms2.h"

namespace z3D
{
	IMPLEMENT_SINGLETON(z3D::Video::Gfx);

	namespace Video
	{
		IMPL_TYPEINFO_ROOT(L"z3D::Video::Gfx", L"", Gfx);
		IMPL_TYPEINFO_ROOT(L"z3D::Video::Shader", L"", Shader);
		IMPL_TYPEINFO_ROOT(L"z3D::Video::TexBase", L"", TexBase);
		IMPL_TYPEINFO_ROOT(L"z3D::Video::Renderbuffer", L"", Renderbuffer);
		IMPL_TYPEINFO_ROOT(L"z3D::Video::VBO", L"", VBO);
		IMPL_TYPEINFO_ROOT(L"z3D::Video::IBO", L"", IBO);
		IMPL_TYPEINFO(L"z3D::Video::Tex2D", L"", Tex2D, TexBase);
		IMPL_TYPEINFO(L"z3D::Video::TexCube", L"", TexCube, TexBase);
		IMPL_TYPEINFO(L"z3D::Video::Tex2DAVI", L"", Tex2DAVI, Tex2D);
		IMPL_TYPEINFO(L"z3D::Video::Tex2DGIF", L"", Tex2DGIF, Tex2D);

		#if defined(Z3D_ENABLE_VIDEO_D3D)
			IMPL_TYPEINFO(L"z3D::Video::d3dGfx", L"", d3dGfx, Gfx);
			IMPL_TYPEINFO(L"z3D::Video::d3dShader", L"", d3dShader, Shader);
			IMPL_TYPEINFO(L"z3D::Video::d3dTex2D", L"", d3dTex2D, Tex2D);
			IMPL_TYPEINFO(L"z3D::Video::d3dTexCube", L"", d3dTexCube, TexCube);
			IMPL_TYPEINFO(L"z3D::Video::d3dRenderbuffer", L"", d3dRenderbuffer, Renderbuffer);
			IMPL_TYPEINFO(L"z3D::Video::d3dVBO", L"", d3dVBO, VBO);
			IMPL_TYPEINFO(L"z3D::Video::d3dIBO", L"", d3dIBO, IBO);
		#endif

		unsigned int					Tex2D::MAX_RES = 256;
		bool							Tex2D::FORCE_16BIT = false;

		unsigned int					Tex2DGIF::MAX_RES = 256;
		bool							Tex2DGIF::FORCE_16BIT = false;

		ResMgr							AVIReader::_res_mgr;
		bool							AVIRenderer::_inited = false;
		map<long,long>					AVIRenderer::_map_fcc_codecid;

		unsigned int					Tex2DAVI::MAX_RES = 256;
		bool							Tex2DAVI::FORCE_16BIT = false;
		map<wcs,bool>					Tex2DAVI::_map_cache_frames_filepath;

		unsigned int					TexCube::MAX_RES = 256;
		bool							TexCube::FORCE_16BIT = false;

		ResMgr							Img2D::_res_mgr;
		hash_map<wcs,SPtr<Img2D> >		Img2D::_packed_img_lookup;
		bool							Img2D::FORCE_16BIT = false;
		SPtr<Tex2D>						Img2D::_null_tex2d;

		ResMgr								FontCache::_res_mgr;

		FTLibrary*							FTLibrary::_instance = NULL;

		vector< SPtr<FontCache> >			TexFont::_array_font_cache;
		map<mbs,SPtr<FontFaceLoadInfo> >	TexFont::_map_name_load_info;

		in_place_allocator<ShaderUniforms2::BUFFERSET[2]>	ShaderUniforms2::_buffer_allocator;

		#if defined(Z3D_ENABLE_VIDEO_D3D)
			#pragma comment(lib, "dxerr.lib")
			#pragma comment(lib,"dxguid.lib")
			#ifdef _DEBUG
				#pragma comment(lib,"d3dx9d.lib")
			#else
				#pragma comment(lib,"d3dx9.lib")
			#endif
			#pragma comment(lib,"d3d9.lib")

		#endif

		// to build the following libraies, check out ffdshow first
		#pragma comment(lib,"libavcodec.lib")						// find and open libavcodec solution, change the libavcodec project targeting from dll to static library, exclude dllentry.c, then build, copy the debug build to bin_dbg and the release build to bin
		#pragma comment(lib,"libmplayer.lib")							// find and open mplayerlib solution, change the mplayerlib project target from dll to static library, exclude all inside mp3 filter, exclude mplayer.c, then build, copy the debug build to bin_dbg and the release build to bin

		#pragma comment(lib,"freetype.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_SG

#include "SG/Mesh.h"
#include "SG/SNode.h"
#include "SG/SMeshNode.h"
#include "SG/SCameraNode.h"
#include "SG/SLightNode.h"
#include "SG/SBoneNode.h"
#include "SG/SKDTNode.h"
#include "SG/SStructNode.h"
#include "SG/VertexDeformCtrlBase.h"
#include "SG/TMCtrl.h"
#include "SG/MorpherCtrl.h"
#include "SG/SkinCtrl.h"
#include "SG/MtlCtrl.h"
#include "SG/MeshRenderer.h"
#include "SG/SMgr.h"
#include "SG/CameraCtrl.h"
#include "SG/LightCtrl.h"
#include "SG/DataLoader.h"
#include "SG/cgDecalFX.h"
#include "SG/cgCloudFX.h"
#include "SG/cgBeamFX.h"
#include "SG/cgBillBoardFX.h"

namespace z3D
{
	IMPLEMENT_SINGLETON(z3D::SG::SMgr);

	namespace SG
	{
		size_t							SNode::_next_id = 1;
		//size_t						SNode::_next_userprop_id = 0;
		//vector<wstring>				SNode::_array_userprop_name;
		//wstring						SNode::_empty_userprop_name;

		SPtr<btTriTree>					SNode::_empty_spt;
		//SPtr<ocTriTree>					SNode::_empty_spt;

		MtlMap							MtlMap::_no_transform;

		ResMgr							LoadedMesh::_res_mgr;
		ResMgr							LoadedMtl::_res_mgr;
		ResMgr							SkinData::_res_mgr;
		ResMgr							MorpherData::_res_mgr;
		ResMgr							AnimDataArchive::_res_mgr;

		SPtr<DefMeshRenderer>			DefMeshRenderer::_instance;

		vector<SRenderInfo*>			SRenderInfoStatic::_array_ri;
		vector<SRenderInfo*>			SRenderInfoStatic::_array_free_ri;
		SRenderInfoStatic::SRenderInfoStaticsDestroyer		SRenderInfoStatic::_destroyer;					// must after _array_free_ri, because destroy needs access to _array_free_ri

		//SPtr<mrParamHostScene>			mrParamHostScene::_built_in = new mrParamHostScene();
		//SPtr<mrDefault>					mrDefault::_built_in = new mrDefault(mrParamHostScene::built_in());

		IMPL_TYPEINFO_ROOT(L"z3D::SG::SNode", L"", SNode);
		IMPL_TYPEINFO(L"z3D::SG::SMeshNode", L"", SMeshNode, SNode);
		IMPL_TYPEINFO(L"z3D::SG::SCameraNode", L"", SCameraNode, SNode);
		IMPL_TYPEINFO(L"z3D::SG::SLightNode", L"", SLightNode, SNode);
		IMPL_TYPEINFO(L"z3D::SG::SBoneNode", L"", SBoneNode, SNode);
		IMPL_TYPEINFO(L"z3D::SG::SKDTNode", L"", SKDTNode, SNode);
		IMPL_TYPEINFO(L"z3D::SG::SStructNode", L"", SStructNode, SKDTNode);
		//IMPL_TYPEINFO(L"z3D::SG::SStructNode", L"", SStructNode, SNode);

		IMPL_TYPEINFO(L"z3D::SG::VertexDeformCtrlBase", L"", VertexDeformCtrlBase, CtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::TMCtrl", L"", TMCtrl, CtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::SkinCtrl", L"", SkinCtrl, VertexDeformCtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::MorpherCtrl", L"", MorpherCtrl, VertexDeformCtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::MtlCtrl", L"", MtlCtrl, CtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::CameraCtrl", L"", CameraCtrl, CtrlBase);
		IMPL_TYPEINFO(L"z3D::SG::LightCtrl", L"", LightCtrl, CtrlBase);

		IMPL_TYPEINFO(L"z3D::SG::cgDecalFXBase", L"", cgDecalFXBase, SNode);
		IMPL_TYPEINFO(L"z3D::SG::cgDecalFX", L"", cgDecalFX, cgDecalFXBase);
		IMPL_TYPEINFO(L"z3D::SG::cgPointLightFX", L"", cgPointLightFX, cgDecalFXBase);
		IMPL_TYPEINFO(L"z3D::SG::cgCloudFX", L"", cgCloudFX, SNode);
		IMPL_TYPEINFO(L"z3D::SG::cgBeamFX", L"", cgBeamFX, SNode);
		IMPL_TYPEINFO(L"z3D::SG::cgBillBoardFX", L"", cgBillBoardFX, SNode);

		const Vec4 Mtl::OPAQUE_WHITE = Vec4(1,1,1,1);

		//vector<SLightNode*>				SLightNode::_array_global_light;

#pragma comment(lib,"Opcode.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_AUDIO

#include "Audio/Oal.h"
#include "Audio/wavSndPlayer.h"

namespace z3D
{
	IMPLEMENT_SINGLETON(z3D::Audio::Oal);

	namespace Audio
	{
		ResMgr							wavSndPlayer::_res_mgr;
#pragma comment(lib,"openal32.lib")
#pragma comment(lib,"ogg.lib")
#pragma comment(lib,"vorbis.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_GAMEFRM

#include "GameFrm/W32Wnd.h"
#include "GameFrm/uiMgr.h"

namespace z3D
{
	IMPLEMENT_SINGLETON(z3D::GameFrm::uiMgr);
	namespace GameFrm
	{
		map<HWND,W32Wnd*>		W32Wnd::_map_hwnd_obj;
		HCURSOR					W32Wnd::_default_cursor = NULL;
	};
};

#endif

#ifdef Z3D_ENABLE_TCPFRM

#include "TCPFrm/NetPools.h"
#include "TCPFrm/NetSys.h"

namespace z3D
{
	namespace TCPFrm
	{
		mem_pool<NetMsg>*						NetPools::_pool_netmsg = NULL;
		mem_pool<SENDOL>*						NetPools::_pool_sendol = NULL;
		lfqueue<BigMsg::MEMINFO>				NetPools::_queue_reusable_bigmsg_mem;

		mutex_fast		NetSys::_mx_rsa;
		void*			NetSys::_rsa_ctx = NULL;

		// use NetSys::gen_symmetric_encryption_table() to generate the following data
		unsigned char	NetSys::_sym_encrypt_table[256] = {
			0x98, 0xD0, 0xB3, 0xA4, 0xB5, 0xF4, 0x79, 0x2A, 0x4C, 0xD5, 0x3E, 0x7C, 0xA2, 0xE9, 0x86, 0x83, 
			0x4F, 0xCD, 0x29, 0x3D, 0x46, 0x3B, 0x16, 0xAB, 0xE3, 0x38, 0xDA, 0x6B, 0x6E, 0x34, 0x31, 0x3E, 
			0xC8, 0xF8, 0x2C, 0xF8, 0xF2, 0xD3, 0x9D, 0xA1, 0xC2, 0x73, 0xA7, 0x54, 0x97, 0x1A, 0x49, 0x34, 
			0x4A, 0x49, 0xE0, 0x31, 0xD3, 0x0D, 0x2A, 0x38, 0x07, 0x1F, 0x1C, 0x26, 0xCE, 0x15, 0x07, 0x76, 
			0x29, 0xE3, 0x61, 0xE9, 0x67, 0x86, 0x4A, 0x52, 0x19, 0x3E, 0x62, 0x15, 0x52, 0x43, 0x4C, 0x49, 
			0xE0, 0xD6, 0xC2, 0x68, 0xC8, 0x6B, 0xD6, 0x94, 0x0B, 0x1F, 0xDC, 0xEC, 0x9D, 0x9B, 0xF1, 0x3B, 
			0x7A, 0x16, 0x9E, 0x5E, 0x58, 0x37, 0x2A, 0x46, 0xC4, 0x16, 0xA8, 0xB6, 0x51, 0x0D, 0x3D, 0xC7, 
			0xE6, 0xB3, 0xBA, 0x45, 0xB5, 0xC7, 0x2F, 0x5B, 0x13, 0x98, 0x43, 0x23, 0x0E, 0x5B, 0x4A, 0x62, 
			0x3D, 0x91, 0x73, 0xE5, 0x92, 0x0B, 0xCB, 0x5D, 0x8A, 0x29, 0x3B, 0x2F, 0x6D, 0xB6, 0xAD, 0x51, 
			0xB0, 0xBC, 0x23, 0x70, 0xA8, 0x0E, 0x1F, 0x8F, 0x07, 0x13, 0x1C, 0x19, 0xE6, 0x54, 0x1C, 0x43, 
			0xF4, 0xEA, 0xC4, 0xCB, 0x4C, 0x83, 0x61, 0x32, 0x5D, 0x57, 0x58, 0x9E, 0xD9, 0x5E, 0x6D, 0xAD, 
			0x0E, 0xB9, 0x23, 0x19, 0x89, 0xC1, 0xD5, 0x37, 0x64, 0xA2, 0x1A, 0xDA, 0x68, 0xE5, 0xF2, 0xA7, 
			0xBC, 0x1A, 0x26, 0x91, 0x25, 0x79, 0xB0, 0x2C, 0x32, 0x15, 0x67, 0x64, 0x0D, 0x76, 0x8C, 0xCE, 
			0x97, 0x7C, 0x37, 0x89, 0x25, 0xA4, 0x2C, 0xD9, 0x45, 0xAB, 0x70, 0xD0, 0xEC, 0x32, 0x57, 0x34, 
			0x75, 0xA1, 0xCD, 0xC1, 0xF1, 0xAE, 0x8C, 0x94, 0x45, 0x75, 0x0B, 0x38, 0x92, 0x46, 0x31, 0x26, 
			0x85, 0x9B, 0x13, 0xB9, 0xBA, 0x25, 0x4F, 0x2F, 0x8A, 0xAE, 0x85, 0xEA, 0x8F, 0x6E, 0xDC, 0x7A
		};

		class TCPFrmDeinit
		{
		public:
			~TCPFrmDeinit()
			{
				NetSys::disposePools();
				NetSys::shutdown();
			}
		};
		static TCPFrmDeinit deinitializer;
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#pragma comment(lib,"xyssl.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_MYSQL

namespace z3D
{
	namespace mySQL
	{
#pragma comment(lib,"libmysql.lib")
	};
};

#endif

#ifdef Z3D_ENABLE_INTERSECT

#endif

#ifdef Z3D_ENABLE_PHYSICS

namespace z3D
{
	namespace Physics
	{
#pragma comment(lib,"bullet.lib")
	};
};

#endif