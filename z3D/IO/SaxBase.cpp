
#define _CRT_SECURE_NO_WARNINGS 1

#include "SaxBase.h"

#include "../Core/functor_inl.h"

namespace z3D
{
	namespace IO
	{
		SaxBase::AttribGetter::AttribGetter()
		{
			clear();
		}
		void					SaxBase::AttribGetter::clear()
		{
			name_buffer_pos = 0;
			entries = 0;
		}
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(const char** addr)			{ return TYPE_CHAR_POINTER; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(mbs* addr)					{ return TYPE_STRING; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(wcs* addr)					{ return TYPE_WSTRING; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(bool* addr)					{ return TYPE_BOOL; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(int32_t* addr)				{ return TYPE_INT32; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(uint32_t* addr)				{ return TYPE_INT32; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(int64_t* addr)				{ return TYPE_INT64; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(uint64_t* addr)				{ return TYPE_INT64; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(float* addr)					{ return TYPE_FLOAT; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(double* addr)				{ return TYPE_DOUBLE; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Vec2* addr)		{ return TYPE_VEC2; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Vec3* addr)		{ return TYPE_VEC3; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Vec4* addr)		{ return TYPE_VEC4; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Quat* addr)		{ return TYPE_QUAT; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Mat2* addr)		{ return TYPE_MAT2; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Mat3* addr)		{ return TYPE_MAT3; }
		SaxBase::AttribGetter::TYPE			SaxBase::AttribGetter::getType(z3D::Core::Mat4* addr)		{ return TYPE_MAT4; }
		ptrdiff_t				SaxBase::AttribGetter::get(const char* name, const char** addr, bool copy_name)
		{
			return get<const char*>(name, addr, copy_name);
		}
		ptrdiff_t				SaxBase::AttribGetter::__get(const char* name, void* addr, TYPE type, bool copy_name)
		{
			_ENTRY e;
			if(copy_name)
			{
				e.name = name_buffer + name_buffer_pos;
				size_t len = strlen(name) + 1;

				Z_ASSERT( name_buffer_pos + len <= NAME_BUFFER_SIZE );

				strncpy((char*)e.name, name, len);
				name_buffer_pos += len;
			}
			else
				e.name = name;
			e.value_addr = addr;
			e.type = (uint16_t)type;
			e.got = false;

			Z_ASSERT( entries < MAX_ENTRIES );

			array_entry[entries] = e;

			ptrdiff_t index = entries;

			++entries;

			return index;
		}
		bool					SaxBase::AttribGetter::got(ptrdiff_t index) const
		{
			Z_ASSERT( index >= 0 && index < (ptrdiff_t)entries );
			return !!array_entry[index].got;
		}
		SaxBase::SaxBase()
		{
			reset();
		}
		SaxBase::~SaxBase() {}
		void					SaxBase::reset()
		{
			_level = 0;
			_event = EVENT_NONE;
			_name = NULL;
			_array_attrib = NULL;
			_cdata = NULL;
			_cdata_len = 0;

			_size_stack_dele_restore = 0;

			_latest_dele_restore_level = 0;
		}
		void					SaxBase::setUserPointer(const SPtr<RefBase>& user_pointer)
		{
			_user_pointer = user_pointer;
		}
		const SPtr<RefBase>&	SaxBase::getUserPointer() const
		{
			return _user_pointer;
		}
		void					SaxBase::setElementStartHandler(const ELEMENT_START_HANDLER& func)
		{
			Z_ASSERT( !_level );
			_func_element_start = func;
		}
		void					SaxBase::setElementEndHandler(const ELEMENT_END_HANDLER& func)
		{
			Z_ASSERT( !_level );
			_func_element_end = func;
		}
		void					SaxBase::setCharacterDataHandler(const CHARACTER_DATA_HANDLER& func)
		{
			Z_ASSERT( !_level );
			_func_character_data = func;
		}
		void					SaxBase::__element_start(const char* name, const char * const * array_attrib)
		{
			++_level;
			_event = EVENT_ELEMENT_START;
			_name = name;
			_array_attrib = array_attrib;

			if(_func_element_start.valid())
				_func_element_start(this, name, array_attrib);
		}
		void					SaxBase::__element_end(const char* name)
		{
			_event = EVENT_ELEMENT_END;
			_name = name;

			if(_func_element_end.valid())
				_func_element_end(this, name);

			if(_size_stack_dele_restore && _latest_dele_restore_level == _level)
			{
				const _DELE_RESTORE& restore = _stack_dele_restore[_size_stack_dele_restore - 1];
				if(restore.level == _level)
				{
					_func_element_start = restore.start_handler;
					_func_element_end = restore.end_handler;
					_func_character_data = restore.cdata_handler;

					--_size_stack_dele_restore;

					_latest_dele_restore_level = _size_stack_dele_restore ? _stack_dele_restore[_size_stack_dele_restore - 1].level: 0;
				}
			}

			--_level;
		}
		void					SaxBase::__character_data(const char* s, size_t len)
		{
			_event = EVENT_CHARACTER_DATA;
			_cdata = s;
			_cdata_len = len;

			if(_func_character_data.valid())
				_func_character_data(this, s, len);
		}
		void					SaxBase::passOnTo(const ELEMENT_START_HANDLER& start_handler)
		{
			passOnTo(start_handler, ELEMENT_END_HANDLER(), CHARACTER_DATA_HANDLER());
		}
		void					SaxBase::passOnTo(const ELEMENT_START_HANDLER& start_handler, const ELEMENT_END_HANDLER& end_handler)
		{
			passOnTo(start_handler, end_handler, CHARACTER_DATA_HANDLER());
		}
		void					SaxBase::passOnTo(const ELEMENT_START_HANDLER& start_handler, const ELEMENT_END_HANDLER& end_handler, const CHARACTER_DATA_HANDLER& cdata_handler)
		{
			Z_ASSERT( _event == EVENT_ELEMENT_START && _size_stack_dele_restore < MAX_DELE_RESTORE_STACK_SIZE );

			_DELE_RESTORE restore = {_level, _func_element_start, _func_element_end, _func_character_data};
			_stack_dele_restore[_size_stack_dele_restore] = restore;
			++_size_stack_dele_restore;

			_latest_dele_restore_level = _level;

			_func_element_start = start_handler;
			_func_element_end = end_handler;
			_func_character_data = cdata_handler;

			if(_func_element_start.valid())
				_func_element_start(this, _name, _array_attrib);
		}
	};
};

