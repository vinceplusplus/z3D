
#include "BillboardCtrl.h"
#include "SCameraNode.h"

namespace z3D
{
	namespace SG
	{
		BillboardCtrl::BillboardCtrl(const wcs& name)
			: CtrlBase(name)
		{
		}
		BillboardCtrl::~BillboardCtrl()
		{
		}
		void						BillboardCtrl::setCameraNode(const SPtr<SCameraNode>& camera)
		{
			_camera = camera;
		}
		SPtr<SCameraNode>			BillboardCtrl::getCameraNode() const
		{
			return _camera.lock();
		}
		void						BillboardCtrl::addBillboard(const SPtr<SNode>& node, bool yaw_only)
		{
			ITEM item = {node, yaw_only};
			_array_item.push_back(item);
		}
		void						BillboardCtrl::removeBillboard(const SPtr<SNode>& node)
		{
			for(size_t i = 0; i < _array_item.size(); ++i)
			{
				ITEM& item = _array_item[i];
				if(item.node.valid() && item.node.get_unsafe() == node.get())
				{
					_array_item.erase(_array_item.begin() + i);
					return;
				}
			}
		}
		void						BillboardCtrl::update(REAL dt)
		{
			if(!_camera.valid())
				return;
			if(!_array_item.size())
				return;
			for(size_t i = 0; i < _array_item.size(); ++i)
			{
				ITEM& item = _array_item[i];
				if(!item.node.valid())
				{
					_array_item.erase(_array_item.begin() + i);
					--i;
					continue;
				}
				if(item.yaw_only)
				{
					Vec3 dir = Vec3::zNegAxis * _camera.get_unsafe()->world_rot().asMat3();
					REAL yaw, pitch;
					dir.resolveOriRH(yaw,pitch);
					item.node.get_unsafe()->setWorldRot(Quat(Mat3::rotationX(90) * Mat3::rotationY(yaw)));
				}
				else
				{
					item.node.get_unsafe()->setWorldRot(Quat(Mat3::rotationX(90) * _camera.get_unsafe()->world_rot().asMat3()));
				}
			}
		}
		ptrdiff_t					BillboardCtrl::getPriority() const
		{
			return 5000;
		}
		bool						BillboardCtrl::isUpdateDeferred() const
		{
			return false;
		}
		void						BillboardCtrl::clear()
		{
			_array_item.clear();
		}
	};
};

