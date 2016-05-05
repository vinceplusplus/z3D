
#include "AnyValue.h"
#include "Misc.h"

namespace z3D
{
	namespace Core
	{
		AnyValue::AnyValue()
		{
			_type = TYPE_WSTRING;
			_int64_value = 0;
			_double_value = 0;
		}
		size_t			AnyValue::getType() const
		{
			return _type;
		}
		const wcs		AnyValue::getWString() const
		{
			switch(_type)
			{
			case TYPE_WSTRING:
				return _wstring_value;
			case TYPE_INT64:
				return FORMATW(L"%I64d", _int64_value);
			case TYPE_DOUBLE:
				return FORMATW(L"%f", _double_value);
			default:
				return L"";
			}
		}
		int32_t			AnyValue::getInt32() const
		{
			return (int32_t)getInt64();
		}
		uint32_t		AnyValue::getUInt32() const
		{
			return (uint32_t)getInt64();
		}
		int64_t			AnyValue::getInt64() const
		{
			switch(_type)
			{
			case TYPE_WSTRING:
				return (int64_t)_wtoi64(_wstring_value.c_str());
			case TYPE_INT64:
				return (int64_t)_int64_value;
			case TYPE_DOUBLE:
				return (int64_t)_double_value;
			default:
				return 0;
			}
		}
		uint64_t		AnyValue::getUInt64() const
		{
			return (uint64_t)getInt64();
		}
		float			AnyValue::getFloat() const
		{
			return (float)getDouble();
		}
		double			AnyValue::getDouble() const
		{
			switch(_type)
			{
			case TYPE_WSTRING:
				return (double)_wtof(_wstring_value.c_str());
			case TYPE_INT64:
				return (double)_int64_value;
			case TYPE_DOUBLE:
				return (double)_double_value;
			default:
				return 0;
			}
		}
		void				AnyValue::setWString(const wcs& value)
		{
			_wstring_value = value;
			_type = TYPE_WSTRING;
		}
		void				AnyValue::setInt32(int32_t value)
		{
			setInt64((int64_t)value);
		}
		void				AnyValue::setUInt32(uint32_t value)
		{
			setInt64((int64_t)value);
		}
		void				AnyValue::setInt64(int64_t value)
		{
			_int64_value = value;
			_type = TYPE_INT64;
		}
		void				AnyValue::setUInt64(uint64_t value)
		{
			setInt64((int64_t)value);
		}
		void				AnyValue::setFloat(float value)
		{
			setDouble((double)value);
		}
		void				AnyValue::setDouble(double value)
		{
			_double_value = value;
			_type = TYPE_DOUBLE;
		}
	};
};
