
#pragma once

#include "../Core/Header.h"
using namespace z3D::Core;
#include "../IO/Header.h"
using namespace z3D::IO;

#define Z3D_ENABLE_VIDEO_D3D

namespace z3D
{
	namespace Video
	{
		class rsView;
		class rsShade;

		class LockParams;
		class GfxHelper;
		class Tex2DCreateParams;
		class TexCubeCreateParams;

		class Gfx;
		class VBO;
		class IBO;
		class TexBase;
		class Tex2D;
		class TexCube;
		class Renderbuffer;
		class Img2D;
		class Jotter;
		class ShaderUniforms;
		class ShaderUniforms2;

		class Tex2DGIFCF;
		class Tex2DGIF;

		class AVIReader;
		class AVIRenderer;
		class Tex2DAVICF;
		class Tex2DAVI;

		class DrawTextOperation;
		class TexFont;

		class Shader;

		class ShaderGen;
		class ShaderGenConf;
	};
};

