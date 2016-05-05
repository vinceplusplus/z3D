
#include "predecl.h"
#include "Mesh.h"

namespace z3D
{
	namespace SG
	{
		// MeshInst can only be used with Mesh, to modify some of attributes, modify public data members, finally call endEdit()
		//class MeshInst: public RefBase
		//{
		//protected:
		//	SPtr<Mesh>				_mesh;
		//public:
		//	vector<Vec3>			array_vert;
		//	vector<Vec3>			array_norm;
		//	vector<Vec2>			array_mvert;
		//	vector<SubMeshInst>		array_submesh;
		//	vector<Mtl>				array_mtl;
		//protected:
		//	AABB					_bound;
		//	int						_dirty_bound;
		//protected:
		//	vector<Plane>			_face_plane;
		//protected:
		//	Vec3					_cached_face_visibility_pov;
		//	vector< char >			_cached_face_visibility;					// actually, sign bit, 0 - visible, 1 - non-visible
		//public:
		//	MeshInst(const SPtr<Mesh>& mesh);
		//public:
		//	void					prepare_vbo_vert(int submesh);
		//	void					prepare_vbo_norm(int submesh);
		//	void					prepare_vbo_diffuse(int submesh);
		//	void					prepare_vbo_normal(int submesh);
		//	void					prepare_vbo_selfill(int submesh);
		//public:
		//	void					loadTextures();								// currently loads all textures
		//public:
		//	const AABB&				local_bound();
		//public:
		//	void					clearFacePlane();
		//	void					buildFacePlane();
		//	const vector<Plane>&	getFacePlane() const;
		//public:
		//	void					computeFaceVisibility(const Vec3& pov);
		//	void					clearFaceVisibility();
		//	const vector<char>&		getFaceVisibility() const;
		//public:
		//	void					endEdit(bool edit_vert,bool edit_norm, bool edit_mvert);
		//public:
		//	const SPtr<Mesh>&		mesh() const;
		//};
	};
};


