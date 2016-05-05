
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace Core
	{
		class stack_any
		{
		private:
			size_t					_pos;
			vector<char>			_buffer;
			char*					_ptr;
		public:
			stack_any();
		public:
			size_t					available() const;
		public:
			void					push(const void* array_byte, size_t bytes);
			void					pop(void* array_byte, size_t bytes);
		public:
			template<class T>
			void					push(const T& data) {push(&data, sizeof(data));}
			template<class T>
			void					pop(T& data) {pop(&data, sizeof(data));}
		public:
			void					push(char data);
			void					push(unsigned char data);
			void					push(wchar_t data);
			void					push(short data);
			void					push(unsigned short data);
			void					push(int data);
			void					push(unsigned int data);
			void					push(long data);
			void					push(unsigned long data);
			void					push(__int64 data);
			void					push(unsigned __int64 data);
			void					push(bool data);
			void					push(float data);
			void					push(double data);
		public:
			char					popChar();
			wchar_t					popWChar();
			short					popShort();
			int						popInt();
			long					popLong();
			__int64					popInt64();
			bool					popBool();
			float					popFloat();
			double					popDouble();
		};
	};
};

