
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace TCPFrm
	{
		class NetSys
		{
		private:
			static mutex_fast		_mx_rsa;
			static void*			_rsa_ctx;
		private:
			static unsigned char	_sym_encrypt_table[256];
		public:
			static bool				startup();
			static void				shutdown();
		public:
			static void				rsa_set_example_public_key();																// for quick start purpose
			static void				rsa_set_example_private_key();																// for quick start purpose
		public:
			static void				rsa_set_public_key(const char* N, const char* E);																									// all in radix-16, must be set during program initialization, should be set in client side
			static void				rsa_set_private_key(const char* N, const char* E, const char* D, const char* P, const char* Q, const char* DP, const char* DQ, const char* QP);		// all in radix-16, must be set during program initialization, should be set in server side
		public:
			static bool				rsa_encrypt(const void* input, uint32_t input_len, void* output, uint32_t output_len);		// input_len must be shorter than the modulus minus 3+8, output_len must be the same size as the modulus
			static bool				rsa_decrypt(const void* input, uint32_t input_len, void* output, uint32_t* output_len);		// input_len must be the same size as the modulus, output_len will be updated to the length of the original message
		public:
			static void				rsa_gen_key(uint32_t nbits, uint32_t exponent, mbs& N, mbs& E, mbs& D, mbs& P, mbs& Q, mbs& DP, mbs& DQ, mbs& QP);			// recommended exponent is 65537
		public:
			static uint32_t			rsa_get_modulus_size();																		// in bytes
		public:
			static void				gen_symmetric_encryption_table(unsigned char table[256]);
		public:
			static void				sym_encrypt(unsigned char* data, uint32_t len, uint64_t& key);
			static void				sym_decrypt(unsigned char* data, uint32_t len, uint64_t& key);
		public:
			static void				createPools(uint32_t max_netmsg, uint32_t max_send_overlapped, uint32_t max_reusable_bigmsg);	// 256, 256, 1 are suggested for client, 32768, 32768, 4096 are suggested for server
			static void				disposePools();
		public:
			static intptr_t			createSocket();
			static bool				destroySocket(intptr_t s);
			static bool				isSocketHandleValid(intptr_t s);
		public:
			static mbs				gethostname();
			static vector<mbs>		resolve_addr(const mbs& name);
			static mbs				resolve_1addr(const mbs& name);
			static unsigned long	inet_addr(const mbs& dotted_addr);
		public:
			static vector<mbs>		getLocalAddresses();
		};
	};
};

