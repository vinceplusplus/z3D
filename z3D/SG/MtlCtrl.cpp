
#include "MtlCtrl.h"
#include "SMeshNode.h"
#include "MeshInst.h"

namespace z3D
{
	namespace SG
	{
		template<class T> static REAL	getStartTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(0).time;
		}
		template<class T> static REAL	getEndTime(const AnimDataEval<T>& eval, REAL default_time)
		{
			if(!eval.anim_data() || !eval.anim_data()->getKeyCount())
				return default_time;
			return eval.anim_data()->getKey(eval.anim_data()->getKeyCount() - 1).time;
		}

		MtlCtrl::MapAnimDataEval::MapAnimDataEval(): offset_u(NULL), offset_v(NULL), tiling_u(NULL), tiling_v(NULL), angle_u(NULL), angle_v(NULL), angle_w(NULL)
		{}
		void							MtlCtrl::MapAnimDataEval::setAnimData(MtlMap& mtlmap)
		{
			offset_u = mtlmap.animdata.offset_u;
			offset_v = mtlmap.animdata.offset_v;
			tiling_u = mtlmap.animdata.tiling_u;
			tiling_v = mtlmap.animdata.tiling_v;
			angle_u = mtlmap.animdata.angle_u;
			angle_v = mtlmap.animdata.angle_v;
			angle_w = mtlmap.animdata.angle_w;
		}
		void							MtlCtrl::MapAnimDataEval::updateMtlMap(MtlMap& mtlmap, REAL t)
		{
			if(offset_u.anim_data())				mtlmap.uvgen.offset_u = offset_u.sample(t);
			if(offset_v.anim_data())				mtlmap.uvgen.offset_v = offset_v.sample(t);
			if(tiling_u.anim_data())				mtlmap.uvgen.tiling_u = tiling_u.sample(t);
			if(tiling_v.anim_data())				mtlmap.uvgen.tiling_v = tiling_v.sample(t);
			if(angle_u.anim_data())					mtlmap.uvgen.angle_u = angle_u.sample(t);
			if(angle_v.anim_data())					mtlmap.uvgen.angle_v = angle_v.sample(t);
			if(angle_w.anim_data())					mtlmap.uvgen.angle_w = angle_w.sample(t);
		}
		void							MtlCtrl::MapAnimDataEval::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			start = min(start, getStartTime(offset_u, start));
			start = min(start, getStartTime(offset_v, start));
			start = min(start, getStartTime(tiling_u, start));
			start = min(start, getStartTime(tiling_v, start));
			start = min(start, getStartTime(angle_u, start));
			start = min(start, getStartTime(angle_v, start));
			start = min(start, getStartTime(angle_w, start));

			end = -INFINITY;
			end = max(getEndTime(offset_u, end), end);
			end = max(getEndTime(offset_v, end), end);
			end = max(getEndTime(tiling_u, end), end);
			end = max(getEndTime(tiling_v, end), end);
			end = max(getEndTime(angle_u, end), end);
			end = max(getEndTime(angle_v, end), end);
			end = max(getEndTime(angle_w, end), end);
		}
		MtlCtrl::MtlCtrl(const wcs& name, const SPtr<SMeshNode>& node, size_t mtl_index)
			: CtrlBase(name), _mesh_node(node), _mtl_index(mtl_index), _eval_ambient(0), _eval_diffuse(0), _eval_specular(0), _eval_specular_level(0), _eval_glossiness(0), _eval_opacity(0)
		{
		}
		MtlCtrl::~MtlCtrl()
		{
		}
		bool							MtlCtrl::hasAnimData() const
		{
			return true;
		}
		void							MtlCtrl::getAnimDataTimeRange(REAL& start, REAL& end) const
		{
			start = INFINITY;
			start = min(start, getStartTime(_eval_ambient, start));
			start = min(start, getStartTime(_eval_diffuse, start));
			start = min(start, getStartTime(_eval_specular, start));
			start = min(start, getStartTime(_eval_specular_level, start));
			start = min(start, getStartTime(_eval_glossiness, start));
			start = min(start, getStartTime(_eval_opacity, start));
			end = -INFINITY;
			end = max(end, getEndTime(_eval_ambient, end));
			end = max(end, getEndTime(_eval_diffuse, end));
			end = max(end, getEndTime(_eval_specular, end));
			end = max(end, getEndTime(_eval_specular_level, end));
			end = max(end, getEndTime(_eval_glossiness, end));
			end = max(end, getEndTime(_eval_opacity, end));
			if(true)
			{
				REAL start2, end2;
				_eval_map_diffuse.getAnimDataTimeRange(start2, end2);
				start = min(start, start2); end = max(end, end2);
				_eval_map_normal.getAnimDataTimeRange(start2, end2);
				start = min(start, start2); end = max(end, end2);
				_eval_map_selfill.getAnimDataTimeRange(start2, end2);
				start = min(start, start2); end = max(end, end2);
				_eval_map_specular.getAnimDataTimeRange(start2, end2);
				start = min(start, start2); end = max(end, end2);
			}
		}
		AnimCtrl*						MtlCtrl::getAnimCtrl()
		{
			return &_anim_ctrl;
		}
		void							MtlCtrl::update(REAL dt)
		{
			if(!_mesh_node.valid())
				return;

			Z_ASSERT( find(_mesh_node.get_unsafe()->getCtrlSet().getArray().begin(), _mesh_node.get_unsafe()->getCtrlSet().getArray().end(), this) != _mesh_node.get_unsafe()->getCtrlSet().getArray().end());

			_anim_ctrl.advance(dt);

			SPtr<SMeshNode> mn = _mesh_node.lock();

			Mtl& mtl = mn->mesh()->data.mtls.edit()[_mtl_index % mn->mesh()->data.mtls.get().size()];

			REAL t = _anim_ctrl.current_stage().getTime();

			if(getAmbientAnimData())
				mtl.ambient = _eval_ambient.sample(t);
			if(getDiffuseAnimData())
				mtl.diffuse = _eval_diffuse.sample(t);
			if(getSpecularAnimData())
				mtl.specular = _eval_specular.sample(t);
			if(getSpecularLevelAnimData())
				mtl.specular_level = _eval_specular_level.sample(t);
			if(getGlossinessAnimData())
				mtl.glossiness = _eval_glossiness.sample(t);
			if(getOpacityAnimData())
				mtl.opacity = _eval_opacity.sample(t);

			_eval_map_diffuse.updateMtlMap(mtl.map_diffuse, t);
			_eval_map_normal.updateMtlMap(mtl.map_normal, t);
			_eval_map_selfill.updateMtlMap(mtl.map_selfill, t);
			_eval_map_specular.updateMtlMap(mtl.map_specular, t);
		}
		ptrdiff_t						MtlCtrl::getPriority() const
		{
			return 6000;
		}
		bool							MtlCtrl::isUpdateDeferred() const
		{
			return false;
		}
		void							MtlCtrl::setAmbientAnimData(const SPtr< AnimData<Vec3> >& animdata)
		{
			_eval_ambient = animdata;
		}
		void							MtlCtrl::setDiffuseAnimData(const SPtr< AnimData<Vec3> >& animdata)
		{
			_eval_diffuse = animdata;
		}
		void							MtlCtrl::setSpecularAnimData(const SPtr< AnimData<Vec3> >& animdata)
		{
			_eval_specular = animdata;
		}
		void							MtlCtrl::setSpecularLevelAnimData(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_specular_level = animdata;
		}
		void							MtlCtrl::setGlossinessAnimData(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_glossiness = animdata;
		}
		void							MtlCtrl::setOpacityAnimData(const SPtr< AnimData<REAL> >& animdata)
		{
			_eval_opacity = animdata;
		}
		const SPtr< AnimData<Vec3> >&	MtlCtrl::getAmbientAnimData() const
		{
			return _eval_ambient.anim_data();
		}
		const SPtr< AnimData<Vec3> >&	MtlCtrl::getDiffuseAnimData() const
		{
			return _eval_diffuse.anim_data();
		}
		const SPtr< AnimData<Vec3> >&	MtlCtrl::getSpecularAnimData() const
		{
			return _eval_specular.anim_data();
		}
		const SPtr< AnimData<REAL> >&	MtlCtrl::getSpecularLevelAnimData() const
		{
			return _eval_specular_level.anim_data();
		}
		const SPtr< AnimData<REAL> >&	MtlCtrl::getGlossinessAnimData() const
		{
			return _eval_glossiness.anim_data();
		}
		const SPtr< AnimData<REAL> >&	MtlCtrl::getOpacityAnimData() const
		{
			return _eval_opacity.anim_data();
		}
		MtlCtrl::MapAnimDataEval&		MtlCtrl::getDiffuseMapAnimDataEval()
		{
			return _eval_map_diffuse;
		}
		MtlCtrl::MapAnimDataEval&		MtlCtrl::getNormalMapAnimDataEval()
		{
			return _eval_map_normal;
		}
		MtlCtrl::MapAnimDataEval&		MtlCtrl::getSelfillMapAnimDataEval()
		{
			return _eval_map_selfill;
		}
		MtlCtrl::MapAnimDataEval&		MtlCtrl::getSpecularMapAnimDataEval()
		{
			return _eval_map_specular;
		}
	};
};

