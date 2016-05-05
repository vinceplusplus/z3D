
#pragma once

#include "predecl.h"

#include "noncopyable.h"

#include "AlignedHeapOp.h"

namespace z3D
{
	namespace Core
	{
		class version: public AlignedHeapOp
		{
		private:
			static volatile uint64_t Z_ALIGN(8) 	_cur_ver;
			static volatile uint64_t Z_ALIGN(8) 	_cur_group;
		private:
			uint64_t Z_ALIGN(8)						_group;
			uint64_t Z_ALIGN(8) 					_ver;
		public:
			version();
			version(const version& right);
		public:
			bool				versioned() const;
		public:
			version&			operator=(const version& right);
		public:
			bool				operator>=(const version& right) const;
		public:
			void				setGroup(const version& v);
			bool				sameGroup(const version& v) const;
		public:
			void				update();
		public:
			void				reset();
		};

		// version tracked
		template<class T>
		class vt: noncopyable, AlignedHeapOp
		{
		private:
			T					_o;
		public:
			version				ver;
		public:
			const T&			get() const
			{
				return _o;
			}
			T&					edit()
			{
				ver.update();
				return _o;
			}
		};
	};
};

