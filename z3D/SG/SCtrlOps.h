
#pragma once

#include "predecl.h"

namespace z3D
{
	namespace SG
	{
		class SCtrlOps
		{
		public:
			static void						update(CtrlUpdater& updater, const SPtr<SNode>& node, size_t levels_left = -1);
			static void						set(const TimeAnimCfg& cfg, const SPtr<SNode>& node, size_t levels_left = -1);
			static void						transit(const TimeAnimCfg& cfg, REAL duration, const SPtr<SNode>& node, size_t levels_left = -1);
			static void						setOnlyTM(const TimeAnimCfg& cfg, const SPtr<SNode>& node, size_t levels_left = -1);
			static void						transitOnlyTM(const TimeAnimCfg& cfg, REAL duration, const SPtr<SNode>& node, size_t levels_left = -1);
			static void						enumerate(vector<SPtr<CtrlBase>>& ctrls, const SPtr<SNode>& node, size_t levels_left = -1);
			static bool						testStopped(const SPtr<SNode>& node, size_t levels_left = -1);
		};
	};
};

