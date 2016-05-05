
#pragma once

#include "predecl.h"
#include "Gfx.h"

namespace z3D
{
	namespace Video
	{
		class TexBase: public RefBase
		{
		public:
			DECL_TYPEINFO();
		protected:
											TexBase();
			virtual							~TexBase();
		public:
			virtual GfxEnum::TEXTARGET		getTarget() const = 0;
		};

		//class TexBase: public ResBase
		//{
		//public:
		//	DECL_TYPEINFO();
		//protected:
		//	ptrdiff_t					_known_opaque;
		//public:
		//	version						ver;
		//protected:
		//								TexBase(const wcs& filepath);
		//	virtual						~TexBase();
		//public:
		//	virtual GfxEnum::TEXTARGET	tex_target() const = 0;
		//public:
		//	virtual void				update();
		//public:
		//	virtual SPtr<TexBase>		getRealTexture();
		//public:
		//	static bool					supportsNonPower2();
		//public:
		//	void						setKnownOpaque(bool opaque);
		//	bool						knownOpaque() const;
		//};
	};
};


