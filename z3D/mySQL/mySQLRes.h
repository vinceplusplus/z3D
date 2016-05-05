
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace mySQL
	{
		class mySQLRes
		{
		private:
			shared_count	_sc;
		private:
			void*			_res;
			unsigned int	_codepage;
		private:
			void				___ctor();
			void				___ctor(void* res, unsigned int codepage);
			void				___ctor(const mySQLRes& other);
			void				___dtor();
		public:
			mySQLRes();
			mySQLRes(void* res, unsigned int codepage);			// ownership is passed
		public:
			~mySQLRes();
		public:
			mySQLRes(const mySQLRes& other);					// copy constructor
		public:
			mySQLRes&			operator=(const mySQLRes& other);
		public:
			unsigned __int64	rows() const;
		public:
			bool				empty() const;
		public:
			char**				row(unsigned __int64 row);
			char**				nextRow();
		public:
			char				getChar(int r, int c);
			short				getShort(int r, int c);
			int					getInt(int r, int c);
			__int64				getInt64(int r, int c);
			unsigned char		getUChar(int r, int c);
			unsigned short		getUShort(int r, int c);
			unsigned int		getUInt(int r, int c);
			unsigned __int64	getUInt64(int r, int c);
			float				getFloat(int r, int c);
			double				getDouble(int r, int c);
			mbs					getString(int r, int c);
			wcs					getWString(int r, int c);
		};
	};
};