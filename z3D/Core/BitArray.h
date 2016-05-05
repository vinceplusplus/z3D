
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class BitArray
		{
		private:
			uint32_t*			_dword_array;
			uint32_t			_dword_count;
		private:
			uint32_t			_onbit_count;
			uint32_t*			_cur_dword_ptr;
			uint32_t			_cur_dword_pos;
			uint32_t			_cur_dword;
			uint32_t			_cur_bit_pos;
			uint32_t			_onbit_available;

		protected:
			void				___ctor(uint32_t dword_count);
			void				___ctor(uint32_t dword_count,const uint32_t* dword_array);
			void				___dtor();
		public:
			BitArray();
			BitArray(uint32_t dword_count);
			BitArray(uint32_t dword_count,const uint32_t* dword_array);
			BitArray(const BitArray& other);
			~BitArray();
		public:
			BitArray& operator=(const BitArray& other);
		public:
			void			recalc_onbit_count();
		public:
			uint32_t		onbit_count() const;
		public:
			void			set(uint32_t bit_pos);
			void			reset(uint32_t bit_pos);
			void			set();
			void			reset();
			bool			test(uint32_t bit_pos) const;
			bool			testSAFE(uint32_t bit_pos) const;
		public:
			void			and(const BitArray& bitarray);
			void			or(const BitArray& bitarray);
			void			xor(const BitArray& bitarray);
			void			and(uint32_t start_dword,const BitArray& bitarray);
			void			or(uint32_t start_dword,const BitArray& bitarray);
			void			xor(uint32_t start_dword,const BitArray& bitarray);
			void			not();
		public:
			int32_t			findfirst();					// return -1 if not found
			int32_t			findnext();						// return -1 if not found
		public:
			const uint32_t* dword_array() const;
			uint32_t		dword_count() const;
		};
	};
};

