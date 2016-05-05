
#pragma once

#include "predecl.h"

#include "fast_string.h"

namespace z3D
{
	namespace Core
	{
		class AnyValue
		{
		public:
			enum
			{
				TYPE_WSTRING,
				TYPE_INT64,
				TYPE_DOUBLE
			};
		public:
			size_t				_type;
			wcs					_wstring_value;
			int64_t				_int64_value;
			double				_double_value;
		public:
			AnyValue();
		public:
			size_t				getType() const;
		public:
			const wcs			getWString() const;
			int32_t				getInt32() const;
			uint32_t			getUInt32() const;
			int64_t				getInt64() const;
			uint64_t			getUInt64() const;
			float				getFloat() const;
			double				getDouble() const;
		public:
			void				setWString(const wcs& value);
			void				setInt32(int32_t value);
			void				setUInt32(uint32_t value);
			void				setInt64(int64_t value);
			void				setUInt64(uint64_t value);
			void				setFloat(float value);
			void				setDouble(double value);
		};
	};
};

