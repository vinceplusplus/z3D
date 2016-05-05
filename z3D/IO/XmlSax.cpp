
#define _CRT_SECURE_NO_WARNINGS 1

#include "XmlSax.h"

#include "../Core/SMem.h"
#include "../Core/Vector2.h"
#include "../Core/Vector3.h"
#include "../Core/Vector4.h"
#include "../Core/Matrix2.h"
#include "../Core/Matrix3.h"
#include "../Core/Matrix4.h"
#include "../Core/Misc.h"

#include "../../expat/expat.h"

#include "FileSystem.h"

namespace z3D
{
	namespace IO
	{
		void					XmlSax::__expat_element_start(void* user_data, const char* name, const char ** array_attrib)
		{
			XmlSax* sax = ((XmlSax*)user_data);
			sax->__element_start(name, array_attrib);
		}
		void					XmlSax::__expat_element_end(void* user_data, const char* name)
		{
			XmlSax* sax = ((XmlSax*)user_data);
			sax->__element_end(name);
		}
		void					XmlSax::__expat_character_data(void* user_data, const char* s, int len)
		{
			XmlSax* sax = ((XmlSax*)user_data);
			sax->__character_data(s, (size_t)len);
		}
		void					XmlSax::__element_start(const char* name, const char * const * array_attrib)
		{
			if(_export_xb)
			{
				_inside_element_start = true;
				_export_element_start.type = 0;
				_export_element_start.name = name;

				_export_element_start.list_attrib.clear();
				for(const char * const * a = array_attrib; *a; a += 2)
				{
					EXPORT_ATTRIB attrib;
					attrib.name = a[0];
					attrib.value = a[1];
					attrib.type = AttribGetter::TYPE_CHAR_POINTER;

					_export_element_start.list_attrib.push_back(attrib);
				}

				_export_element_start.cdata.clear();

				_map_export_attrib_type.clear();
			}

			SaxBase::__element_start(name, array_attrib);

			if(_export_xb)
			{
				_inside_element_start = false;

				for(list<EXPORT_ATTRIB>::iterator iter = _export_element_start.list_attrib.begin(); iter != _export_element_start.list_attrib.end(); ++iter)
					if(_map_export_attrib_type.count(iter->name))
					{
						iter->type = _map_export_attrib_type[iter->name];
						if(iter->type == AttribGetter::TYPE_STRING || iter->type == AttribGetter::TYPE_WSTRING)
							iter->type = AttribGetter::TYPE_CHAR_POINTER;
					}

				_list_export_event.push_back(_export_element_start);
			}
		}
		void					XmlSax::__element_end(const char* name)
		{
			if(_export_xb)
			{
				EXPORT_EVENT e;
				e.type = 1;
				e.name = name;

				_list_export_event.push_back(e);
			}

			SaxBase::__element_end(name);
		}
		void					XmlSax::__character_data(const char* s, size_t len)
		{
			if(_export_xb)
			{
				EXPORT_EVENT e;
				e.type = 2;
				e.cdata.resize(len);
				if(len)
					memcpy(VECTOR_BEGIN_PTR(e.cdata), s, len);

				_list_export_event.push_back(e);
			}

			SaxBase::__character_data(s, len);
		}
		bool					XmlSax::parse(const char* buffer, size_t len, mbs& error_string)
		{
			_filepath = L"";
			_export_xb = false;

			return __parse(buffer, len, error_string);
		}
		bool					XmlSax::parse(const wcs& filepath, mbs& error_string)
		{
			SMem sm;
			if(!FileSystem::loadFromAll(filepath, sm))
				return false;

			_filepath = filepath;
			_export_xb = false;
			if(_export_xb && false)
			{
				_inside_element_start = false;
				_list_export_event.clear();
			}

			bool ret = __parse((const char*)sm.get(), sm.size(), error_string);

			sm.reset();

			if(_export_xb)
			{
				map<mbs, size_t> string_table;

				for(list<EXPORT_EVENT>::iterator iter = _list_export_event.begin(); iter != _list_export_event.end(); ++iter)
				{
					if(iter->type == 0)
					{
						if(!string_table.count(iter->name))
							string_table[iter->name] += 2;
						else
							string_table[iter->name] += 1;
						for(list<EXPORT_ATTRIB>::iterator iter2 = iter->list_attrib.begin(); iter2 != iter->list_attrib.end(); ++iter2)
						{
							if(!string_table.count(iter2->name))
								string_table[iter2->name] += 2;
							else
								string_table[iter2->name] += 1;
							if(iter2->type == AttribGetter::TYPE_CHAR_POINTER)
								string_table[iter2->value] += 1;
						}
					}
				}

				for(map<mbs, size_t>::iterator iter = string_table.begin(); iter != string_table.end();)
				{
					if(iter->second == 1)
					{
						iter = string_table.erase(iter);
						continue;
					}
					++iter;
				}
				multimap<size_t, mbs> string_table2;
				for(map<mbs, size_t>::iterator iter = string_table.begin(); iter != string_table.end(); ++iter)
					string_table2.insert(pair<size_t, mbs>(iter->second, iter->first));

				vector<mbs> string_table3;
				for(multimap<size_t, mbs>::iterator iter = string_table2.begin(); iter != string_table2.end(); ++iter)
					string_table3.push_back(iter->second);

				size_t string_table_size = 0;

				for(size_t i = 0; i < string_table3.size(); ++i)
				{
					string_table[string_table3[i]] = string_table_size;

					string_table_size += string_table3[i].length() + 1;
				}

				class lenwrite
				{
				public:
					static void write(size_t len, FILE* fp)
					{
						Z_ASSERT( len < 268435456 );
						size_t s;

						if(len < 128)
							fwrite(&len, 1, 1, fp);
						else if(len < 16384)
						{
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f);
							fwrite(&s, 1, 1, fp);
						}
						else if(len < 2097152)
						{
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f);
							fwrite(&s, 1, 1, fp);
						}
						else
						{
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f) | 0x80;
							fwrite(&s, 1, 1, fp);

							len >>= 7;
							s = (len & 0x7f);
							fwrite(&s, 1, 1, fp);
						}
					}
				};

				FILE* fp = NULL;
				_wfopen_s(&fp, (filepath + L".bx").c_str(), L"wb");
				if(fp)
				{
					fwrite("BX  ", 4, 1, fp);

					lenwrite::write(string_table_size, fp);

					for(size_t i = 0; i < string_table3.size(); ++i)
						fwrite(string_table3[i].c_str(), string_table3[i].length() + 1, 1, fp);

					for(list<EXPORT_EVENT>::iterator iter = _list_export_event.begin(); iter != _list_export_event.end(); ++iter)
					{
						if(iter->type == 0)
						{
							if(iter->list_attrib.size() == 0)
								fputc(0, fp);
							else if(iter->list_attrib.size() < 256)
								fputc(1, fp);
							else if(iter->list_attrib.size() < 65536)
								fputc(2, fp);
							else
								fputc(3, fp);

							lenwrite::write(string_table.find(iter->name)->second, fp);

							if(iter->list_attrib.size())
							{
								if(iter->list_attrib.size() < 256)
								{
									uint8_t attribs = (uint8_t)iter->list_attrib.size();
									fwrite(&attribs, sizeof(attribs), 1, fp);
								}
								else if(iter->list_attrib.size() < 65536)
								{
									uint16_t attribs = (uint16_t)iter->list_attrib.size();
									fwrite(&attribs, sizeof(attribs), 1, fp);
								}
								else
								{
									uint32_t attribs = (uint32_t)iter->list_attrib.size();
									fwrite(&attribs, sizeof(attribs), 1, fp);
								}
							}
							for(list<EXPORT_ATTRIB>::iterator iter2 = iter->list_attrib.begin(); iter2 != iter->list_attrib.end(); ++iter2)
							{
								lenwrite::write(string_table.find(iter2->name)->second, fp);

								if(iter2->type == AttribGetter::TYPE_CHAR_POINTER)
								{
									if(string_table.count(iter2->value))
									{
										fwrite(&iter2->type, 1, 1, fp);
										lenwrite::write(string_table.find(iter2->value)->second, fp);
									}
									else
									{
										AttribGetter::TYPE type = AttribGetter::TYPE_STRING;
										fwrite(&type, 1, 1, fp);
										lenwrite::write(iter2->value.length() + 1, fp);
										fwrite(iter2->value.c_str(), iter2->value.length() + 1, 1, fp);
									}
								}
								else
								{
									fwrite(&iter2->type, 1, 1, fp);
									int ret;

									switch(iter2->type)
									{
										case AttribGetter::TYPE_BOOL:
											{
												lenwrite::write(sizeof(bool) + 1, fp);
												bool v = !!strtoul(iter2->value.c_str(), NULL, 10);
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_INT32:
											{
												lenwrite::write(sizeof(int32_t) + 1, fp);
												int32_t v = (int32_t)strtoul(iter2->value.c_str(), NULL, 10);
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_INT64:
											{
												lenwrite::write(sizeof(int64_t) + 1, fp);
												int64_t v = (int64_t)_strtoui64(iter2->value.c_str(), NULL, 10);
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_FLOAT:
											{
												lenwrite::write(sizeof(float) + 1, fp);
												float v = (float)strtod(iter2->value.c_str(), NULL);
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_DOUBLE:
											{
												lenwrite::write(sizeof(double) + 1, fp);
												double v = (double)strtod(iter2->value.c_str(), NULL);
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_VEC2:
											{
												lenwrite::write(sizeof(Vec2) + 1, fp);
												Vec2 v;
												ret = sscanf(iter2->value.c_str(), "%f %f", &v.x, &v.y);
												Z_ASSERT( ret == 2 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_VEC3:
											{
												lenwrite::write(sizeof(Vec3) + 1, fp);
												Vec3 v;
												ret = sscanf(iter2->value.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
												Z_ASSERT( ret == 3 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_VEC4:
										case AttribGetter::TYPE_QUAT:
											{
												lenwrite::write(sizeof(Vec4) + 1, fp);
												Vec4 v;
												ret = sscanf(iter2->value.c_str(), "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
												Z_ASSERT( ret == 4 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_MAT2:
											{
												lenwrite::write(sizeof(Mat2) + 1, fp);
												Mat2 v;
												ret = sscanf(iter2->value.c_str(), "%f %f %f %f", (REAL*)&v, (REAL*)&v + 1, (REAL*)&v + 2, (REAL*)&v + 3);
												Z_ASSERT( ret == 4 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_MAT3:
											{
												lenwrite::write(sizeof(Mat3) + 1, fp);
												Mat3 v;
												ret = sscanf(iter2->value.c_str(), "%f %f %f %f %f %f %f %f %f", (REAL*)&v, (REAL*)&v + 1, (REAL*)&v + 2, (REAL*)&v + 3, (REAL*)&v + 4, (REAL*)&v + 5, (REAL*)&v + 6, (REAL*)&v + 7, (REAL*)&v + 8);
												Z_ASSERT( ret == 9 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
										case AttribGetter::TYPE_MAT4:
											{
												lenwrite::write(sizeof(Mat4) + 1, fp);
												Mat4 v;
												ret = sscanf(iter2->value.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", (REAL*)&v, (REAL*)&v + 1, (REAL*)&v + 2, (REAL*)&v + 3, (REAL*)&v + 4, (REAL*)&v + 5, (REAL*)&v + 6, (REAL*)&v + 7, (REAL*)&v + 8, (REAL*)&v + 9, (REAL*)&v + 10, (REAL*)&v + 11, (REAL*)&v + 12, (REAL*)&v + 13, (REAL*)&v + 14, (REAL*)&v + 15);
												Z_ASSERT( ret == 16 );
												fwrite(&v, sizeof(v), 1, fp);
												fputc(0, fp);
											}
											break;
									}
								}
							}
						}
						else if(iter->type == 1)
						{
							fputc(4, fp);
						}
						else if(iter->type == 2)
						{
							if(iter->cdata.size())
							{
								if(iter->cdata.size() < 256)
								{
									fputc(5, fp);
									uint8_t len = (uint8_t)iter->cdata.size();
									fwrite(&len, sizeof(len), 1, fp);
								}
								else if(iter->cdata.size() < 65536)
								{
									fputc(6, fp);
									uint16_t len = (uint16_t)iter->cdata.size();
									fwrite(&len, sizeof(len), 1, fp);
								}
								else
								{
									fputc(7, fp);
									uint32_t len = (uint32_t)iter->cdata.size();
									fwrite(&len, sizeof(len), 1, fp);
								}
								fwrite(VECTOR_BEGIN_PTR(iter->cdata), iter->cdata.size(), 1, fp);
							}
						}
					}

					fclose(fp);
				}
			}

			return ret;
		}
		void					XmlSax::getAttribs(const char * const * array_attrib, AttribGetter& getter)
		{
			Z_STATIC_ASSERT( sizeof(float) == sizeof(REAL), ASSUMPTION_WRONG );

			if(_export_xb && _inside_element_start && _array_attrib == array_attrib)
			{
				// getter now has some kind of type information about attribs, so we can transform those attribs into their native form
				for(size_t i = 0; i < getter.entries; ++i)
					_map_export_attrib_type[getter.array_entry[i].name] = (AttribGetter::TYPE)getter.array_entry[i].type;
			}

			const char * const * attrib = array_attrib;

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

						e.got = true;

						while(search_start <= search_end && getter.array_entry[search_start].got)
							++search_start;
						while(search_end >= search_start && getter.array_entry[search_end].got)
							--search_end;

						break;
					}
				}

				attrib += 2;
			}
		}
		bool					XmlSax::__parse(const char* buffer, size_t len, mbs& error_string)
		{
			XML_Parser parser = XML_ParserCreate(NULL);

			XML_SetStartElementHandler(parser, &XmlSax::__expat_element_start);
			XML_SetEndElementHandler(parser, &XmlSax::__expat_element_end);
			XML_SetCharacterDataHandler(parser, &XmlSax::__expat_character_data);

			XML_SetUserData(parser, this);

			XML_Status status = XML_Parse(parser, buffer, (int)len, true);
			XML_Error error;
			if(status != XML_STATUS_OK)
				error = XML_GetErrorCode(parser);
			XML_ParserFree(parser);

			if(status == XML_STATUS_OK)
				return true;

			error_string = XML_ErrorString(error);
			return false;
		}
	};
};

