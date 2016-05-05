
#pragma once

#include "predecl.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		class BillboardCtrl: public CtrlBase
		{
		private:
			class ITEM
			{
			public:
				WPtr<SNode>			node;
				size_t				yaw_only;
			};
		private:
			WPtr<SCameraNode>		_camera;
		private:
			vector<ITEM>			_array_item;
		public:
			BillboardCtrl(const wcs& name);
		protected:
			~BillboardCtrl();
		public:
			void						setCameraNode(const SPtr<SCameraNode>& camera);
			SPtr<SCameraNode>			getCameraNode() const;
		public:
			void						addBillboard(const SPtr<SNode>& node, bool yaw_only);
			void						removeBillboard(const SPtr<SNode>& node);
		public:
			void						update(REAL dt);
		public:
			ptrdiff_t					getPriority() const;
			bool						isUpdateDeferred() const;
		public:
			void						clear();
		};
	};
};


