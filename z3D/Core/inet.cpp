
#include "inet.h"
#include "Misc.h"

namespace z3D
{
	namespace Core
	{
		static const wstring INET_USER_AGENT = L"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727) Web-Sniffer/1.0.24";

		inet_reader::inet_reader(__int64 bytes_to_read)
		{
			_bytes_to_read=bytes_to_read;
			_bytes_read=0;
		}
		inet_reader::~inet_reader() {}
		void					inet_reader::run(HINTERNET handle)
		{
			vector<char> buffer;
			while(true)
			{
				DWORD available=0;
				if(!InternetQueryDataAvailable(handle,&available,0,NULL))
					break;
				if(!available)
					break;
				buffer.resize(max(max(1,available),buffer.size()));
				__int64 bytes_to_read=(__int64)buffer.size();
				if(_bytes_to_read>=0)
					bytes_to_read=min(bytes_to_read,_bytes_to_read-_bytes_read);
				DWORD bytes_read=0;
				if(!InternetReadFile(handle,&buffer[0],(DWORD)bytes_to_read,&bytes_read))
					break;
				_bytes_read+=bytes_read;

				if(bytes_read)
					if(!read(&buffer[0],(long)bytes_read))
						break;

				if(_bytes_read==_bytes_to_read)
					break;
			}
			on_stop();
		}
		void					inet_reader::on_stop() {}
		bool					inet_reader::complete() const {return _bytes_read==_bytes_to_read;}

		inet_reader_string::inet_reader_string(__int64 bytes_to_read):inet_reader(bytes_to_read) {}
		bool					inet_reader_string::read(char* buffer,long len)
		{
			str+=string(buffer,len);
			return true;
		}

		inet_reader_buffered::inet_reader_buffered(__int64 bytes_to_read):inet_reader(bytes_to_read) {}
		bool					inet_reader_buffered::read(char* buffer,long len)
		{
			this->buffer.resize(this->buffer.size()+len);
			memcpy((&this->buffer[0])+this->buffer.size()-len,buffer,len);
			return true;
		}

		inet_reader_file::inet_reader_file(const wstring& filepath,bool append,__int64 bytes_to_read):inet_reader(bytes_to_read)
		{
			_handle_file=NULL;
			_filepath=filepath;
			_append=append;
		}
		inet_reader_file::~inet_reader_file()
		{
			if(_handle_file)
			{
				fclose(_handle_file);
				_handle_file=NULL;
			}
		}
		bool				inet_reader_file::read(char* buffer,long len)
		{
			if(!_handle_file)
			{
				_wfopen_s(&_handle_file,_filepath.c_str(),_append?L"ab":L"wb");
				if(!_handle_file)
					return false;
			}
			fwrite(buffer,len,1,_handle_file);
			return true;
		}
		void				inet_reader_file::on_stop()
		{
			if(_handle_file)
			{
				fclose(_handle_file);
				_handle_file=NULL;
			}
		}

		wstring				inet_canonicalize_url(const wstring& url)
		{
			if(!url.length())
				return L"";

			wstring s;
			s.resize(url.length()*6);
			DWORD canned=(DWORD)s.length();
			UrlCanonicalizeW(url.c_str(),(wchar_t*)s.c_str(),&canned,URL_ESCAPE_UNSAFE);
			s.resize(canned);

			return s;
		}

		inet_url_parts		inet_crack_url(const wstring& url)
		{
			wstring url2=inet_canonicalize_url(url);
			inet_url_parts parts;

			URL_COMPONENTS url_components;
			memset(&url_components,0,sizeof(url_components));
			url_components.dwStructSize=sizeof(url_components);
			url_components.dwSchemeLength=1;
			url_components.dwHostNameLength=1;
			url_components.dwUserNameLength=1;
			url_components.dwPasswordLength=1;
			url_components.dwUrlPathLength=1;
			url_components.dwExtraInfoLength=1;
			if(!InternetCrackUrlW(url2.c_str(),0,0,&url_components))
				return parts;

			parts.scheme_str=wstring(url_components.lpszScheme,url_components.dwSchemeLength);
			parts.hostname=wstring(url_components.lpszHostName,url_components.dwHostNameLength);
			parts.username=wstring(url_components.lpszUserName,url_components.dwUserNameLength);
			parts.password=wstring(url_components.lpszPassword,url_components.dwPasswordLength);
			parts.url_path=wstring(url_components.lpszUrlPath,url_components.dwUrlPathLength);
			parts.extra_info=wstring(url_components.lpszExtraInfo,url_components.dwExtraInfoLength);
			parts.scheme=url_components.nScheme;
			parts.port=url_components.nPort;

			return parts;
		}

		__int64				inet_get_resource_size(const wstring& url)
		{
			wstring url2=inet_canonicalize_url(url);

			__int64 res_size=-1;
			bool succ=false;

			HINTERNET handle_internet=NULL;
			HINTERNET handle_url=NULL;
			HINTERNET handle_connect=NULL;
			HINTERNET handle_file=NULL;

			inet_url_parts parts=inet_crack_url(url2);
			do
			{
				if(parts.scheme==INTERNET_SCHEME_HTTP)
				{
					handle_internet=InternetOpenW(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
					if(!handle_internet)
						break;
					handle_url=InternetOpenUrlW(handle_internet,url2.c_str(),NULL,0,0,NULL);
					if(!handle_url)
						break;

					wchar_t buffer[4096];
					DWORD buffer_len=sizeof(buffer);
					DWORD index=0;
					if(!HttpQueryInfoW(handle_url,HTTP_QUERY_CONTENT_LENGTH,buffer,&buffer_len,&index))
						break;
					res_size=_wtoi64(buffer);

					succ=true;
				}
				else if(parts.scheme==INTERNET_SCHEME_FTP)
				{
					handle_internet=InternetOpenW(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
					if(!handle_internet)
						break;
					handle_connect=InternetConnectW(handle_internet,parts.hostname.c_str(),parts.port,parts.username.c_str(),parts.password.c_str(),INTERNET_SERVICE_FTP,INTERNET_FLAG_PASSIVE,NULL);
					if(!handle_connect)
						break;
					handle_file=FtpOpenFile(handle_connect,parts.url_path.c_str(),GENERIC_READ,FTP_TRANSFER_TYPE_BINARY,NULL);
					if(!handle_file)
						break;
					DWORD low,high;
					low=FtpGetFileSize(handle_file,&high);
					if(low==(~0))
						break;
					res_size=low|(((__int64)high)<<32);

					succ=true;
				}
				else
					break;
			}while(false);

			if(handle_file)
				InternetCloseHandle(handle_file);
			if(handle_connect)
				InternetCloseHandle(handle_connect);
			if(handle_url)
				InternetCloseHandle(handle_url);
			if(handle_internet)
				InternetCloseHandle(handle_internet);

			return res_size;
		}
		bool				inet_get_resource_range(const wstring& url,__int64 start,__int64 length,inet_reader& reader)
		{
			wstring url2=inet_canonicalize_url(url);

			bool succ=false;

			HINTERNET handle_internet=NULL;
			HINTERNET handle_url=NULL;
			HINTERNET handle_connect=NULL;
			HINTERNET handle_file=NULL;

			inet_url_parts parts=inet_crack_url(url2);
			do
			{
				if(parts.scheme==INTERNET_SCHEME_HTTP)
				{
					handle_internet=InternetOpenW(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
					if(!handle_internet)
						break;
					wstring header;
					if(length<0)
						header=FORMATW(L"Range: bytes=%I64d-",start);
					else
						header=FORMATW(L"Range: bytes=%I64d-%I64d",start,start+length-1);
					handle_url=InternetOpenUrlW(handle_internet,url2.c_str(),header.c_str(),0,0,NULL);
					if(!handle_url)
						break;

					wchar_t buffer[4096];
					DWORD buffer_len=sizeof(buffer);
					DWORD index=0;

					__int64 len=-1;

					if(HttpQueryInfoW(handle_url,HTTP_QUERY_CONTENT_LENGTH,buffer,&buffer_len,&index))
						len=_wtoi64(buffer);

					reader.run(handle_url);
				}
				else if(parts.scheme==INTERNET_SCHEME_FTP)
				{
					handle_internet=InternetOpenW(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
					if(!handle_internet)
						break;
					handle_connect=InternetConnectW(handle_internet,parts.hostname.c_str(),parts.port,parts.username.c_str(),parts.password.c_str(),INTERNET_SERVICE_FTP,INTERNET_FLAG_PASSIVE,NULL);
					if(!handle_connect)
						break;
					wstring command=FORMATW(L"REST %I64d",start);
					FtpCommandW(handle_connect,FALSE,FTP_TRANSFER_TYPE_BINARY,command.c_str(),NULL,NULL);

					handle_file=FtpOpenFileW(handle_connect,parts.url_path.c_str(),GENERIC_READ,FTP_TRANSFER_TYPE_BINARY,NULL);
					if(!handle_file)
						break;

					reader.run(handle_file);
				}
				else
					break;
			}while(false);

			if(handle_file)
				InternetCloseHandle(handle_file);
			if(handle_connect)
				InternetCloseHandle(handle_connect);
			if(handle_url)
				InternetCloseHandle(handle_url);
			if(handle_internet)
				InternetCloseHandle(handle_internet);

			return true;
		}
	};
};

