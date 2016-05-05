
#pragma once

#include "predecl.h"

#include "../Core/fast_string.h"
#include "../Core/functor.h"
#include "../Core/ResBase.h"

namespace z3D
{
	namespace IO
	{
		class SaxBase
		{
		public:
			class AttribGetter
			{
			public:
				enum
				{
					NAME_BUFFER_SIZE = 4096,
					MAX_ENTRIES = 256,
				};
				typedef enum _TYPE: size_t
				{
					TYPE_CHAR_POINTER = 0,
					TYPE_STRING,
					TYPE_WSTRING,
					TYPE_BOOL,
					TYPE_INT32,
					TYPE_INT64,
					TYPE_FLOAT,
					TYPE_DOUBLE,
					TYPE_VEC2,
					TYPE_VEC3,
					TYPE_VEC4,
					TYPE_QUAT,
					TYPE_MAT2,
					TYPE_MAT3,
					TYPE_MAT4,
				} TYPE;
			public:
				char					name_buffer[ NAME_BUFFER_SIZE ];
				size_t					name_buffer_pos;
			public:
				struct _ENTRY
				{
					const char*				name;
					void*					value_addr;
					union
					{
						size_t					__UNNAMED;
						struct
						{
							uint16_t			type;
							uint16_t			got;
						};
					};
				}						array_entry[ MAX_ENTRIES ];
				size_t					entries;
			public:
				AttribGetter();
			public:
				void					clear();
			private:
				TYPE					getType(const char** addr);
				TYPE					getType(mbs* addr);
				TYPE					getType(wcs* addr);
				TYPE					getType(bool* addr);
				TYPE					getType(int32_t* addr);
				TYPE					getType(uint32_t* addr);
				TYPE					getType(int64_t* addr);
				TYPE					getType(uint64_t* addr);
				TYPE					getType(float* addr);
				TYPE					getType(double* addr);
				TYPE					getType(z3D::Core::Vec2* addr);
				TYPE					getType(z3D::Core::Vec3* addr);
				TYPE					getType(z3D::Core::Vec4* addr);
				TYPE					getType(z3D::Core::Quat* addr);
				TYPE					getType(z3D::Core::Mat2* addr);
				TYPE					getType(z3D::Core::Mat3* addr);
				TYPE					getType(z3D::Core::Mat4* addr);
				template<class T>
				TYPE					getType(T* addr)					{ Z_STATIC_ASSERT( false, WRONG_TYPE ); return -1; }
			private:
				ptrdiff_t				__get(const char* name, void* addr, TYPE type, bool copy_name);
			public:
				ptrdiff_t				get(const char* name, const char** addr, bool copy_name);
				template<class T>
				ptrdiff_t				get(const char* name, T* addr, bool copy_name) { return __get(name, addr, getType(addr), copy_name); }
				bool					got(ptrdiff_t index) const;
			};
		public:
			enum
			{
				MAX_DELE_RESTORE_STACK_SIZE = 64,
			};
			typedef enum _EVENT: size_t
			{
				EVENT_NONE = 0,
				EVENT_ELEMENT_START,
				EVENT_ELEMENT_END,
				EVENT_CHARACTER_DATA,
			} EVENT;
		public:
			typedef functor3<void, SaxBase*, const char*, const char * const *>
									ELEMENT_START_HANDLER;
			typedef functor2<void, SaxBase*, const char*>
									ELEMENT_END_HANDLER;
			typedef functor3<void, SaxBase*, const char*, size_t>
									CHARACTER_DATA_HANDLER;
		private:
			SPtr<RefBase>			_user_pointer;
		private:
			ELEMENT_START_HANDLER	_func_element_start;
			ELEMENT_END_HANDLER		_func_element_end;
			CHARACTER_DATA_HANDLER	_func_character_data;
		protected:
			size_t					_level;
			EVENT					_event;
			const char*				_name;
			const char * const *	_array_attrib;
			const char*				_cdata;
			size_t					_cdata_len;
		private:
			struct _DELE_RESTORE
			{
				size_t					level;
				ELEMENT_START_HANDLER	start_handler;
				ELEMENT_END_HANDLER		end_handler;
				CHARACTER_DATA_HANDLER	cdata_handler;

			}						_stack_dele_restore[ MAX_DELE_RESTORE_STACK_SIZE ];
			size_t					_size_stack_dele_restore;
			size_t					_latest_dele_restore_level;
		public:
			SaxBase();
			virtual					~SaxBase();
		public:
			void					reset();
		public:
			void					setUserPointer(const SPtr<RefBase>& user_pointer);
			const SPtr<RefBase>&	getUserPointer() const;
			void					setElementStartHandler(const ELEMENT_START_HANDLER& func);
			void					setElementEndHandler(const ELEMENT_END_HANDLER& func);
			void					setCharacterDataHandler(const CHARACTER_DATA_HANDLER& func);
		protected:
			void					__element_start(const char* name, const char * const * array_attrib);
			void					__element_end(const char* name);
			void					__character_data(const char* s, size_t len);
		public:
			virtual bool			parse(const char* buffer, size_t len, mbs& error_string) = 0;
			virtual bool			parse(const wcs& filepath, mbs& error_string) = 0;
			virtual void			getAttribs(const char * const * array_attrib, AttribGetter& getter) = 0;
		public:
			void					passOnTo(const ELEMENT_START_HANDLER& start_handler);
			void					passOnTo(const ELEMENT_START_HANDLER& start_handler, const ELEMENT_END_HANDLER& end_handler);
			void					passOnTo(const ELEMENT_START_HANDLER& start_handler, const ELEMENT_END_HANDLER& end_handler, const CHARACTER_DATA_HANDLER& cdata_handler);
		};
	};
};

