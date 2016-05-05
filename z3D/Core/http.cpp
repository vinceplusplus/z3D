
#include "inet.h"
#include "mutex_fast.h"

#include <queue>
using namespace std;

namespace z3D
{
	namespace Core
	{
		static const wstring INET_USER_AGENT = L"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727) Web-Sniffer/1.0.24";

		static mutex_fast				_mx_http_nonblock;
		static HANDLE					_thread_http_nonblock = NULL;
		static queue<wstring>			_url_queue_http_nonblock;
		static bool						_need_stop_http_nonblock = false;

		static DWORD WINAPI				http_nonblock_thread_proc(LPVOID param)
		{
			while(!_need_stop_http_nonblock)
			{
				_mx_http_nonblock.lock();

				while(_url_queue_http_nonblock.size())
				{
					check_http(_url_queue_http_nonblock.front());
					_url_queue_http_nonblock.pop();
				}

				_mx_http_nonblock.unlock();
				Sleep(100);
			}

			return 0;
		}

		static void						start_http_nonblock_thread()
		{
			_mx_http_nonblock.lock();
			if(!_thread_http_nonblock)
			{
				DWORD threadid;
				_thread_http_nonblock = CreateThread(NULL,0,http_nonblock_thread_proc,NULL,CREATE_SUSPENDED,&threadid);
				ResumeThread(_thread_http_nonblock);

				_need_stop_http_nonblock = false;
			}
			_mx_http_nonblock.unlock();
		}

		bool		check_http(const wstring& url)
		{
			wchar_t url2_buffer[4096];
			DWORD canned=sizeof(url2_buffer)/sizeof(wchar_t);
			UrlCanonicalize(url.c_str(),url2_buffer,&canned,URL_ESCAPE_UNSAFE);

			wstring url2=url2_buffer;

			int size=0;

			HINTERNET handle_internet=NULL;
			HINTERNET handle_connection=NULL;
			HINTERNET handle_request=NULL;

			do
			{
				handle_internet = InternetOpen(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);

				if(!handle_internet)
					break;
				URL_COMPONENTS url_components;
				memset(&url_components,0,sizeof(url_components));
				url_components.dwStructSize=sizeof(url_components);
				url_components.dwSchemeLength=1;
				url_components.dwHostNameLength=1;
				url_components.dwUrlPathLength=1;
				if(!InternetCrackUrl(url2.c_str(),0,0,&url_components))
					break;
				wstring scheme(url_components.lpszScheme,url_components.dwSchemeLength);
				wstring hostname(url_components.lpszHostName,url_components.dwHostNameLength);
				wstring urlpath(url_components.lpszUrlPath,url_components.dwUrlPathLength);

				handle_connection = InternetConnect(handle_internet,hostname.c_str(),url_components.nPort,NULL,NULL,INTERNET_SERVICE_HTTP,0,NULL);

				const wchar_t* accept_types[]={L"*/*",NULL};

				// using "HEAD" as verb will not work on dynamic pages such as php
				handle_request = HttpOpenRequest(handle_connection,L"GET",urlpath.c_str(),NULL,NULL,accept_types,0,NULL);

				if(!handle_request)
					break;

				if(!HttpSendRequest(handle_request,NULL,0,NULL,0))
					break;

				int index=0;

				wchar_t buffer[4096];
				int buffer_length=4096;
				if(!HttpQueryInfo(handle_request,HTTP_QUERY_CONTENT_LENGTH,buffer,(DWORD*)&buffer_length,(DWORD*)&index))
					break;

				size=_wtoi(buffer);

			}while(false);

			if(handle_request)
				InternetCloseHandle(handle_request);
			if(handle_connection)
				InternetCloseHandle(handle_connection);
			if(handle_internet)
				InternetCloseHandle(handle_internet);

			return size?true:false;
		}

		void		check_http_nonblock(const wstring& url)
		{
			start_http_nonblock_thread();

			_mx_http_nonblock.lock();
			_url_queue_http_nonblock.push(url);
			_mx_http_nonblock.unlock();
		}

		void		shutdown_http_nonblock()
		{
			_need_stop_http_nonblock = true;

			while(true)
			{
				_mx_http_nonblock.lock();
				DWORD ret=WaitForSingleObject(_thread_http_nonblock,0);
				if(ret==WAIT_OBJECT_0)
				{
					CloseHandle(_thread_http_nonblock);
					_thread_http_nonblock = NULL;
				}
				_mx_http_nonblock.unlock();
				if(!_thread_http_nonblock)
					return;
			}
		}
		bool check_http(const wstring& url,const vector<char>& post_data,vector<char>& output)
		{
			URL_COMPONENTS url_comp;
			memset(&url_comp,0,sizeof(url_comp));
			url_comp.dwStructSize=sizeof(url_comp);
			url_comp.dwExtraInfoLength=1;
			url_comp.dwHostNameLength=1;
			url_comp.dwPasswordLength=1;
			url_comp.dwSchemeLength=1;
			url_comp.dwUrlPathLength=1;
			url_comp.dwUserNameLength=1;
			InternetCrackUrlW(url.c_str(),(DWORD)url.length(),0,&url_comp);

			HINTERNET handle_internet=NULL;
			HINTERNET handle_connect=NULL;
			HINTERNET handle_request=NULL;

			bool succ=false;

			do
			{
				handle_internet=InternetOpenW(INET_USER_AGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);
				if(!handle_internet)
					break;
				handle_connect=InternetConnect(handle_internet,wstring(url_comp.lpszHostName,url_comp.dwHostNameLength).c_str(),url_comp.nPort,0,0,INTERNET_SERVICE_HTTP,0,0);
				if(!handle_connect)
					break;
				const wchar_t* accept_types[]={L"*/*",0};
				handle_request=HttpOpenRequest(handle_connect,post_data.size()?L"POST":L"GET",wstring(url_comp.lpszUrlPath).c_str(),0,0,accept_types,(url_comp.nScheme==INTERNET_SCHEME_HTTPS)?INTERNET_FLAG_SECURE:0,0);
				if(!handle_request)
					break;
				if(!HttpSendRequest(handle_request,0,0,post_data.size()?((void*)&post_data[0]):0,(DWORD)post_data.size()))
					break;

				DWORD code=300;
				DWORD index=0;
				DWORD code_len=4;
				if(!HttpQueryInfo(handle_request,HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,&code,&code_len,&index))
					break;
				if(code>=300)
					break;

				char buffer[4096];
				DWORD bytes_read=0;
				DWORD available=0;

				while(true)
				{
					if(!InternetQueryDataAvailable(handle_request,&available,0,0))
						break;
					if(!available)
						break;
					if(!InternetReadFile(handle_request,buffer,sizeof(buffer),&bytes_read))
						break;
					if(bytes_read)
					{
						output.resize(output.size()+bytes_read);
						memcpy(&output[output.size()-bytes_read],buffer,bytes_read);
					}
				}

				succ=true;
			}while(false);

			if(handle_request)
				InternetCloseHandle(handle_request);
			if(handle_connect)
				InternetCloseHandle(handle_connect);
			if(handle_internet)
				InternetCloseHandle(handle_internet);

			return succ;
		}

		bool check_http(const wstring& url,const vector<char>& post_data,string& output)
		{
			vector<char> output2;
			if(!check_http(url,post_data,output2))
				return false;
			output.resize(output2.size());
			if(output.size())
				memcpy(&output[0],&output2[0],output.size());
			return true;
		}

	};
};