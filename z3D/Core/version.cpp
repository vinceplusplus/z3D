
#include "version.h"

#include "interlocked.h"

namespace z3D
{
	namespace Core
	{
		version::version()
		{
			Z_ASSERT_ALIGN(&_group, sizeof(_group));
			Z_ASSERT_ALIGN(&_ver, sizeof(_ver));

			if(true)
			{
				uint64_t ex = _cur_group;
				while(interlocked::compare_exchange_64(&_cur_group, ex + 1, ex) != ex)
					ex = _cur_group;														// need sleep or yield ??
				_group = ex + 1;
			}
			_ver = 0;
		}
		version::version(const version& right)
		{
			_group = right._group;
			_ver = right._ver;
		}
		bool				version::versioned() const
		{
			return !!_ver;
		}
		version&			version::operator=(const version& right)
		{
			_group = right._group;
			_ver = right._ver;
			return *this;
		}
		bool				version::operator>=(const version& right) const
		{
			return (_group == right._group) ? ((_ver == right._ver) ? (_ver ? true : false) : (_ver > right._ver)) : false;
		}
		void				version::setGroup(const version& v)
		{
			_group = v._group;
			_ver = 0;
		}
		bool				version::sameGroup(const version& v) const
		{
			return _group == v._group;
		}
		void				version::update()
		{
			if(true)
			{
				uint64_t ex = _cur_ver;
				while(interlocked::compare_exchange_64(&_cur_ver, ex + 1, ex) != ex)
					ex = _cur_ver;															// need sleep or yield ??
				_ver = ex + 1;
			}
		}
		void				version::reset()
		{
			_ver = 0;
		}
	};
};

