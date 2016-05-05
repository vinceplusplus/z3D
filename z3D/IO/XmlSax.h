
#pragma once

#include "predecl.h"

#include "SaxBase.h"

namespace z3D
{
	namespace IO
	{
		class XmlSax: public SaxBase
		{
		private:
			ptrdiff_t				_export_xb;
			wcs						_filepath;
		private:
			ptrdiff_t				_inside_element_start;
		private:
			class EXPORT_ATTRIB
			{
			public:
				mbs						name;
				mbs						value;
				AttribGetter::TYPE		type;
			};
			class EXPORT_EVENT
			{
			public:
				size_t					type;						// 0 - start, 1 - end, 2 - cdata
				mbs						name;
				list<EXPORT_ATTRIB>		list_attrib;
				vector<char>			cdata;
			};

			list<EXPORT_EVENT>			_list_export_event;
			map<mbs, AttribGetter::TYPE>
										_map_export_attrib_type;
			EXPORT_EVENT				_export_element_start;
		private:
			static void				__expat_element_start(void* user_data, const char* name, const char ** array_attrib);
			static void				__expat_element_end(void* user_data, const char* name);
			static void				__expat_character_data(void* user_data, const char* s, int len);
		private:
			void					__element_start(const char* name, const char * const * array_attrib);
			void					__element_end(const char* name);
			void					__character_data(const char* s, size_t len);
		public:
			bool					parse(const char* buffer, size_t len, mbs& error_string);
			bool					parse(const wcs& filepath, mbs& error_string);
			void					getAttribs(const char * const * array_attrib, AttribGetter& getter);
		private:
			bool					__parse(const char* buffer, size_t len, mbs& error_string);
		};
	};
};

