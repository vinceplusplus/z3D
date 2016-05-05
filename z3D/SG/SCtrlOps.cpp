
#include "SCtrlOps.h"
#include "SVisitor.h"
#include "SNode.h"

namespace z3D
{
	namespace SG
	{
		void							SCtrlOps::update(CtrlUpdater& updater, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				CtrlUpdater&				_updater;
			public:
				visitor(CtrlUpdater& updater)
					: _updater(updater)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					CtrlSet& cs = node->getCtrlSet();

					cs.update(_updater);

					return true;
				}
			};

			updater.reset();

			visitor v(updater);
			v.traverse(node, levels_left);
			if(!updater.isManuallyExecDeferredUpdate())
				updater.execDeferredUpdate();
		}
		void							SCtrlOps::set(const TimeAnimCfg& cfg, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				const TimeAnimCfg&			_cfg;
			public:
				visitor(const TimeAnimCfg& cfg)
					: _cfg(cfg)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					CtrlSet& cs = node->getCtrlSet();

					cs.set(_cfg);

					return true;
				}
			};

			visitor v(cfg);
			v.traverse(node, levels_left);
		}
		void							SCtrlOps::transit(const TimeAnimCfg& cfg, REAL duration, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				const TimeAnimCfg&			_cfg;
				const REAL					_duration;
			public:
				visitor(const TimeAnimCfg& cfg, REAL duration)
					: _cfg(cfg), _duration(duration)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					CtrlSet& cs = node->getCtrlSet();

					cs.transit(_cfg, _duration);

					return true;
				}
			};

			visitor v(cfg, duration);
			v.traverse(node, levels_left);
		}
		void							SCtrlOps::setOnlyTM(const TimeAnimCfg& cfg, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				const TimeAnimCfg&			_cfg;
			public:
				visitor(const TimeAnimCfg& cfg)
					: _cfg(cfg)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					SPtr<CtrlBase> tm = node->getTMCtrl();
					if(tm && tm->hasAnimData())
						node->getTMCtrl()->getAnimCtrl()->set(_cfg);

					return true;
				}
			};

			visitor v(cfg);
			v.traverse(node, levels_left);
		}
		void							SCtrlOps::transitOnlyTM(const TimeAnimCfg& cfg, REAL duration, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				const TimeAnimCfg&			_cfg;
				const REAL					_duration;
			public:
				visitor(const TimeAnimCfg& cfg, REAL duration)
					: _cfg(cfg), _duration(duration)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					SPtr<CtrlBase> tm = node->getTMCtrl();
					if(tm && tm->hasAnimData())
						node->getTMCtrl()->getAnimCtrl()->transit(_cfg, _duration);

					return true;
				}
			};

			visitor v(cfg, duration);
			v.traverse(node, levels_left);
		}
		void							SCtrlOps::enumerate(vector<SPtr<CtrlBase>>& ctrls, const SPtr<SNode>& node, size_t levels_left)
		{
			class visitor: public SVisitor
			{
			private:
				vector<SPtr<CtrlBase>>&		_ctrls;
			public:
				visitor(vector<SPtr<CtrlBase>>& ctrls)
					: _ctrls(ctrls)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					CtrlSet& cs = node->getCtrlSet();

					class fn
					{
					private:
						vector<SPtr<CtrlBase>>&		_ctrls;
					public:
						fn(vector<SPtr<CtrlBase>>& ctrls)
							: _ctrls(ctrls)
						{}
					public:
						void						operator() (const SPtr<CtrlBase>& ctrl)
						{
							_ctrls.push_back(ctrl);
						}
					};

					for_each<vector<SPtr<CtrlBase>>::const_iterator, fn&>(cs.getArray().begin(), cs.getArray().end(), fn(_ctrls));

					return true;
				}
			};

			visitor v(ctrls);
			v.traverse(node, levels_left);
		}
		bool						SCtrlOps::testStopped(const SPtr<SNode>& node, size_t levels_left)
		{
			bool stopped = true;

			class visitor: public SVisitor
			{
			private:
				bool&						_stopped;
			public:
				visitor(bool& stopped)
					: _stopped(stopped)
				{}
			public:
				bool						onVisit(const SPtr<SNode>& node)
				{
					if(!node->visible())
						return false;

					CtrlSet& cs = node->getCtrlSet();

					_stopped &= cs.isStopped();

					return true;
				}
			};

			visitor v(stopped);
			v.traverse(node, levels_left);

			return stopped;
		}
	};
};

