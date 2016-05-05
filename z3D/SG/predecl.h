
#pragma once

#include "../Core/Header.h"
using namespace z3D::Core;
#include "../Intersect/Header.h"
using namespace z3D::Intersect;
#include "../IO/Header.h"
using namespace z3D::IO;
#include "../Video/Header.h"
using namespace z3D::Video;

namespace z3D
{
	namespace SG
	{
		class MtlMap;
		class Mtl;
		class LoadedMesh;
		class LoadedMtl;
		class Mesh;
		class SNode;
		class SMeshNode;
		class SCameraNode;
		class SLightNode;
		class SStructNode;
		class SMgr;
		class RenderParamQuery;
		class SRenderParamQuery;
		class SRenderItem;
		class SRenderInfo;
		class MeshRenderer;
		class SCtrlOps;
		class TMCtrl;
		class SkinCtrl;
		class MtlCtrl;
		class CameraCtrl;
		class LightCtrl;
		class VertexDeformCtrlBase;

		class DrawHelper;

		class cgDecalFXBase;
		class cgDecalFX;
		class cgPointLightFX;
		class cgCloudFX;
		class cgBeamFX;
		class cgBillBoardFX;

		class TranslateGizmo;
		class RotateGizmo;
		class ScaleGizmo;
	};
};

