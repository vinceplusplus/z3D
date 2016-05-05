
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "Logger.h"

#include "../aligned_storage_inl.h"

#include "../scoped_lock.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef min
#undef max

#include <time.h>

namespace z3D
{
	namespace Core
	{
#include "../z_off_debug_new.h"
		Logger::Logger(const wchar_t* prefix)
			: _mx(new (_mx.allocate()) mutex(), aligned_storage_tag::ctor())
		{
			Z_STATIC_ASSERT(sizeof(_last_datetime) >= sizeof(__time64_t), __TIME64_T_LARGER_THAN_64_BIT);

			memcpy(_prefix, prefix, min(wcslen(prefix) + 1, _countof(_prefix)) * sizeof(wchar_t));
			_prefix[_countof(_prefix) - 1] = 0;

			_add_datetime = true;
			_last_datetime = _time64(NULL);

			_buffer_limit = 5;

			_buffer_available_start = 0;
			_lines = 0;

			_ext_logger = NULL;
		}
#include "../z_on_debug_new.h"
		Logger::~Logger()
		{
			flush();

			//_mx.destroy<mutex>();
		}
		bool				Logger::getAddDateTime()
		{
			return !!_add_datetime;
		}
		void				Logger::setAddDateTime(bool add)
		{
			_add_datetime = add;
		}
		size_t				Logger::getBufferLimit()
		{
			return _buffer_limit;
		}
		void				Logger::setBufferLimit(size_t limit)
		{
			_buffer_limit = limit;
		}
		void				Logger::flush()
		{
			scoped_lock<mutex> o(_mx.get());

			if(_lines)
			{
				tm t = *_localtime64((const __time64_t*)&_last_datetime);
				wchar_t filename[128];
				swprintf(filename, L"%s%04d-%02d-%02d.txt", _prefix, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

				FILE* fp = _wfopen(filename, L"r");
				if(!fp)
				{
					fp = _wfopen(filename, L"ab");
					if(!fp)
						return;
					wchar_t UNICODE_INDICATOR = 0xfeff;
					fwrite(&UNICODE_INDICATOR, sizeof(UNICODE_INDICATOR), 1, fp);
				}
				else
				{
					fclose(fp);
					fp = _wfopen(filename, L"ab");
					if(!fp)
						return;
				}
				const wchar_t* line = _buffer;
				for(size_t i = 0; i < _lines; ++i)
				{
					size_t s = wcslen(line);
					fwrite(line, s * sizeof(wchar_t), 1, fp);
					fwrite(L"\r\n", 2 * sizeof(wchar_t), 1, fp);

					line += s + 1;
				}
				fclose(fp);

				_lines = 0;
				_buffer_available_start = 0;
			}
		}
		Logger&			Logger::operator<<(const wchar_t* line)
		{
			scoped_lock<mutex> o(_mx.get());
			__time64_t cur = _time64(NULL);
			tm t1 = *_localtime64((const __time64_t*)&_last_datetime);
			tm t2 = *_localtime64(&cur);

			if(t1.tm_year != t2.tm_year || t1.tm_mon != t2.tm_mon || t1.tm_mday != t2.tm_mday)
				flush();
			_last_datetime = cur;
			addLine(line);

			return *this;
		}
		Logger&			Logger::operator<<(const char* line)
		{
			scoped_lock<mutex> o(_mx.get());
			_temp_buffer[max(0, MultiByteToWideChar(CP_ACP, 0, line, -1, _temp_buffer, _countof(_temp_buffer) - 1))] = 0;
			this->operator << (_temp_buffer);

			return *this;
		}
		void				Logger::addLine(const wchar_t* line)
		{
			wchar_t time_buffer[128];
			size_t time_len;
			if(_add_datetime)
			{
				__time64_t tt = _time64(NULL);
				tm t = *_localtime64(&tt);
				swprintf(time_buffer, L"%02d:%02d:%02d %09d ", t.tm_hour, t.tm_min, t.tm_sec, GetCurrentThreadId());

				time_len = wcslen(time_buffer);
			}
			else
				time_len = 0;

			size_t line_len = wcslen(line);
			line_len = min(line_len, _countof(_buffer) - time_len - 1);

			if(_countof(_buffer) - _buffer_available_start < time_len + line_len + 1)
				flush();

			memcpy(_buffer + _buffer_available_start, time_buffer, time_len * sizeof(wchar_t));
			_buffer_available_start += time_len;
			memcpy(_buffer + _buffer_available_start,line, line_len * sizeof(wchar_t));
			_buffer[_buffer_available_start + line_len] = 0;
			_buffer_available_start += line_len + 1;
			++_lines;

			if(_ext_logger)
				_ext_logger->addLine(line);
		}
		void				Logger::setExtLogger(ExtLogger* extlogger)
		{
			_ext_logger = extlogger;
		}
		ExtLogger*		Logger::getExtLogger() const
		{
			return _ext_logger;
		}
	};
};

