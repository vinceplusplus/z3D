
#pragma once

#include "predecl.h"

#include <wininet.h>
#pragma comment(lib,"wininet.lib")

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

namespace z3D
{
	namespace Core
	{
		class inet_url_parts
		{
		public:
			wstring			scheme_str;
			int				scheme;
			wstring			hostname;
			int				port;
			wstring			username;
			wstring			password;
			wstring			url_path;
			wstring			extra_info;
		};

		class inet_reader
		{
		public:
			__int64					_bytes_to_read;
			__int64					_bytes_read;
		public:
									inet_reader(__int64 bytes_to_read);
			virtual					~inet_reader();
		public:
			virtual void			run(HINTERNET handle);
			virtual bool			read(char* buffer,long len) = 0;			// returns true to continue
			virtual void			on_stop();
		public:
			bool					complete() const;
		};

		class inet_reader_string: public inet_reader
		{
		public:
			string					str;
		public:
									inet_reader_string(__int64 bytes_to_read);
		public:
			bool					read(char* buffer,long len);
		};

		class inet_reader_buffered: public inet_reader
		{
		public:
			vector<char>			buffer;
		public:
									inet_reader_buffered(__int64 bytes_to_read);
		public:
			bool					read(char* buffer,long len);
		};

		class inet_reader_file: public inet_reader
		{
		protected:
			FILE*			_handle_file;
			wstring			_filepath;
			bool			_append;
		public:
			inet_reader_file(const wstring& filepath,bool append,__int64 bytes_to_read);
			~inet_reader_file();
		public:
			bool				read(char* buffer,long len);
			void				on_stop();
		};

		wstring				inet_canonicalize_url(const wstring& url);
		inet_url_parts		inet_crack_url(const wstring& url);
		__int64				inet_get_resource_size(const wstring& url);
		bool				inet_get_resource_range(const wstring& url,__int64 start,__int64 length,inet_reader& reader);			// passing -1 as length implies no limit

		bool				check_http(const wstring& url,const vector<char>& post_data,vector<char>& output);
		bool				check_http(const wstring& url,const vector<char>& post_data,string& output);
		bool				check_http(const wstring& url);
		void				check_http_nonblock(const wstring& url);
		void				shutdown_http_nonblock();
	};
};

