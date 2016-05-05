
#define _CRT_SECURE_NO_WARNINGS 1

#include "BxSax.h"

#include "../Core/SMem.h"
#include "../Core/Misc.h"

#include "FileSystem.h"
#include "MemReader.h"

namespace z3D
{
	namespace IO
	{
		bool					BxSax::parse(const char* buffer, size_t len, mbs& error_string)
		{
			class lenread
			{
			public:
				static size_t		read(MemReader& reader)
				{
					size_t len = 0;
					uint8_t c;
					c = reader.read<uint8_t>();
					len |= ((size_t)c) & 0x7f;
					if(c & 0x80)
					{
						c = reader.read<uint8_t>();
						len |= (((size_t)c) & 0x7f) << 7;
						if(c & 0x80)
						{
							c = reader.read<uint8_t>();
							len |= (((size_t)c) & 0x7f) << 14;
							if(c & 0x80)
							{
								c = reader.read<uint8_t>();
								len |= (((size_t)c) & 0x7f) << 21;
							}
						}
					}
					return len;
				}
			};

			MemReader reader(buffer, len);

			if(true)
			{
				char ID[4] = {0, 0, 0, 0};
				reader.read(ID, 4);
				if(memcmp(ID, "BX  ", 4))
					return false;
			}

			size_t string_table_size = lenread::read(reader);

			const char* string_table = (const char*)reader.buffer() + reader.offset();

			reader.seek(reader.offset() + string_table_size);

			const char* element_name_stack[1024];
			size_t element_name_stack_size = 0;

			uint8_t event_type;

			while(reader.available())
			{
				event_type = reader.read<uint8_t>();

				switch(event_type)
				{
					// event of element start
				case 0:
				case 1:
				case 2:
				case 3:
					{
						const char* name = string_table + lenread::read(reader);

						size_t attribs;
						switch(event_type)
						{
						case 0:		attribs = 0;								break;
						case 1:		attribs = reader.read<uint8_t>();			break;
						case 2:		attribs = reader.read<uint16_t>();			break;
						case 3:		attribs = reader.read<uint32_t>();			break;
						default:	attribs = 0;								break;
						}

						const char* array_attrib[2048];
						const char** a = array_attrib;

						AttribGetter::TYPE array_type[1024];
						size_t array_len[1024];

						for(size_t i = 0; i < attribs; ++i)
						{
							a[0] = string_table + lenread::read(reader);
							AttribGetter::TYPE attrib_type = (AttribGetter::TYPE)reader.read<uint8_t>();
							array_type[i] = attrib_type;
							if(attrib_type == AttribGetter::TYPE_CHAR_POINTER)
								a[1] = string_table + lenread::read(reader);
							else
							{
								size_t len = lenread::read(reader);
								array_len[i] = len;
								a[1] = (const char*)reader.buffer() + reader.offset();
								reader.seek(reader.offset() + len);
							}
							a += 2;
						}
						*a = 0;

						element_name_stack[element_name_stack_size] = name;
						++element_name_stack_size;

						_array_attrib = array_attrib;
						_array_attrib_type = array_type;
						_array_attrib_len = array_len;

						SaxBase::__element_start(name, array_attrib);
					}
					break;
					// event of element end
				case 4:
					{
						--element_name_stack_size;

						Z_ASSERT( (ptrdiff_t)element_name_stack_size >= 0 );

						SaxBase::__element_end(element_name_stack[element_name_stack_size]);
					}
					break;
					// event of character data
				case 5:
					{
						size_t len = reader.read<uint8_t>();
						const char* cdata = (const char*)reader.buffer() + reader.offset();
						reader.seek(reader.offset() + len);

						SaxBase::__character_data(cdata, len);
					}
					break;
				case 6:
					{
						size_t len = reader.read<uint16_t>();
						const char* cdata = (const char*)reader.buffer() + reader.offset();
						reader.seek(reader.offset() + len);

						SaxBase::__character_data(cdata, len);
					}
					break;
				case 7:
					{
						size_t len = reader.read<uint32_t>();
						const char* cdata = (const char*)reader.buffer() + reader.offset();
						reader.seek(reader.offset() + len);

						SaxBase::__character_data(cdata, len);
					}
					break;
				}
			}

			return true;
		}
		bool					BxSax::parse(const wcs& filepath, mbs& error_string)
		{
			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return false;

			bool ret = parse((const char*)sm.get(), sm.size(), error_string);

			return ret;
		}
		void					BxSax::getAttribs(const char * const * array_attrib, AttribGetter& getter)
		{
			Z_STATIC_ASSERT( sizeof(float) == sizeof(REAL), ASSUMPTION_WRONG );

			Z_ASSERT( _array_attrib == array_attrib );

			const char * const * attrib = _array_attrib;
			const AttribGetter::TYPE* attrib_type = _array_attrib_type;
			const size_t* attrib_len = _array_attrib_len;

			ptrdiff_t search_start = 0;
			ptrdiff_t search_end = getter.entries - 1;

			while(*attrib)
			{
				if(search_end < search_start)
					break;

				const char * const n = attrib[0];
				const char * const v = attrib[1];

				for(ptrdiff_t i = search_start; i <= search_end; ++i)
				{
					AttribGetter::_ENTRY& e = getter.array_entry[i];
					if(strcmp(e.name, n) == 0)
					{
						if(*attrib_type == AttribGetter::TYPE_CHAR_POINTER || *attrib_type == AttribGetter::TYPE_STRING)
						{
							int ret;
							switch(e.type)
							{
							case AttribGetter::TYPE_CHAR_POINTER:
								*(const char **)e.value_addr = v;
								break;
							case AttribGetter::TYPE_STRING:
								*(mbs*)e.value_addr = v;
								break;
							case AttribGetter::TYPE_WSTRING:
								*(wcs*)e.value_addr = UTF8TOWCS(v);
								break;
							case AttribGetter::TYPE_BOOL:
								*(bool*)e.value_addr = !!strtoul(v, NULL, 10);
								break;
							case AttribGetter::TYPE_INT32:
								*(int32_t*)e.value_addr = (int32_t)strtoul(v, NULL, 10);
								break;
							case AttribGetter::TYPE_INT64:
								*(int64_t*)e.value_addr = (int64_t)_strtoui64(v, NULL, 10);
								break;
							case AttribGetter::TYPE_FLOAT:
								*(float*)e.value_addr = (float)strtod(v, NULL);
								break;
							case AttribGetter::TYPE_DOUBLE:
								*(double*)e.value_addr = (double)strtod(v, NULL);
								break;
							case AttribGetter::TYPE_VEC2:
								ret = sscanf(v, "%f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1);
								Z_ASSERT( ret == 2 );
								break;
							case AttribGetter::TYPE_VEC3:
								ret = sscanf(v, "%f %f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1, (REAL*)e.value_addr + 2);
								Z_ASSERT( ret == 3 );
								break;
							case AttribGetter::TYPE_VEC4:
							case AttribGetter::TYPE_QUAT:
								ret = sscanf(v, "%f %f %f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1, (REAL*)e.value_addr + 2, (REAL*)e.value_addr + 3);
								Z_ASSERT( ret == 4 );
								break;
							case AttribGetter::TYPE_MAT2:
								ret = sscanf(v, "%f %f %f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1, (REAL*)e.value_addr + 2, (REAL*)e.value_addr + 3);
								Z_ASSERT( ret == 4 );
								break;
							case AttribGetter::TYPE_MAT3:
								ret = sscanf(v, "%f %f %f %f %f %f %f %f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1, (REAL*)e.value_addr + 2, (REAL*)e.value_addr + 3, (REAL*)e.value_addr + 4, (REAL*)e.value_addr + 5, (REAL*)e.value_addr + 6, (REAL*)e.value_addr + 7, (REAL*)e.value_addr + 8);
								Z_ASSERT( ret == 9 );
								break;
							case AttribGetter::TYPE_MAT4:
								ret = sscanf(v, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", (REAL*)e.value_addr, (REAL*)e.value_addr + 1, (REAL*)e.value_addr + 2, (REAL*)e.value_addr + 3, (REAL*)e.value_addr + 4, (REAL*)e.value_addr + 5, (REAL*)e.value_addr + 6, (REAL*)e.value_addr + 7, (REAL*)e.value_addr + 8, (REAL*)e.value_addr + 9, (REAL*)e.value_addr + 10, (REAL*)e.value_addr + 11, (REAL*)e.value_addr + 12, (REAL*)e.value_addr + 13, (REAL*)e.value_addr + 14, (REAL*)e.value_addr + 15);
								Z_ASSERT( ret == 16 );
								break;
							}
						}
						else
						{
							Z_ASSERT( *attrib_type == e.type );
							if(*attrib_type == e.type)
							{
								memcpy(e.value_addr, v, *attrib_len - 1);
							}
						}

						e.got = true;

						while(search_start <= search_end && getter.array_entry[search_start].got)
							++search_start;
						while(search_end >= search_start && getter.array_entry[search_end].got)
							--search_end;

						break;
					}
				}

				attrib += 2;
				attrib_type += 1;
				attrib_len += 1;
			}
		}
	};
};

