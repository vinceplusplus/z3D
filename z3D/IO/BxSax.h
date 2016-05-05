
#pragma once

#include "predecl.h"

#include "SaxBase.h"

namespace z3D
{
	namespace IO
	{
		class BxSax: public SaxBase
		{
		private:
			const char * const *		_array_attrib;
			const AttribGetter::TYPE *	_array_attrib_type;
			const size_t*				_array_attrib_len;
		public:
			bool						parse(const char* buffer, size_t len, mbs& error_string);
			bool						parse(const wcs& filepath, mbs& error_string);
			void						getAttribs(const char * const * array_attrib, AttribGetter& getter);
		private:
			//bool						__parse(const char* buffer, size_t len, mbs& error_string);
		};
	};
};

