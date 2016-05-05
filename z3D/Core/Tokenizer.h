
#pragma once

#include "predecl.h"
#include "AnyValue.h"

namespace z3D
{
	namespace Core
	{
		class Token
		{
		public:
			unsigned short	type;
			AnyValue		value;
		};

		class Tokenizer
		{
		protected:
			vector<Token>	_array_token;
		public:
			Tokenizer(const mbs& utf8_str);
			virtual ~Tokenizer();
		public:
			size_t				getTokenCount() const;
			unsigned short		getTokenType(size_t idx) const;
		public:
			const wcs			getTokenWString(size_t idx) const;
			int32_t				getTokenInt32(size_t idx) const;
			uint32_t			getTokenUInt32(size_t idx) const;
			int64_t				getTokenInt64(size_t idx) const;
			uint64_t			getTokenUInt64(size_t idx) const;
			float				getTokenFloat(size_t idx) const;
			double				getTokenDouble(size_t idx) const;
		public:
			// pattern format, each wchar_t represent the token type, 'i' - identifier, 'd' - integer, 'f' - real number, 'n' - numeric (integer/real), 's' - string
			bool				matchPattern(size_t idx, const wcs& pattern) const;
		};
	};
};



