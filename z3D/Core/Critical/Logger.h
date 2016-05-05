
#pragma once

#include "predecl.h"

#include "../aligned_storage.h"
#include "../mutex.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace z3D
{
	namespace Core
	{
		class ExtLogger
		{
		public:
			virtual ~ExtLogger() {}
		public:
			virtual void		addLine(const wchar_t* line) = 0;
		};

		class __Z3DCORECRIT_API Logger
		{
		private:
			enum
			{
				BUFFER_MAX_LEN = 16384,
				TEMP_BUFFER_MAX_LEN = 16384,
			};
		private:
			wchar_t						_prefix[32];
			size_t						_add_datetime;
			uint64_t					_last_datetime;
			size_t						_buffer_limit;
			wchar_t						_buffer[BUFFER_MAX_LEN];
			size_t						_buffer_available_start;
			size_t						_lines;
			wchar_t						_temp_buffer[TEMP_BUFFER_MAX_LEN];
			typed_aligned_storage<mutex, 24, 4, true>
										_mx;
			ExtLogger*					_ext_logger;
		public:
			Logger(const wchar_t* prefix = L"log");
			virtual ~Logger();
		public:
			bool				getAddDateTime();
			void				setAddDateTime(bool add);
			size_t				getBufferLimit();
			void				setBufferLimit(unsigned int limit);
			void				flush();
			Logger&				operator<<(const wchar_t* line);
			Logger&				operator<<(const char* line);
		private:
			void				addLine(const wchar_t* line);
		public:
			void				setExtLogger(ExtLogger* extlogger);
			ExtLogger*			getExtLogger() const;
		};

		__Z3DCORECRIT_API extern Logger syslog_basic;
	};
};

#pragma warning(pop)