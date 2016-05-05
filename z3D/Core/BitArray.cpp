
#include "BitArray.h"
#include "MathFunc.h"

#define SAFE_DELETE(x) {if((x)!=NULL){delete (x);(x)=NULL;}}
#define SAFE_DELETE_ARRAY(x) {if((x)!=NULL){delete[] (x);(x)=NULL;}}

namespace z3D
{
	namespace Core
	{
		void				BitArray::___ctor(uint32_t dword_count)
		{
			dword_count = max((uint32_t)1,dword_count);
			_dword_array=new uint32_t[dword_count];
			_dword_count=dword_count;
			_cur_dword_ptr=NULL;
			_cur_dword_pos=0;
			_cur_dword=0;
			_cur_bit_pos=0;
			_onbit_available=0;
			reset();
		}
		void				BitArray::___ctor(uint32_t dword_count,const uint32_t* dword_array)
		{
			dword_count = max((uint32_t)1,dword_count);
			_dword_array=new uint32_t[dword_count];
			_dword_count=dword_count;
			_cur_dword_ptr=NULL;
			_cur_dword_pos=0;
			_cur_dword=0;
			_cur_bit_pos=0;
			_onbit_available=0;
			memcpy(_dword_array,dword_array,sizeof(unsigned long)*dword_count);
			recalc_onbit_count();
		}
		void				BitArray::___dtor()
		{
			SAFE_DELETE_ARRAY(_dword_array);
		}
		BitArray::BitArray()
		{
			___ctor(1);
		}
		BitArray::BitArray(uint32_t dword_count)
		{
			___ctor(dword_count);
		}
		BitArray::BitArray(uint32_t dword_count,const uint32_t* dword_array)
		{
			___ctor(dword_count,dword_array);
		}
		BitArray::BitArray(const BitArray& other)
		{
			___ctor(other._dword_count,other._dword_array);
		}
		BitArray::~BitArray()
		{
			___dtor();
		}
		BitArray& BitArray::operator=(const BitArray& other)
		{
			___dtor();
			___ctor(other._dword_count,other._dword_array);
			return (*this);
		}
		uint32_t BitArray::onbit_count() const
		{
			return _onbit_count;
		}
		void BitArray::recalc_onbit_count()
		{
			_onbit_count=0;
			uint32_t* end=_dword_array+_dword_count;
			uint32_t* ptr=_dword_array-1;
			while(end!=(++ptr))
				_onbit_count+=BITCOUNT(*ptr);
		}
		void BitArray::set(uint32_t bit_pos)
		{
#ifdef _DEBUG
			if(bit_pos<0||bit_pos>=(_dword_count<<5))
			{
				*((unsigned char*)0)=100;
			}
#endif
			int dword_pos=bit_pos>>5;
			unsigned long test_bit=1<<(bit_pos&31);
			if(_dword_array[dword_pos]&test_bit)
				return;
			_dword_array[dword_pos]|=test_bit;
			_onbit_count++;
		}
		void BitArray::reset(uint32_t bit_pos)
		{
			int dword_pos=bit_pos>>5;
			unsigned long test_bit=1<<(bit_pos&31);
			if(!(_dword_array[dword_pos]&test_bit))
				return;
			_dword_array[dword_pos]&=~test_bit;
			_onbit_count--;
		}
		void BitArray::set()
		{
			int count=_dword_count;
			uint32_t* ptr=_dword_array;
			while(count--)
				*(ptr++)=0xffffffff;
			_onbit_count=_dword_count<<5;
		}
		void BitArray::reset()
		{
			int count=_dword_count;
			uint32_t* ptr=_dword_array;
			while(count--)
				*(ptr++)=0;
			_onbit_count=0;
		}
		bool BitArray::test(uint32_t bit_pos) const
		{
			int dword_pos=bit_pos>>5;
			unsigned long test_bit=1<<(bit_pos&31);
			return (_dword_array[dword_pos]&test_bit)?true:false;
		}
		bool BitArray::testSAFE(uint32_t bit_pos) const
		{
			if((bit_pos>>5)>=_dword_count)
				return false;
			int dword_pos=bit_pos>>5;
			unsigned long test_bit=1<<(bit_pos&31);
			return (_dword_array[dword_pos]&test_bit)?true:false;
		}
		void BitArray::and(const BitArray& bitarray)
		{
			int count=min(_dword_count,bitarray._dword_count);
			uint32_t* ptr1=_dword_array,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1&=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::or(const BitArray& bitarray)
		{
			int count=min(_dword_count,bitarray._dword_count);
			uint32_t* ptr1=_dword_array,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1|=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::xor(const BitArray& bitarray)
		{
			uint32_t count=min(_dword_count,bitarray._dword_count);
			uint32_t* ptr1=_dword_array,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1^=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::and(uint32_t start_dword,const BitArray& bitarray)
		{
			uint32_t count=max(min(_dword_count-start_dword,bitarray._dword_count),(uint32_t)0);
			uint32_t* ptr1=_dword_array+start_dword,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1&=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::or(uint32_t start_dword,const BitArray& bitarray)
		{
			uint32_t count=max(min(_dword_count-start_dword,bitarray._dword_count),(uint32_t)0);
			uint32_t* ptr1=_dword_array+start_dword,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1|=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::xor(uint32_t start_dword,const BitArray& bitarray)
		{
			int count=max(min(_dword_count-start_dword,bitarray._dword_count),(uint32_t)0);
			uint32_t* ptr1=_dword_array+start_dword,* ptr2=bitarray._dword_array;
			while(count)
			{
				*ptr1^=*ptr2;
				++ptr1;
				++ptr2;
				--count;
			}
//			recalc_onbit_count();
		}
		void BitArray::not()
		{
			uint32_t count=_dword_count;
			uint32_t* ptr=_dword_array;
			while(count)
			{
				*ptr=~(*ptr);
				ptr++;
				count--;
			}
//			recalc_onbit_count();
		}
		int32_t BitArray::findfirst()
		{
			_cur_dword_ptr=_dword_array;
			_cur_dword_pos=0;
			_cur_dword=*_cur_dword_ptr;
			_cur_bit_pos=0;
			_onbit_available=_onbit_count;
			return findnext();
		}
		int32_t BitArray::findnext()
		{
			while(true)
			{
				if(!_onbit_available)
					return -1;
				while(!_cur_dword)
				{
					if(_cur_dword_pos>=_dword_count)
						return -1;
					++_cur_dword_ptr;
					++_cur_dword_pos;
					_cur_dword=*_cur_dword_ptr;
					_cur_bit_pos=0;
				}
				while(_cur_dword)
				{
					if(!(*(unsigned char*)&_cur_dword))
					{
						_cur_dword>>=8;
						_cur_bit_pos+=8;
						continue;
					}
					unsigned long pos=FIRSTBIT(*(unsigned char*)&_cur_dword);
					int pos2=(_cur_dword_pos<<5)|(_cur_bit_pos+pos);
					++pos;				// for optimization
					_cur_dword>>=pos;
					_cur_bit_pos+=pos;
					--_onbit_available;
					return pos2;
				}
			}
		}
		const uint32_t* BitArray::dword_array() const
		{
			return _dword_array;
		}
		uint32_t BitArray::dword_count() const
		{
			return _dword_count;
		}
	};
};
