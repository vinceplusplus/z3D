
#pragma once

#include "predecl.h"
#include "scn_header.h"
#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class LexScan
		{
		protected:
			mbs				_src_str_utf8;
			istrstream		_src_stream;
			void*			_flex_scanner;
		public:
			LexScan(const mbs& str_utf8);
			virtual ~LexScan();
		public:
			int				scan();						// return 0 as the end
		public:
			const wcs&		getIdentifier() const;
			const wcs&		getValueWString() const;
			__int64			getValueInt64() const;
			double			getValueDouble() const;
		public:
			wcs				getScanLex() const;
		};
	};
};


