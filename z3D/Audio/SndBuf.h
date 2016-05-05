
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Audio
	{
		class SndBuf: public ResBase
		{
		protected:
			unsigned int		_buffer_name;
		public:
			SndBuf(const wcs& filepath,unsigned int buffer_name);
		protected:
			virtual ~SndBuf();
		public:
			unsigned int		buffer_name() const;
		public:
			REAL				duration() const;
		};
	};
};
