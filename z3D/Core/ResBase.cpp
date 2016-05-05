
#include "ResBase.h"
#include "ResMgr.h"
#include "functor_inl.h"

namespace z3D
{
	namespace Core
	{
		RefBaseImpl::RefBaseImpl() {}
		RefBaseImpl::~RefBaseImpl() {}
		void								RefBaseImpl::dispose()
		{
			if(_destroy_func.valid())
				_destroy_func(this);
			else
				delete this;
		}
		bool								RefBaseImpl::checkHeldBySPtr() const
		{
			return (_sc.use_count() && _sc.ref_count() > 1);
		}
		void								RefBaseImpl::setUniqueTrigger(const functor1<void, void*>& func, void* ref)
		{
			_unique_trigger.func = func;
			_unique_trigger.ref = ref;
		}
		void								RefBaseImpl::getUniqueTrigger(functor1<void, void*>& func, void*& ref) const
		{
			func = _unique_trigger.func;
			ref = _unique_trigger.ref;
		}
		void								RefBaseImpl::setDestroyFunc(const functor1<void, RefBaseImpl*>& destroy_func)
		{
			_destroy_func = destroy_func;
		}
		const functor1<void, RefBaseImpl*>&	RefBaseImpl::getDestroyFunc() const
		{
			return _destroy_func;
		}


		RefBase::RefBase() {}
		RefBase::~RefBase() {}

		ResBase::ResBase(const wcs& filepath) : _filepath(filepath), _res_mgr(0) {}
		ResBase::~ResBase()
		{
			if(_res_mgr)
				_res_mgr->remove(this);
		}
		void								ResBase::setResMgr(ResMgr* mgr)
		{
			_res_mgr = mgr;
		}
		ResMgr*								ResBase::getResMgr()
		{
			return _res_mgr;
		}
		const wcs&							ResBase::filepath() const
		{
			return _filepath;
		}
		void								ResBase::set_filepath(const wcs& filepath)
		{
			_filepath = filepath;
		}
	};
};

