
#include "stack_any.h"

namespace z3D
{
	namespace Core
	{
		stack_any::stack_any()
		{
			_pos = 0;
			_ptr = NULL;
		}
		size_t					stack_any::available() const
		{
			return _pos;
		}
		void					stack_any::push(const void* array_byte, size_t bytes)
		{
			if(!bytes)
				return;
			_buffer.resize(max(_buffer.size(), _pos+bytes));
			_ptr = &_buffer[0];
			memcpy(_ptr + _pos, array_byte, bytes);
			_pos += bytes;
		}
		void					stack_any::pop(void* array_byte, size_t bytes)
		{
			if(!bytes)
				return;
			_pos -= bytes;
			if((ptrdiff_t)_pos < 0)
			{
				Z_ASSERT(false);
				_pos += bytes;
				return;
			}
			memcpy(array_byte, _ptr + _pos, bytes);
		}
		void					stack_any::push(char data) {push(&data, 1);}
		void					stack_any::push(unsigned char data) {push(&data, 1);}
		void					stack_any::push(wchar_t data) {push(&data, 2);}
		void					stack_any::push(short data) {push(&data, 2);}
		void					stack_any::push(unsigned short data) {push(&data, 2);}
		void					stack_any::push(int data) {push(&data, sizeof(int));}
		void					stack_any::push(unsigned int data) {push(&data, sizeof(unsigned int));}
		void					stack_any::push(long data) {push(&data, 4);}
		void					stack_any::push(unsigned long data) {push(&data, 4);}
		void					stack_any::push(__int64 data) {push(&data, 8);}
		void					stack_any::push(unsigned __int64 data) {push(&data, 8);}
		void					stack_any::push(bool data) {push(&data, 1);}
		void					stack_any::push(float data) {push(&data, 4);}
		void					stack_any::push(double data) {push(&data, 8);}
		char					stack_any::popChar() {char data; pop(&data, sizeof(data)); return data;}
		wchar_t					stack_any::popWChar() {wchar_t data; pop(&data, sizeof(data)); return data;}
		short					stack_any::popShort() {short data; pop(&data, sizeof(data)); return data;}
		int						stack_any::popInt() {int data; pop(&data, sizeof(data)); return data;}
		long					stack_any::popLong() {long data; pop(&data, sizeof(data)); return data;}
		__int64					stack_any::popInt64() {__int64 data; pop(&data, sizeof(data)); return data;}
		bool					stack_any::popBool() {bool data; pop(&data, sizeof(data)); return data;}
		float					stack_any::popFloat() {float data; pop(&data, sizeof(data)); return data;}
		double					stack_any::popDouble() {double data; pop(&data, sizeof(data)); return data;}
	};
};

