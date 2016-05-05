
#define WIN32_MEAN_AND_LEAN

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include "NetSys.h"
#include "NetPools.h"

#include "../../xyssl/rsa.h"
#include "../../xyssl/havege.h"

#undef min
#undef max

namespace z3D
{
	namespace TCPFrm
	{
		bool			NetSys::startup()
		{
			_rsa_ctx = (void*)new rsa_context;
			memset(_rsa_ctx, 0, sizeof(rsa_context));

			WSADATA data;
			return !WSAStartup(MAKEWORD(2, 2), &data);
		}
		void			NetSys::shutdown()
		{
			WSACleanup();

			if(_rsa_ctx)
			{
				rsa_free((rsa_context*)_rsa_ctx);
				delete (rsa_context*)_rsa_ctx;
				_rsa_ctx = NULL;
			}
		}
		void			NetSys::rsa_set_example_public_key()
		{
			rsa_set_public_key(
				"9EF2FDD4430E05457FB4D428305E510D04CB90752DC0E6B3D6DDCE8C5069A42A566A0EA3DEF17D47EF9CB369602FF71F5FC39083508F4FDC8890828B575E9DE2CFB0BE3610ED13E5666EECCF367718B88302B2674D7BC0FF28ACABD75D136DC0375C160C4D71F4F0A5B01C8E57CAE7EFC15196BB6F49805EA74658004BE19825",
				"010001"
				);
		}
		void			NetSys::rsa_set_example_private_key()
		{
			rsa_set_private_key(
				"9EF2FDD4430E05457FB4D428305E510D04CB90752DC0E6B3D6DDCE8C5069A42A566A0EA3DEF17D47EF9CB369602FF71F5FC39083508F4FDC8890828B575E9DE2CFB0BE3610ED13E5666EECCF367718B88302B2674D7BC0FF28ACABD75D136DC0375C160C4D71F4F0A5B01C8E57CAE7EFC15196BB6F49805EA74658004BE19825",
				"010001",
				"3CF695C443B3ACF1EAF94025E9E742CEEB77E800097C62FFB6F97BAEF5688F3E6DC65FB71A5B24E1E25507BAF2D972BEE5CBEC317144862CDF740D9DC3F4D638FD12A32D391D61F353F0D60F569E1E177B5967C941867CA0F3C43FC80F2D1052E8C453B6FDAFADF3EABCF7383DEC47537604399E60C607635E5D275C0B8BEC41",
				"CA5A54FF29642B9A3F481E5F74E15401BD09EFB92CFBBE219C5DB30E3D00EFB4E3D222849245A27170661951BF3A5253CCDA5E35837FC2569E4956B50501F743",
				"C916DB488A8B1378116FB52D4E13AEFBEEB379293998F63E03362AAFE0CB49691850237C791BE88A5D0A70D9F4E9D4595581FABCBB0523213BFCC8A7CE0B3877",
				"6574B327183EB429EB4EA283E3DB523E68E4C1800BD0D0BC9364DB70E91F3E106925A3CEEA73E58591006FF36FB8B32C770A89AFB515EA7F796F75FCDEDCE477",
				"09258E01E5B35FC6555CB46C0422271274901B51C1B9F0716FA9F6151BEF833E5189CBA36CCAD52FFDFD6C96AD6A396B7AD537B77FF642DBCF23B768D299760F",
				"31572495C9B23971C2B89292408F3A717D4CC21A358D13EE47E560A48EA696AAEE496058BE9CE4D391F33701F06CA47ABF8231FB67910D3A02B556630262805C"
				);
		}
		void			NetSys::rsa_set_public_key(const char* N, const char* E)
		{
			rsa_context* ctx = (rsa_context*)_rsa_ctx;
			mpi_read_string(&ctx->N, 16, (char*)N);
			mpi_read_string(&ctx->E, 16, (char*)E);

			ctx->len = ( mpi_msb( &ctx->N ) + 7 ) >> 3;
		}
		void			NetSys::rsa_set_private_key(const char* N, const char* E, const char* D, const char* P, const char* Q, const char* DP, const char* DQ, const char* QP)
		{
			rsa_context* ctx = (rsa_context*)_rsa_ctx;
			mpi_read_string(&ctx->N, 16, (char*)N);
			mpi_read_string(&ctx->E, 16, (char*)E);
			mpi_read_string(&ctx->D, 16, (char*)D);
			mpi_read_string(&ctx->P, 16, (char*)P);
			mpi_read_string(&ctx->Q, 16, (char*)Q);
			mpi_read_string(&ctx->DP, 16, (char*)DP);
			mpi_read_string(&ctx->DQ, 16, (char*)DQ);
			mpi_read_string(&ctx->QP, 16, (char*)QP);

			ctx->len = ( mpi_msb( &ctx->N ) + 7 ) >> 3;
		}
		bool			NetSys::rsa_encrypt(const void* input, uint32_t input_len, void* output, uint32_t output_len)
		{
			scoped_lock<mutex_fast> o(_mx_rsa);

			if(rsa_pkcs1_encrypt((rsa_context*)_rsa_ctx, (unsigned char*)input, (int)input_len, (unsigned char*)output, (int)output_len))
				return false;
			return true;
		}
		bool			NetSys::rsa_decrypt(const void* input, uint32_t input_len, void* output, uint32_t* output_len)
		{
			scoped_lock<mutex_fast> o(_mx_rsa);

			int output_len2 = (int)*output_len;
			if(rsa_pkcs1_decrypt((rsa_context*)_rsa_ctx, (unsigned char*)input, (int)input_len, (unsigned char*)output, &output_len2))
				return false;
			*output_len = (uint32_t)output_len2;
			return true;
		}
		void			NetSys::rsa_gen_key(uint32_t nbits, uint32_t exponent, mbs& N, mbs& E, mbs& D, mbs& P, mbs& Q, mbs& DP, mbs& DQ, mbs& QP)
		{
			rsa_context ctx;
			havege_state hs;

			havege_init(&hs);

			::rsa_gen_key(&ctx, nbits, exponent, havege_rand, &hs);

			char buffer[4096]="";
			int len;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.N, 16, buffer, &len))
				N = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.E, 16, buffer, &len))
				E = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.D, 16, buffer, &len))
				D = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.P, 16, buffer, &len))
				P = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.Q, 16, buffer, &len))
				Q = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.DP, 16, buffer, &len))
				DP = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.DQ, 16, buffer, &len))
				DQ = buffer;

			len = _countof(buffer);
			if(!mpi_write_string(&ctx.QP, 16, buffer, &len))
				QP = buffer;

			rsa_free(&ctx);
		}
		uint32_t		NetSys::rsa_get_modulus_size()
		{
			return ((rsa_context*)_rsa_ctx)->len;
		}
		void			NetSys::gen_symmetric_encryption_table(unsigned char table[256])
		{
			int c = 0;
			for(int i=0;i<256;++i)
			{
				if( BITCOUNT(i) != 3 && BITCOUNT(i) != 5 )
					continue;
				table[c] = (unsigned char)i;
				++c;
			}
			for(int i = 0; c < 256; ++c, ++i)
				table[c] = table[i];

			lrng r;
			uint32_t key = GetTickCount();
			for(int i=0;i<65536;++i)
			{
				int i0 = rand() % 256;
				int i1 = rand() % 256;
				if(i0 == i1)
					continue;
				unsigned char t = table[i0];
				table[i0] = table[i1];
				table[i1] = t;
			}
		}
		void			NetSys::sym_encrypt(unsigned char* data, uint32_t len, uint64_t& key)
		{
			while(len)
			{
				unsigned char k[4];

				*(uint32_t*)k = lrng::randSpecial(key) ^ lrng::randSpecial(key) ^ lrng::randSpecial(key) ^ lrng::randSpecial(key);
				k[0] = _sym_encrypt_table[k[0]];
				k[1] = _sym_encrypt_table[k[1]];
				k[2] = _sym_encrypt_table[k[2]];
				k[3] = _sym_encrypt_table[k[3]];

				if(len >= 4)
				{
					uint32_t d = *(uint32_t*)data;
					uint32_t k2 = *(uint32_t*)k;

					d ^= k2;
					d = (d >> 8) | ((d & 0x000000ff)<< 24);
					d ^= k2;
					d = (d >> 8) | ((d & 0x000000ff) << 24);
					d ^= k2;
					d = (d >> 8) | ((d & 0x000000ff) << 24);
					d ^= k2;

					*(uint32_t*)data = d;
					data += 4;
					len -= 4;
				}
				else if(len >= 2)
				{
					uint16_t d = *(uint16_t*)data;
					uint16_t k2 = *(uint16_t*)k;

					d ^= k2;
					d = ((d & 0xff00) >> 8) | ((d & 0x00ff) << 8);
					d ^= k2;

					*(uint16_t*)data = d;
					data += 2;
					len -= 2;
				}
				else	// 1
				{
					*(uint8_t*)data ^= *(uint8_t*)k;
					break;
				}
			}
		}
		void			NetSys::sym_decrypt(unsigned char* data, uint32_t len, uint64_t& key)
		{
			while(len)
			{
				unsigned char k[4];

				*(uint32_t*)k = lrng::randSpecial(key) ^ lrng::randSpecial(key) ^ lrng::randSpecial(key) ^ lrng::randSpecial(key);
				k[0] = _sym_encrypt_table[k[0]];
				k[1] = _sym_encrypt_table[k[1]];
				k[2] = _sym_encrypt_table[k[2]];
				k[3] = _sym_encrypt_table[k[3]];

				if(len >= 4)
				{
					uint32_t d = *(uint32_t*)data;
					uint32_t k2 = *(uint32_t*)k;

					d ^= k2;
					d = ((d & 0x00ffffff) << 8) | (d >> 24);
					d ^= k2;
					d = ((d & 0x00ffffff) << 8) | (d >> 24);
					d ^= k2;
					d = ((d & 0x00ffffff) << 8) | (d >> 24);
					d ^= k2;

					*(uint32_t*)data = d;
					data += 4;
					len -= 4;
				}
				else if(len >= 2)
				{
					uint16_t d = *(uint16_t*)data;
					uint16_t k2 = *(uint16_t*)k;

					d ^= k2;
					d = ((d & 0x00ff) << 8) | ((d & 0xff00) >> 8);
					d ^= k2;

					*(uint16_t*)data = d;
					data += 2;
					len -= 2;
				}
				else	// 1
				{
					*(uint8_t*)data ^= *(uint8_t*)k;
					break;
				}
			}
		}
		void			NetSys::createPools(uint32_t max_netmsg, uint32_t max_send_overlapped, uint32_t max_reusable_bigmsg)
		{
			return NetPools::createPools(max_netmsg, max_send_overlapped, max_reusable_bigmsg);
		}
		void			NetSys::disposePools()
		{
			return NetPools::disposePools();
		}
		intptr_t		NetSys::createSocket()
		{
			SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if(s == INVALID_SOCKET)
				return s;

			if(true)
			{
				int zero = 0;
				if(setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char *)&zero, sizeof(zero)))
				{
					closesocket(s);
					return INVALID_SOCKET;
				}
				BOOL t = TRUE;
				if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&t, sizeof(t)))
				{
					closesocket(s);
					return INVALID_SOCKET;
				}
			}

			return s;
		}
		bool			NetSys::destroySocket(intptr_t s)
		{
			if(!isSocketHandleValid(s))
				return false;
			closesocket(s);
			return true;
		}
		bool			NetSys::isSocketHandleValid(intptr_t s)
		{
			return s != INVALID_SOCKET;
		}
		mbs				NetSys::gethostname()
		{
			char name[1024];
			if(!::gethostname(name,1024))
				return name;
			return "localhost";
		}
		vector<mbs>		NetSys::resolve_addr(const mbs& name)
		{
			vector<mbs> array_addr;

			addrinfo hint;
			memset(&hint, 0, sizeof(hint));

			hint.ai_family = PF_INET;

			addrinfo* res;
			if(!getaddrinfo(name.c_str(), "0", &hint, &res))
			{
				while(res)
				{
					unsigned char* array_byte = (unsigned char*)&(((sockaddr_in*)res->ai_addr)->sin_addr);
					array_addr.push_back(FORMATA("%u.%u.%u.%u", array_byte[0], array_byte[1], array_byte[2], array_byte[3]));

					res = res->ai_next;
				}

				freeaddrinfo(res);
			}
			return array_addr;
		}
		mbs				NetSys::resolve_1addr(const mbs& name)
		{
			vector<mbs> array_addr=resolve_addr(name);
			if(array_addr.size())
				return array_addr[0];
			return "";
		}
		unsigned long	NetSys::inet_addr(const mbs& dotted_addr)
		{
			return ::inet_addr(dotted_addr.c_str());
		}
		vector<mbs>	NetSys::getLocalAddresses()
		{
			return resolve_addr(gethostname());
		}
	};
};

