
#include "TypeStuff.h"

#include <stdarg.h>

namespace z3D
{
	namespace Core
	{
		TypeInfo::TypeInfo(const wcs& name, const wcs& desc) : _name(name), _desc(desc), _parent(0), _kind_of(0) {}
		TypeInfo::~TypeInfo() {}
		TypeInfo*			TypeInfo::registerType(const wchar_t* name, const wchar_t* desc, size_t parents, ...)
		{
			TypeInfo* newtype = new TypeInfo(name, desc);
			_g_mx.lock();
			newtype->_id = _g_next_id++;
			if(_g_type_info.size() < _g_next_id)
				_g_type_info.resize(_g_next_id);
			_g_type_info[newtype->_id] = newtype;

			newtype->_parent = BitArray((newtype->_id >> 5) + 1);
			newtype->_kind_of = BitArray((newtype->_id >> 5) + 1);

			va_list ap;
			va_start(ap, parents);
			newtype->_kind_of.set(newtype->_id);
			for(size_t i = 0; i < parents; ++i)
			{
				TypeInfo* parent = (TypeInfo*)(va_arg(ap, void*));
				newtype->_parent.set(parent->_id);
				newtype->_kind_of.or(parent->_kind_of);
			}
			va_end(ap);

			_g_mx.unlock();

			newtype->_kind_of.recalc_onbit_count();

			return newtype;
		}
		TypeInfo*			TypeInfo::getRegTypeInfo(size_t index)
		{
			TypeInfo* typeinfo;
			_g_mx.lock();
			typeinfo = _g_type_info[index];
			_g_mx.unlock();
			return typeinfo;
		}
		size_t				TypeInfo::regTypeInfos()
		{
			size_t size;
			_g_mx.lock();
			size = _g_type_info.size();
			_g_mx.unlock();
			return size;
		}
		size_t				TypeInfo::parents() const
		{
			return _parent.onbit_count();
		}
		TypeInfo*			TypeInfo::firstParent()
		{
			int32_t pos = _parent.findfirst();
			if(pos >= 0)
				return getRegTypeInfo(pos);
			return NULL;
		}
		TypeInfo*			TypeInfo::nextParent()
		{
			int32_t pos = _parent.findnext();
			if(pos >= 0)
				return getRegTypeInfo(pos);
			return NULL;
		}
	};
};