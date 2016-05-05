
#include "scn_header.h"
#include "Tokenizer.h"
#include "AnyValue.h"
#include "LexScan.h"

namespace z3D
{
	namespace Core
	{
		Tokenizer::Tokenizer(const mbs& utf8_str)
		{
			size_t count = 0;
			LexScan scan(utf8_str);
			int ret = 0;

			_array_token.resize(1024);
			while((ret = scan.scan()) != 0)
			{
				if(count >= _array_token.size())
					_array_token.resize(_array_token.size() << 1);
				Token& t = _array_token[count];
				if(ret == SCN_TOKEN_IDENTIFIER)
					t.value.setWString(scan.getIdentifier());
				else if(ret == SCN_TOKEN_INTEGER)
					t.value.setInt64(scan.getValueInt64());
				else if(ret == SCN_TOKEN_REAL)
					t.value.setDouble(scan.getValueDouble());
				else if(ret == SCN_TOKEN_STRING)
					t.value.setWString(scan.getValueWString());
				else
					continue;
				t.type = ret;
				++count;
			}
			_array_token.resize(count);
		}
		Tokenizer::~Tokenizer()
		{
		}
		size_t				Tokenizer::getTokenCount() const
		{
			return _array_token.size();
		}
		unsigned short		Tokenizer::getTokenType(size_t idx) const
		{
			return _array_token[idx].type;
		}
		const wcs			Tokenizer::getTokenWString(size_t idx) const
		{
			return _array_token[idx].value.getWString();
		}
		int32_t				Tokenizer::getTokenInt32(size_t idx) const
		{
			return _array_token[idx].value.getInt32();
		}
		uint32_t			Tokenizer::getTokenUInt32(size_t idx) const
		{
			return _array_token[idx].value.getUInt32();
		}
		int64_t				Tokenizer::getTokenInt64(size_t idx) const
		{
			return _array_token[idx].value.getInt64();
		}
		uint64_t			Tokenizer::getTokenUInt64(size_t idx) const
		{
			return _array_token[idx].value.getUInt64();
		}
		float				Tokenizer::getTokenFloat(size_t idx) const
		{
			return _array_token[idx].value.getFloat();
		}
		double				Tokenizer::getTokenDouble(size_t idx) const
		{
			return _array_token[idx].value.getDouble();
		}
		bool					Tokenizer::matchPattern(size_t idx, const wcs& pattern) const
		{
			if(idx + pattern.length() > _array_token.size())
				return false;
			size_t end = idx + pattern.length();
			size_t i, j;
			for(i = 0, j = idx; j < end; ++i, ++j)
			{
				switch(pattern[i])
				{
				case 'i':
					if(_array_token[j].type != SCN_TOKEN_IDENTIFIER)
						return false;
					break;
				case 'd':
					if(_array_token[j].type != SCN_TOKEN_INTEGER)
						return false;
					break;
				case 'f':
					if(_array_token[j].type != SCN_TOKEN_REAL)
						return false;
					break;
				case 'n':
					if(_array_token[j].type != SCN_TOKEN_INTEGER && _array_token[j].type != SCN_TOKEN_REAL)
						return false;
					break;
				case 's':
					if(_array_token[j].type != SCN_TOKEN_STRING)
						return false;
					break;
				default:
					return false;
				}
			}
			return true;
		}
	};
};


