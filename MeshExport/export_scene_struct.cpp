
#include "MeshExport.h"

void resetXForm(INode* node);

bool isBoneNode(IGameNode* node)
{
	if(node->GetMaxNode()->GetTMController())
		if(	node->GetMaxNode()->GetTMController()->ClassID()==BIPSLAVE_CONTROL_CLASS_ID||
			node->GetMaxNode()->GetTMController()->ClassID()==BIPBODY_CONTROL_CLASS_ID||
			node->GetMaxNode()->GetTMController()->ClassID()==FOOTPRINT_CLASS_ID)
		return true;
	if(node->GetMaxNode()->EvalWorldState(0).obj->SuperClassID()==HELPER_CLASS_ID)
		return true;
	if(node->GetMaxNode()->EvalWorldState(0).obj->ClassID()==Class_ID(BONE_CLASS_ID,0))
		return true;
	if(node->GetIGameObject()->GetIGameType()==IGameObject::IGAME_BONE)
		return true;
	return false;
}

bool isMeshNode(IGameNode* node)
{
	if(!ExportOptions::export_bone_as_mesh)
	{
		if(node->GetMaxNode()->GetTMController())
			if(	node->GetMaxNode()->GetTMController()->ClassID()==BIPSLAVE_CONTROL_CLASS_ID||
				node->GetMaxNode()->GetTMController()->ClassID()==BIPBODY_CONTROL_CLASS_ID||
				node->GetMaxNode()->GetTMController()->ClassID()==FOOTPRINT_CLASS_ID)
			return false;
		if(node->GetMaxNode()->EvalWorldState(0).obj->SuperClassID()==HELPER_CLASS_ID)
			return false;
		if(node->GetMaxNode()->EvalWorldState(0).obj->ClassID()==Class_ID(BONE_CLASS_ID,0))
			return false;
		if(node->GetIGameObject()->GetIGameType()==IGameObject::IGAME_BONE)
			return false;
	}
	if(node->GetMaxNode()->EvalWorldState(0).obj->SuperClassID()!=GEOMOBJECT_CLASS_ID)
		return false;
	if(!node->GetMaxNode()->EvalWorldState(0).obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID,0)))
		return false;
	return true;
}

bool isLightNode(IGameNode* node)
{
	if(node->GetMaxNode()->EvalWorldState(0).obj->SuperClassID()==LIGHT_CLASS_ID)
	{
		Class_ID classid = node->GetIGameObject()->GetMaxObject()->ClassID();
		if(classid == Class_ID(OMNI_LIGHT_CLASS_ID,0) || classid == Class_ID(DIR_LIGHT_CLASS_ID,0) || classid == Class_ID(TDIR_LIGHT_CLASS_ID,0) || classid == Class_ID(SPOT_LIGHT_CLASS_ID,0) || classid == Class_ID(FSPOT_LIGHT_CLASS_ID,0))
			return true;
	}
	return false;
}

bool isCameraNode(IGameNode* node)
{
	if(node->GetMaxNode()->EvalWorldState(0).obj->SuperClassID()==CAMERA_CLASS_ID)
		return true;
	return false;
}

bool isMeshNodeSkinned(IGameNode* node)
{
	if(!node->GetIGameObject())
		return false;
	IGameMesh* mesh=(IGameMesh*)node->GetIGameObject();
	if(!mesh)
		return false;
	mesh->InitializeData();
	if(!mesh->IsObjectSkinned())
		return false;
	return true;
}

vector<IGameNode*> enumAllNodes()
{
	vector<IGameNode*> traverse_node;
	int i,j;
	for(i=0;i<g_gamescene->GetTopLevelNodeCount();++i)
		traverse_node.push_back(g_gamescene->GetTopLevelNode(i));
	for(i=0;i<(int)traverse_node.size();++i)
		for(j=0;j<(int)traverse_node[i]->GetChildCount();++j)
			traverse_node.push_back(traverse_node[i]->GetNodeChild(j));
	return traverse_node;
}

vector<IGameMaterial*> enumAllMaterial()
{
	vector<IGameMaterial*> traverse_mat;
	int i,j;
	for(i=0;i<g_gamescene->GetRootMaterialCount();++i)
		traverse_mat.push_back(g_gamescene->GetRootMaterial(i));
	for(i=0;i<(int)traverse_mat.size();++i)
		for(j=0;j<(int)traverse_mat[i]->GetSubMaterialCount();++j)
			traverse_mat.push_back(traverse_mat[i]->GetSubMaterial(j));
	return traverse_mat;
}

vector<IGameNode*> enumSceneStructNode()
{
	vector<IGameNode*> export_node;
	vector<IGameNode*> all_node=enumAllNodes();
	int i;
	for(i=0;i<(int)all_node.size();++i)
		if(!all_node[i]->IsNodeHidden())
			export_node.push_back(all_node[i]);
	return export_node;
}

map<INode*, wcs> node_original_name0;

bool checkInvalidNodeNames()
{
	vector<IGameNode*> array_node=enumAllNodes();
	if(true)
	{
		map<INode*,IGameNode*> map_node;
		for(int i=0;i<(int)array_node.size();++i)
			map_node[array_node[i]->GetMaxNode()]=array_node[i];
		array_node.clear();
		map<INode*,IGameNode*>::iterator iter;
		for(iter=map_node.begin();iter!=map_node.end();++iter)
			array_node.push_back(iter->second);
	}
	node_original_name0.clear();

	vector<wcs> invalid_names;
	for(size_t i = 0; i < array_node.size(); ++i)
	{
		INode* node = array_node[i]->GetMaxNode();
		wcs n = MBSTOWCS(node->GetName());

		// the following line must match the one in a later loop that actually renames nodes
		if(n.find('/') != wcs::npos || n.find('\\') != wcs::npos || n.find(':') != wcs::npos || n.find('*') != wcs::npos || n.find('?') != wcs::npos || n.find('\"') != wcs::npos || n.find('<') != wcs::npos || n.find('>') != wcs::npos || n.find('|') != wcs::npos)
			invalid_names.push_back(n);
	}

	if(invalid_names.size())
	{
		wcs msg = L"Warning\n\nOne or more node(s) with a name containing invalid characters (\\ / : * ? \" < > |): \n\n";
		for(size_t i = 0; i < invalid_names.size(); ++i)
			msg += invalid_names[i] + L"\n";
		MessageBoxW(NULL, msg.c_str(), L"Warning", MB_ICONWARNING);
		if(MessageBoxW(NULL, L"The exporter will try to rename the nodes\n\nPlease DO NOT save this .max file after exporting\n\nYou should re-open the .max file and rename them manually", L"Warning", MB_ICONWARNING | MB_OKCANCEL) != IDOK)
			return false;
	}

	for(size_t i = 0; i < array_node.size(); ++i)
	{
		INode* node = array_node[i]->GetMaxNode();
		wcs n = MBSTOWCS(node->GetName());
		if(n.find('/') != wcs::npos || n.find('\\') != wcs::npos || n.find(':') != wcs::npos || n.find('*') != wcs::npos || n.find('?') != wcs::npos || n.find('\"') != wcs::npos || n.find('<') != wcs::npos || n.find('>') != wcs::npos || n.find('|') != wcs::npos)
		{
			if(!node_original_name0.count(node))
				node_original_name0[node] = n;

			n = REPLACE(n, L"\\", L" ");
			n = REPLACE(n, L"/", L" ");
			n = REPLACE(n, L":", L" ");
			n = REPLACE(n, L"*", L" ");
			n = REPLACE(n, L"?", L" ");
			n = REPLACE(n, L"\"", L" ");
			n = REPLACE(n, L"<", L" ");
			n = REPLACE(n, L">", L" ");
			n = REPLACE(n, L"|", L" ");

			node->SetName((char*)WCSTOMBS(n).c_str());
		}
	}
	return true;
}

vector<wcs> findDuplicatedNodeNames(const vector<IGameNode*>& array_node)
{
	vector<wcs> dup_names;
	map<wcs,bool> test_map;
	int i;
	for(i=0;i<(int)array_node.size();++i)
	{
		wcs name=MBSTOWCS(array_node[i]->GetMaxNode()->GetName());
		if(test_map.count(TOLOWER(name)))
			dup_names.push_back(name);
		else
			test_map[TOLOWER(name)]=true;
	}
	return dup_names;
}

map<INode*,wcs> node_original_name;

bool checkDuplicatedNodeNames()
{
	vector<IGameNode*> array_node=enumAllNodes();
	if(true)
	{
		map<INode*,IGameNode*> map_node;
		for(int i=0;i<(int)array_node.size();++i)
			map_node[array_node[i]->GetMaxNode()]=array_node[i];
		array_node.clear();
		map<INode*,IGameNode*>::iterator iter;
		for(iter=map_node.begin();iter!=map_node.end();++iter)
			array_node.push_back(iter->second);
	}
	vector<wcs> dup_names = findDuplicatedNodeNames(array_node);
	node_original_name.clear();
	while(dup_names.size())
	{
		if(dup_names.size())
		{
			wcs msg = L"Warning\nfound the following duplicated node names:\n\n";
			int i;
			for(i=0;i<(int)dup_names.size();++i)
				msg+=dup_names[i]+L"\n";
			MessageBoxW(NULL, msg.c_str(), L"Duplicated Node Names", MB_ICONWARNING);
			if(MessageBoxW(NULL, L"The exporter will try to rename the nodes\n\nPlease DO NOT save this .max file after exporting\n\nYou should re-open the .max file and rename them manually", L"Warning", MB_ICONWARNING | MB_OKCANCEL) != IDOK)
				return false;
		}
		int i,j;
		for(i=0;i<(int)dup_names.size();++i)
		{
			int count=0;
			for(j=0;j<(int)array_node.size();++j)
			{
				if(TOLOWER(MBSTOWCS(array_node[j]->GetMaxNode()->GetName())) == TOLOWER(dup_names[i]))
				{
					++count;
					if(count>1)
					{
						if(node_original_name.count(array_node[j]->GetMaxNode())==0)
							node_original_name[array_node[j]->GetMaxNode()]=MBSTOWCS(array_node[j]->GetMaxNode()->GetName());
						char name_buffer[4096];
						strcpy_s(name_buffer, FORMATA("%s_DN%03d", array_node[j]->GetMaxNode()->GetName(), count).c_str());
						array_node[j]->GetMaxNode()->SetName(name_buffer);
					}
				}
			}
		}
		dup_names = findDuplicatedNodeNames(array_node);
	}

	return true;
}

map<Mtl*, wcs> material_original_name0;

bool checkInvalidMaterialNames()
{
	vector<Mtl*> array_material;
	if(true)
	{
		vector<IGameNode*> array_node = enumAllNodes();
		for(size_t i = 0; i < array_node.size(); ++i)
		{
			IGameNode* gamenode = array_node[i];
			if(gamenode && getNodeExportMaterial(gamenode))
			{
				Mtl* mtl = getNodeExportMaterial(gamenode);
				size_t j;
				for(j = 0; j < array_material.size(); ++j)
					if(array_material[j] == mtl)
						break;
				if(j == array_material.size())
					array_material.push_back(mtl);
			}
		}
	}
	material_original_name0.clear();

	bool first = true;

	for(size_t i = 0; i < array_material.size(); ++i)
	{
		Mtl* mtl = array_material[i];
		wcs n = MBSTOWCS(mtl->GetName().data());
		if(n.find('/') != wcs::npos || n.find('\\') != wcs::npos || n.find(':') != wcs::npos || n.find('*') != wcs::npos || n.find('?') != wcs::npos || n.find('\"') != wcs::npos || n.find('<') != wcs::npos || n.find('>') != wcs::npos || n.find('|') != wcs::npos)
		{
			if(first)
			{
				first = false;

				MessageBoxW(NULL, L"Warning\n\nOne or more material(s) with a name containing invalid characters (\\ / : * ? \" < > |)", L"Warning", MB_ICONWARNING);
				if(MessageBoxW(NULL, L"The exporter will try to rename the materials\n\nPlease DO NOT save this .max file after exporting\n\nYou should re-open the .max file and rename them manually", L"Warning", MB_ICONWARNING | MB_OKCANCEL) != IDOK)
					return false;
			}

			if(!material_original_name0.count(mtl))
				material_original_name0[mtl] = n;

			n = REPLACE(n, L"\\", L" ");
			n = REPLACE(n, L"/", L" ");
			n = REPLACE(n, L":", L" ");
			n = REPLACE(n, L"*", L" ");
			n = REPLACE(n, L"?", L" ");
			n = REPLACE(n, L"\"", L" ");
			n = REPLACE(n, L"<", L" ");
			n = REPLACE(n, L">", L" ");
			n = REPLACE(n, L"|", L" ");

			mtl->SetName(WCSTOMBS(n).c_str());
		}
	}
	return true;
}

vector<wcs> findDuplicatedMaterialNames(const vector<Mtl*>& array_material)
{
	vector<wcs> dup_names;
	map<wcs,bool> test_map;
	int i;
	for(i=0;i<(int)array_material.size();++i)
	{
		wcs name = MBSTOWCS(array_material[i]->GetName().data());

		if(test_map.count(TOLOWER(name)))
			dup_names.push_back(name);
		else
			test_map[TOLOWER(name)] = true;
	}
	return dup_names;
}

map<Mtl*,wcs> material_original_name;

bool checkDuplicatedMaterialNames()
{
	vector<Mtl*> array_material;
	if(true)
	{
		vector<IGameNode*> array_node = enumAllNodes();
		for(size_t i = 0; i < array_node.size(); ++i)
		{
			IGameNode* gamenode = array_node[i];
			if(gamenode && getNodeExportMaterial(gamenode))
			{
				Mtl* mtl = getNodeExportMaterial(gamenode);
				size_t j;
				for(j = 0; j < array_material.size(); ++j)
					if(array_material[j] == mtl)
						break;
				if(j == array_material.size())
					array_material.push_back(mtl);
			}
		}
	}
	material_original_name.clear();

	vector<wcs> dup_names = findDuplicatedMaterialNames(array_material);

	while(dup_names.size())
	{
		if(dup_names.size())
		{
			wcs msg = L"Warning\nfound the following duplicated material names:\n\n";
			int i;
			for(i=0;i<(int)dup_names.size();++i)
				msg+=dup_names[i]+L"\n";
			MessageBoxW(NULL, msg.c_str(), L"Duplicated Material Names", MB_ICONWARNING);
			if(MessageBoxW(NULL, L"The exporter will try to rename the materials\n\nPlease DO NOT save this .max file after exporting\n\nYou should re-open the .max file and rename them manually", L"Warning", MB_ICONWARNING | MB_OKCANCEL) != IDOK)
				return false;
		}
		int i,j;
		for(i=0;i<(int)dup_names.size();++i)
		{
			int count=0;
			for(j=0;j<(int)array_material.size();++j)
			{
				if(TOLOWER(MBSTOWCS(array_material[j]->GetName().data())) == TOLOWER(dup_names[i]))
				{
					++count;
					if(count>1)
					{
						if(material_original_name.count(array_material[j]) == 0)
							material_original_name[array_material[j] ] =MBSTOWCS(array_material[j]->GetName().data());
						mbs new_name = FORMATA("%s_DN%03d",array_material[j]->GetName().data(),count);
						array_material[j]->SetName(new_name.c_str());
						//if(string(array_material[j]->GetMaxMaterial()->GetName().data())!=new_name)
							//MessageBox(NULL,FORMATA("shit: %s %s",array_material[j]->GetMaxMaterial()->GetName().data(),new_name.c_str()).c_str(),"",0);
					}
				}
			}
		}
		dup_names = findDuplicatedMaterialNames(array_material);
	}
	return true;
}

void rollbackRenamedNodeNames0()
{
	map<INode*,wcs>::iterator iter;
	for(iter=node_original_name0.begin();iter!=node_original_name0.end();++iter)
		iter->first->SetName((TCHAR*)WCSTOMBS(iter->second).c_str());
	node_original_name0.clear();
}

void rollbackRenamedNodeNames()
{
	map<INode*,wcs>::iterator iter;
	for(iter=node_original_name.begin();iter!=node_original_name.end();++iter)
		iter->first->SetName((TCHAR*)WCSTOMBS(iter->second).c_str());
	node_original_name.clear();
}

void rollbackRenamedMaterialNames0()
{
	map<Mtl*,wcs>::iterator iter;
	for(iter=material_original_name0.begin();iter!=material_original_name0.end();++iter)
		iter->first->SetName((TCHAR*)WCSTOMBS(iter->second).c_str());
	material_original_name0.clear();
}

void rollbackRenamedMaterialNames()
{
	map<Mtl*,wcs>::iterator iter;
	for(iter=material_original_name.begin();iter!=material_original_name.end();++iter)
		iter->first->SetName((TCHAR*)WCSTOMBS(iter->second).c_str());
	material_original_name.clear();
}

void sortForSkinnedInstances(vector<IGameNode*>& array_node)
{
	if(!array_node.size())
		return;

	vector<IGameSkin*> skinned_instances;				// actually can use set, but want to preserve the order

	for(size_t i = array_node.size() - 1; (ptrdiff_t)i >= 0 ; --i)
	{
		IGameNode* gamenode = array_node[i];
		INode* node = gamenode->GetMaxNode();
		Object* obj = node->GetObjOrWSMRef();
		IGameSkin* gameskin = (gamenode->GetIGameObject() && gamenode->GetIGameObject()) ? gamenode->GetIGameObject()->GetIGameSkin() : NULL;
		Modifier* skin_mod = gameskin ? gameskin->GetMaxModifier() : NULL;

		if(!gameskin)
			continue;

		Tab<INode*> nodetab;
		gameskin->EffectedNodes(nodetab);

		// log abuse
		for(int j = 0; j < nodetab.Count(); ++j)
		{
			INode* n = nodetab[j];
			if(n != node && n->GetObjOrWSMRef() == obj)
			{
				bool found = false;
				for(size_t k = 0; k < skinned_instances.size(); ++k)
				{
					if(skinned_instances[k]->GetMaxModifier() != skin_mod)
						continue;
					found = true;
					break;
				}
				if(!found)
					skinned_instances.push_back(gameskin);
			}
		}

		// perform sorting
		for(int j = 0; j < nodetab.Count(); ++j)
		{
			INode* n = nodetab[j];
			if(n->GetObjOrWSMRef() != obj)		// not the same instance
				continue;
			if(n == node)						// if thats the first node found, no changes needed
				break;
			for(size_t k = i + 1; k < array_node.size(); ++k)
				if(array_node[k]->GetMaxNode() == n)
				{
					array_node.erase(array_node.begin() + i);
					array_node.insert(array_node.begin() + k, gamenode);
					break;
				}
			break;
		}
	}

	if(!skinned_instances.size())
		return;

	mbs s = "Skinned instances are found, they can be very problematic in MAX. Nodes: \n\n";

	for(size_t i = 0; i < skinned_instances.size(); ++i)
	{
		Tab<INode*> nodetab;
		skinned_instances[i]->EffectedNodes(nodetab);

		for(int j = 0; j < nodetab.Count(); ++j)
		{
			INode* node = nodetab[j];
			Object* obj = node->GetObjOrWSMRef();
			for(int k = 0; k < nodetab.Count(); ++k)
			{
				if(j == k)
					continue;

				if(nodetab[k]->GetObjOrWSMRef() != obj)
					continue;

				s += node->GetName();
				s += "\r\n";
				break;
			}
		}
		s += "\r\n";
	}

	MessageBoxA(NULL, s.c_str(), "Warning", 0);
}

struct SKINNED_MESH
{
	INode*				node;
	wcs					filepath;
};

bool exportSceneStruct(vector<IGameNode*> array_node, TiXmlDocument& doc, vector<SKINNED_MESH>& array_skinned_mesh, KFWriter& kfwriter, const wcs& animdata_misc_path)
{
	clearMeshNodeCache();

	sortForSkinnedInstances(array_node);

	doc.InsertEndChild(TiXmlDeclaration("1.0", "utf-8", ""));

	TiXmlHandle root(doc.InsertEndChild(TiXmlElement("scene")));

	TiXmlElement& an = *(TiXmlElement*)root.ToElement()->InsertEndChild(TiXmlElement("nodes"));
	TiXmlElement& hi = *(TiXmlElement*)root.ToElement()->InsertEndChild(TiXmlElement("links"));

	UniqueMap<IGameNode*> node_map = UniqueMap<IGameNode*>::fromVector(array_node);

	TimeValue cur_time = GetCOREInterface()->GetTime();

	size_t chunk_idx;

	set<wcs> lowered_used_names;

	for(size_t i = 0; i < array_node.size(); ++i)
	{
		IGameNode* node = array_node[i];

		//syslog << FORMATA("exportSceneStruct: node \"%s\"", node->GetName());

		lowered_used_names.insert(TOLOWER(MBSTOWCS(node->GetName())));

		Mat4 m = getLocalTM(node, cur_time);
		Vec3 pos, scale;
		z3D::Core::Quat rot;
		m.factorSRT(scale, rot, pos);

		if(true)
		{
			if(false)
			{
				// oh my god... this method is not stable
				z3D::Core::Quat so, r;
				Vec3 s, t;
				m.decompose(so, s, r, t);

				Mat3 delta = so.asMat3();

				if(!delta.testIdentity(0.01f))
				{
					syslog << FORMATA("warning: \"%s\": shearing component detected (scale orientation: %f %f %f %f %f %f %f %f %f) !!! look out for shearing components, they are not supported in the engine", node->GetName()
						, delta[0][0], delta[0][1], delta[0][2]
						, delta[1][0], delta[1][1], delta[1][2]
						, delta[2][0], delta[2][1], delta[2][2]
						);
				}
			}
			else
			{
				Mat4 delta = m.inverse() * (Mat4::scaling(scale) * rot.asMat4() * Mat4::translation(pos));
				if(!delta.testIdentity(0.1f))
				{
					syslog << FORMATA("warning: \"%s\": re-composed SRT is not nearly equal to the original (delta: %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f) !!! look out for shearing components, they are not supported in the engine", node->GetName()
						, delta[0][0], delta[0][1], delta[0][2], delta[0][3]
						, delta[1][0], delta[1][1], delta[1][2], delta[1][3]
						, delta[2][0], delta[2][1], delta[2][2], delta[2][3]
						, delta[3][0], delta[3][1], delta[3][2], delta[3][3]
						);
				}
			}
		}

		bool cast_shadow = node->GetMaxNode()->CastShadows() ? true: false;
		bool recv_shadow = node->GetMaxNode()->RcvShadows() ? true: false;

		zTiXmlElement& e = *(zTiXmlElement*)an.InsertEndChild(TiXmlElement("node"));
		e.SetAttribute("name", MBSTOWCS(node->GetName()));
		e.SetAttribute("pos", pos);
		e.SetAttribute("rot", rot);
		e.SetAttribute("scale", scale);
		e.SetAttribute("cast_shadow", cast_shadow);
		e.SetAttribute("recv_shadow", recv_shadow);

		if(isMeshNode(node))
		{
			wcs src, mtl, skin;
			EXPORTED_MORPHER exported_morpher;

			//syslog << "exportMeshNode()";
			bool ret = exportMeshNode(node, src, mtl, exported_morpher, skin, kfwriter, animdata_misc_path);
			if(ret)
			{
				e.SetAttribute("type", "mesh");

				zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("mesh"));
				e2.SetAttribute("src", src);
				e2.SetAttribute("mtl", mtl);

				if(exported_morpher.node)
				{
					zTiXmlElement& e3 = *(zTiXmlElement*)e2.InsertEndChild(TiXmlElement("morpher"));
					e3.SetAttribute("src", exported_morpher.filepath);
					e3.SetAttribute("channel_count", (int32_t)exported_morpher.chans.size());
					const vector<pair<float, size_t> >& chans = exported_morpher.chans;
					for(size_t i = 0; i < chans.size(); ++i)
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("channel"));
						e4.SetAttribute("weight", chans[i].first);
						if(chans[i].second != ~((size_t)0))
						{
							zTiXmlElement& e5 = *(zTiXmlElement*)e4.InsertEndChild(TiXmlElement("animdata"));
							zTiXmlElement& e6 = *(zTiXmlElement*)e5.InsertEndChild(TiXmlElement("weight"));
							e6.SetAttribute("src", animdata_misc_path);
							e6.SetAttribute("chunk", (int32_t)chans[i].second);
						}
					}
				}

				if(skin.length())
				{
					SKINNED_MESH s = {node->GetMaxNode(), skin};

					array_skinned_mesh.push_back(s);
				}
			}
			else
			{
				e.SetAttribute("type", "node");
			}
		}
		else if(isLightNode(node))
		{
			e.SetAttribute("type", "light");

			zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("light"));

			IGameLight* light = (IGameLight*)node->GetIGameObject();
			light->InitializeData();
			IGameProperty* prop = NULL;
			Point3 color2(1, 1, 1);
			prop=light->GetLightColor();
			if(prop)
				prop->GetPropertyValue(color2);
			Vec3 color(color2[0], color2[1], color2[2]);
			REAL multiplier = 1.0f, radius = 40.0f;
			prop = light->GetLightMultiplier();
			if(prop)
				prop->GetPropertyValue(multiplier);
			prop = light->GetLightAttenStart();
			if(prop)
				prop->GetPropertyValue(radius);
			Class_ID classid = node->GetIGameObject()->GetMaxObject()->ClassID();
			bool ambient_only = false;
			if(classid == Class_ID(OMNI_LIGHT_CLASS_ID, 0) || classid == Class_ID(DIR_LIGHT_CLASS_ID, 0) || classid == Class_ID(TDIR_LIGHT_CLASS_ID, 0) || classid == Class_ID(SPOT_LIGHT_CLASS_ID, 0) || classid == Class_ID(FSPOT_LIGHT_CLASS_ID, 0))
				ambient_only = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetAmbientOnly() ? true : false;
			if(ambient_only)
			{
				REAL radius = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(cur_time);
				bool enabled = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight() ? true : false;

				e2.SetAttribute("type", "ambient");
				e2.SetAttribute("color", color);
				e2.SetAttribute("multiplier", multiplier);
				e2.SetAttribute("radius", radius);
				e2.SetAttribute("enabled", enabled);
			}
			else if(classid == Class_ID(DIR_LIGHT_CLASS_ID,0) || classid == Class_ID(TDIR_LIGHT_CLASS_ID, 0))
			{
				REAL radius = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(cur_time);
				REAL beam = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetHotspot(cur_time);
				REAL dist = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetTDist(cur_time);
				bool enabled = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight() ? true : false;
				bool cast_shadow = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow() ? true : false;

				e2.SetAttribute("type", "dir");
				e2.SetAttribute("color", color);
				e2.SetAttribute("multiplier", multiplier);
				e2.SetAttribute("radius", radius);
				e2.SetAttribute("beam", beam);
				e2.SetAttribute("dist", dist);
				e2.SetAttribute("enabled", enabled);
				e2.SetAttribute("cast_shadow", cast_shadow);
			}
			else if(classid == Class_ID(OMNI_LIGHT_CLASS_ID, 0))
			{
				REAL radius = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(cur_time);
				bool enabled = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight() ? true : false;
				bool cast_shadow = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow() ? true : false;

				e2.SetAttribute("type", "point");
				e2.SetAttribute("color", color);
				e2.SetAttribute("multiplier", multiplier);
				e2.SetAttribute("radius", radius);
				e2.SetAttribute("enabled", enabled);
				e2.SetAttribute("cast_shadow", cast_shadow);
			}
			else if(classid == Class_ID(SPOT_LIGHT_CLASS_ID, 0) || classid == Class_ID(FSPOT_LIGHT_CLASS_ID, 0))
			{
				REAL radius = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(cur_time);
				REAL inner_angle = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetHotspot(cur_time);
				REAL outer_angle = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetFallsize(cur_time);
				bool enabled = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight() ? true : false;
				bool cast_shadow = ((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow() ? true : false;

				e2.SetAttribute("type", "spot");
				e2.SetAttribute("color", color);
				e2.SetAttribute("multiplier", multiplier);
				e2.SetAttribute("radius", radius);
				e2.SetAttribute("inner_cone", inner_angle);
				e2.SetAttribute("outer_cone", outer_angle);
				e2.SetAttribute("enabled", enabled);
				e2.SetAttribute("cast_shadow", cast_shadow);
			}
			if(true)
			{
				zTiXmlElement& e3 = *(zTiXmlElement*)e2.InsertEndChild(TiXmlElement("animdata"));

				Animatable* animatable = NULL;
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(), L"Parameters"), L"Color");
				if(animatable)
				{
					class getter: public KeyGetter<Vec3>
					{
					public:
						GenLight*				light;
						getter(GenLight* light_): light(light_) {}
						Vec3					get(TimeValue t)		{	return toVec3(light->GetRGBColor(t));	}
					};
					if(kfwriter.exportChunk(animatable, getter((GenLight*)light->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("color"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(), L"Parameters"), L"Multiplier");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						GenLight*				light;
						getter(GenLight* light_): light(light_) {}
						REAL					get(TimeValue t)		{	return light->GetIntensity(t);	}
					};
					if(kfwriter.exportChunk(animatable, getter((GenLight*)light->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("multiplier"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(), L"Parameters"), L"Decay Falloff");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						GenLight*				light;
						getter(GenLight* light_): light(light_) {}
						REAL					get(TimeValue t)		{	return light->GetDecayRadius(t);	}
					};
					if(kfwriter.exportChunk(animatable, getter((GenLight*)light->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("radius"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(), L"Parameters"), L"Hotspot");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						GenLight*				light;
						getter(GenLight* light_): light(light_) {}
						REAL					get(TimeValue t)		{	return light->GetHotspot(t);	}
					};
					if(kfwriter.exportChunk(animatable, getter((GenLight*)light->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("inner_cone"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(), L"Parameters"), L"Falloff");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						GenLight*				light;
						getter(GenLight* light_): light(light_) {}
						REAL					get(TimeValue t)		{	return light->GetFallsize(t);	}
					};
					if(kfwriter.exportChunk(animatable, getter((GenLight*)light->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("outer_cone"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				if(!e3.FirstChildElement())
					e2.RemoveChild(&e3);
			}
			node->ReleaseIGameObject();
		}
		else if(isCameraNode(node))
		{
			e.SetAttribute("type", "camera");

			IGameCamera* camera = (IGameCamera*)node->GetIGameObject();
			camera->InitializeData();

			REAL fov = 90, near_clip = 1, far_clip = 1000, target_dist = 100;
			IGameProperty* prop = NULL;

			prop = camera->GetCameraFOV();
			if(prop)
				prop->GetPropertyValue(fov);
			fov = fov * 180 / z3D::Core::PI;
			fov = ATAN(1.0f / (g_interface->GetRendImageAspect() / TAN(fov * 0.5f))) * 2;

			prop = camera->GetCameraNearClip();
			if(prop)
				prop->GetPropertyValue(near_clip);

			prop = camera->GetCameraFarClip();
			if(prop)
				prop->GetPropertyValue(far_clip);

			prop = camera->GetCameraTargetDist();
			if(prop)
				prop->GetPropertyValue(target_dist);

			zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("camera"));
			e2.SetAttribute("fov", fov);
			e2.SetAttribute("near", near_clip);
			e2.SetAttribute("far", far_clip);
			e2.SetAttribute("target_dist", target_dist);
			e2.SetAttribute("ortho", ((CameraObject*)camera->GetMaxObject())->IsOrtho());

			if(true)
			{
				zTiXmlElement& e3 = *(zTiXmlElement*)e2.InsertEndChild(TiXmlElement("animdata"));

				//listAnimatables(getSubAnim(camera->GetMaxObject(), L"Parameters"));

				Animatable* animatable = NULL;
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(), L"Parameters"), L"FOV");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						CameraObject*			cam;
						getter(CameraObject* cam_): cam(cam_) {}
						REAL					get(TimeValue t)
						{
							REAL fov = cam->GetFOV(t);
							fov = fov * 180 / z3D::Core::PI;
							fov = ATAN(1.0f / (GetCOREInterface()->GetRendImageAspect() / TAN(fov * 0.5f))) * 2.0f;
							return fov;
						}
					};
					if(kfwriter.exportChunk(animatable, getter((CameraObject*)camera->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("fov"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(), L"Parameters"), L"Near Clip");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						CameraObject*			cam;
						getter(CameraObject* cam_): cam(cam_) {}
						REAL					get(TimeValue t)		{	return cam->GetClipDist(t, CAM_HITHER_CLIP);	}
					};
					if(kfwriter.exportChunk(animatable, getter((CameraObject*)camera->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("near"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(), L"Parameters"), L"Far Clip ");							// very weird, indeed, the version i work on has a trailing space
				if(!animatable)
					animatable = getSubAnim(getSubAnim(camera->GetMaxObject(), L"Parameters"), L"Far Clip");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						CameraObject*			cam;
						getter(CameraObject* cam_): cam(cam_) {}
						REAL					get(TimeValue t)		{	return cam->GetClipDist(t, CAM_YON_CLIP);	}
					};
					if(kfwriter.exportChunk(animatable, getter((CameraObject*)camera->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("far"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(), L"Parameters"), L"Target Distance");
				if(animatable)
				{
					class getter: public KeyGetter<REAL>
					{
					public:
						CameraObject*			cam;
						getter(CameraObject* cam_): cam(cam_) {}
						REAL					get(TimeValue t)		{	return cam->GetTDist(t);	}
					};
					if(kfwriter.exportChunk(animatable, getter((CameraObject*)camera->GetMaxObject()), chunk_idx))
					{
						zTiXmlElement& e4 = *(zTiXmlElement*)e3.InsertEndChild(TiXmlElement("target_dist"));
						e4.SetAttribute("src", animdata_misc_path);
						e4.SetAttribute("chunk", (int32_t)chunk_idx);
					}
				}

				// if no animatable exported then remove the node
				if(!e3.FirstChildElement())
					e2.RemoveChild(&e3);
			}

			node->ReleaseIGameObject();
		}
		else if(isBoneNode(node))
		{
			e.SetAttribute("type", "bone");

			Box3 bb;
			node->GetIGameObject()->GetMaxObject()->GetDeformBBox(cur_time, bb, NULL);
			Matrix3 trans;
			trans.SetRotateX(-PI * 0.5f);
			Matrix3 objtm = node->GetMaxNode()->GetObjectTM(0);
			Matrix3 nodetm = node->GetMaxNode()->GetNodeTM(0);
			nodetm.Invert();
			objtm *= nodetm;
			bb = bb * objtm;
			bb = bb * trans;

			Vec3 mini(bb.pmin.x, bb.pmin.y, bb.pmin.z);
			Vec3 maxi(bb.pmax.x, bb.pmax.y, bb.pmax.z);

			zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("bone"));
			e2.SetAttribute("min", mini);
			e2.SetAttribute("max", maxi);
		}
		else
		{
			e.SetAttribute("type", "node");
		}
		IGameNode* parent = node->GetNodeParent();
		if(parent)
		{
			if(node_map[parent])
			{
				zTiXmlElement& e = *(zTiXmlElement*)hi.InsertEndChild(TiXmlElement("link"));
				e.SetAttribute("child", MBSTOWCS(node->GetName()));
				e.SetAttribute("parent", MBSTOWCS(parent->GetName()));
			}
		}
	}

	if(ExportOptions::export_current_view)
	{
		if(!lowered_used_names.count(L"camera01"))
		{
			ViewExp* current_view = GetCOREInterface()->GetActiveViewport();
			if(current_view)
			{
				if(true)
				{
					Mat4 cam_tm;
					if(true)
					{
						Matrix3 tm;
						current_view->GetAffineTM(tm);
						cam_tm = ROTXNEG90_4 * (ROTX90_4 * toMat4(tm).inverse() * ROTXNEG90_4);
					}
					Vec3 pos;
					z3D::Core::Quat rot;
					Vec3 scl;
					cam_tm.factorSRT(scl, rot, pos);

					zTiXmlElement& e = *(zTiXmlElement*)an.InsertEndChild(TiXmlElement("node"));
					e.SetAttribute("name", "Camera01");
					e.SetAttribute("pos", pos);
					e.SetAttribute("rot", rot);
					e.SetAttribute("scale", scl);
					e.SetAttribute("cast_shadow", false);
					e.SetAttribute("recv_shadow", false);
					e.SetAttribute("type", "camera");

					REAL fov = current_view->GetFOV();
					fov = fov * 180 / z3D::Core::PI;
					fov = ATAN(1.0f / (g_interface->GetRendImageAspect() / TAN(fov * 0.5f))) * 2;

					zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("camera"));
					e2.SetAttribute("fov", fov);
					e2.SetAttribute("near", 1.0f);
					e2.SetAttribute("far", 50000.0f);
					e2.SetAttribute("target_dist", current_view->GetFocalDist());
					e2.SetAttribute("ortho", !current_view->IsPerspView());

					if(current_view->GetViewCamera())
					{
						INode* n = current_view->GetViewCamera();
						const ObjectState& s = n->EvalWorldState(GetCOREInterface()->GetTime());
						if(s.obj && s.obj->SuperClassID() == CAMERA_CLASS_ID)
						{
							e2.SetAttribute("near", ((CameraObject*)s.obj)->GetClipDist(GetCOREInterface()->GetTime(), CAM_HITHER_CLIP));
							e2.SetAttribute("far", ((CameraObject*)s.obj)->GetClipDist(GetCOREInterface()->GetTime(), CAM_YON_CLIP));
						}
					}
				}

				if(ExportOptions::add_helper_lights)
				{
					if(!lowered_used_names.count(L"tempambientlight") && !lowered_used_names.count(L"temppointlight"))
					{
						if(true)
						{
							zTiXmlElement& e = *(zTiXmlElement*)an.InsertEndChild(TiXmlElement("node"));
							e.SetAttribute("name", "TempAmbientLight");
							e.SetAttribute("pos", Vec3(0, 0, 0));
							e.SetAttribute("rot", z3D::Core::Quat(Mat3::identity));
							e.SetAttribute("scale", Vec3(1, 1, 1));
							e.SetAttribute("cast_shadow", false);
							e.SetAttribute("recv_shadow", false);
							e.SetAttribute("type", "light");

							zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("light"));
							e2.SetAttribute("type", "ambient");
							e2.SetAttribute("color", Vec3(1, 1, 1));
							e2.SetAttribute("multiplier", 0.4f);
							e2.SetAttribute("radius", 50000.0f);
							e2.SetAttribute("enabled", true);
						}
						if(true)
						{
							zTiXmlElement& e = *(zTiXmlElement*)an.InsertEndChild(TiXmlElement("node"));
							e.SetAttribute("name", "TempPointLight");
							e.SetAttribute("pos", Vec3(0, 0, 0));
							e.SetAttribute("rot", z3D::Core::Quat(Mat3::identity));
							e.SetAttribute("scale", Vec3(1, 1, 1));
							e.SetAttribute("cast_shadow", false);
							e.SetAttribute("recv_shadow", false);
							e.SetAttribute("type", "light");

							zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("light"));
							e2.SetAttribute("type", "point");
							e2.SetAttribute("color", Vec3(1, 1, 1));
							e2.SetAttribute("multiplier", 1.0f);
							e2.SetAttribute("radius", 50000.0f);
							e2.SetAttribute("enabled", true);
							e2.SetAttribute("cast_shadow", false);
						}

						if(true)
						{
							zTiXmlElement& e = *(zTiXmlElement*)hi.InsertEndChild(TiXmlElement("link"));
							e.SetAttribute("child", "TempAmbientLight");
							e.SetAttribute("parent", "Camera01");
						}
						if(true)
						{
							zTiXmlElement& e = *(zTiXmlElement*)hi.InsertEndChild(TiXmlElement("link"));
							e.SetAttribute("child", "TempPointLight");
							e.SetAttribute("parent", "Camera01");
						}
					}
				}
			}
		}
	}

	return true;
}

#if 0

bool exportSceneStruct(const vector<IGameNode*> array_node,MemWriter& writer)
{

	writer.clear();

	wcs output;
	int i;
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];
		/*
		if(!isCameraNode(node))
			output+=FORMATW(L"create_node \"%s\"\r\n",(MBSTOWCS(node->GetName())+L"_tm").c_str());*/

		if(isMeshNode(node))
		{
			if(ExportOptions::text_version && false)
				output+=FORMATW(L"create_mesh_node \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),ESCSTR(ExportOptions::mesh_dir+L"/"+L"mesh_"+MBSTOWCS(node->GetName())+L".txt").c_str());
			else
				output+=FORMATW(L"create_mesh_node \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),ESCSTR(ExportOptions::mesh_dir+L"/"+L"mesh_"+MBSTOWCS(node->GetName())+L".bin").c_str());
		}
		else if(isLightNode(node))
		{
			IGameLight* light=(IGameLight*)node->GetIGameObject();
			light->InitializeData();
			IGameProperty* prop=NULL;
			Point3 color(1,1,1);
			prop=light->GetLightColor();
			if(prop)
				prop->GetPropertyValue(color);
			REAL multiplier=1.0f,radius=40.0f;
			prop=light->GetLightMultiplier();
			if(prop)
				prop->GetPropertyValue(multiplier);
			prop=light->GetLightAttenStart();
			if(prop)
				prop->GetPropertyValue(radius);
			bool enabled=true;
			bool cast_shadow=true;
			if(node->GetIGameObject()->GetMaxObject()->ClassID()==Class_ID(OMNI_LIGHT_CLASS_ID,0))
			{
				radius=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(0);
				enabled=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight()?true:false;
				cast_shadow=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow()?true:false;
				bool ambient_only=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetAmbientOnly()?true:false;

				if(ambient_only)
				{
					output+=FORMATW(L"create_ambient_light_node \"%s\" %f %f %f %f %d\r\n",
						MBSTOWCS(node->GetName()).c_str(),
						color.x,color.y,color.z,
						multiplier,
						enabled?1:0);
				}
				else
				{
					output+=FORMATW(L"create_point_light_node \"%s\" %f %f %f %f %f %d %d\r\n",
						MBSTOWCS(node->GetName()).c_str(),
						color.x,color.y,color.z,
						multiplier,
						radius,
						enabled?1:0,
						cast_shadow?1:0);
				}
			}
			else if(node->GetIGameObject()->GetMaxObject()->ClassID()==Class_ID(DIR_LIGHT_CLASS_ID,0) || node->GetIGameObject()->GetMaxObject()->ClassID()==Class_ID(TDIR_LIGHT_CLASS_ID,0))
			{
				REAL beam=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetHotspot(0);
				REAL dist=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(0);
				enabled=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight()?true:false;
				cast_shadow=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow()?true:false;

				output+=FORMATW(L"create_dir_light_node \"%s\" %f %f %f %f %f %f %d %d\r\n",
					MBSTOWCS(node->GetName()).c_str(),
					color.x,color.y,color.z,
					multiplier,
					beam,
					dist,
					enabled?1:0,
					cast_shadow?1:0);
			}
			else if(node->GetIGameObject()->GetMaxObject()->ClassID()==Class_ID(SPOT_LIGHT_CLASS_ID,0))
			{
				radius=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetDecayRadius(0);
				enabled=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetUseLight()?true:false;
				cast_shadow=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetShadow()?true:false;
				REAL inner_angle=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetHotspot(0);
				REAL outer_angle=((GenLight*)node->GetIGameObject()->GetMaxObject())->GetFallsize(0);

				output+=FORMATW(L"create_spot_light_node \"%s\" %f %f %f %f %f %f %f %d %d\r\n",
					MBSTOWCS(node->GetName()).c_str(),
					color.x,color.y,color.z,
					multiplier,
					radius,
					inner_angle,
					outer_angle,
					enabled?1:0,
					cast_shadow?1:0);
			}
			else
			{
				output+=FORMATW(L"create_node \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str());
			}
			//listAnimatables(getSubAnim(light->GetMaxObject(),L"Parameters"));
			if(true)
			{
				Animatable* animatable=NULL;
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(),L"Parameters"),L"Color");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:GenLight*					light;public:exporter(GenLight* light2):light(light2) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							Vec3 p=toVec3(light->GetRGBColor(timevalue));
							writer.write(p.x);writer.write(p.y);writer.write(p.z);	}};

					wcs name=wcs(L"light_color_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((GenLight*)light->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_light_node_color_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(),L"Parameters"),L"Multiplier");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:GenLight*					light;public:exporter(GenLight* light2):light(light2) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(light->GetIntensity(timevalue));	}};

					wcs name=wcs(L"light_multiplier_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((GenLight*)light->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_light_node_multiplier_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(),L"Parameters"),L"Decay Falloff");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:GenLight*					light;public:exporter(GenLight* light2):light(light2) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(light->GetDecayRadius(timevalue));	}};

					wcs name=wcs(L"light_radius_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((GenLight*)light->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_light_node_radius_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(),L"Parameters"),L"Hotspot");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:GenLight*					light;public:exporter(GenLight* light2):light(light2) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(light->GetHotspot(timevalue));	}};

					wcs name=wcs(L"light_inner_angle_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((GenLight*)light->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_light_node_inner_angle_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(light->GetMaxObject(),L"Parameters"),L"Falloff");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:GenLight*					light;public:exporter(GenLight* light2):light(light2) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(light->GetFallsize(timevalue));	}};

					wcs name=wcs(L"light_outer_angle_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((GenLight*)light->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_light_node_outer_angle_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
			}
			node->ReleaseIGameObject();
		}
		else if(isCameraNode(node))
		{
			IGameCamera* camera=(IGameCamera*)node->GetIGameObject();
			camera->InitializeData();

			REAL fov=90,near_clip=1,far_clip=1000,target_dist=100;
			IGameProperty* prop=NULL;

			prop=camera->GetCameraFOV();
			if(prop)
				prop->GetPropertyValue(fov);
			fov=fov*180/z3D::Core::PI;
			fov=ATAN(1.0f/(g_interface->GetRendImageAspect()/TAN(fov*0.5f)))*2;

			prop=camera->GetCameraNearClip();
			if(prop)
				prop->GetPropertyValue(near_clip);

			prop=camera->GetCameraFarClip();
			if(prop)
				prop->GetPropertyValue(far_clip);

			prop=camera->GetCameraTargetDist();
			if(prop)
				prop->GetPropertyValue(target_dist);

			output+=FORMATW(L"create_camera_node \"%s\" %f %f %f %f\r\n",MBSTOWCS(node->GetName()).c_str(),fov,near_clip,far_clip,target_dist);

			if(((CameraObject*)camera->GetMaxObject())->IsOrtho())
				output+=FORMATW(L"set_camera_ortho \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str());

			//listAnimatables(getSubAnim(camera->GetMaxObject(),L"Parameters"));
			if(true)
			{
				Animatable* animatable=NULL;
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(),L"Parameters"),L"FOV");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:CameraObject*					cam;public:exporter(CameraObject* camera):cam(camera) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							REAL fov=cam->GetFOV(timevalue);
							fov=fov*180/z3D::Core::PI;
							fov=ATAN(1.0f/(g_interface->GetRendImageAspect()/TAN(fov*0.5f)))*2;
							writer.write(fov);	}};

					wcs name=wcs(L"camera_yfov_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((CameraObject*)camera->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_camera_node_yfov_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(),L"Parameters"),L"Near Clip");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:CameraObject*					cam;public:exporter(CameraObject* camera):cam(camera) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(cam->GetClipDist(timevalue,CAM_HITHER_CLIP));	}};

					wcs name=wcs(L"camera_znear_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((CameraObject*)camera->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_camera_node_znear_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
				animatable = getSubAnim(getSubAnim(camera->GetMaxObject(),L"Parameters"),L"Far Clip ");
				if(!animatable)
					animatable = getSubAnim(getSubAnim(camera->GetMaxObject(),L"Parameters"),L"Far Clip");
				if(animatable && GetKeyControlInterface(animatable))
				{
					class exporter: public AnimDataExporter	{private:CameraObject*					cam;public:exporter(CameraObject* camera):cam(camera) {}public:
						void		write(IKeyControl* keyctl, TimeValue timevalue, MemWriter& writer)	{
							writer.write(cam->GetClipDist(timevalue,CAM_YON_CLIP));	}};

					wcs name=wcs(L"camera_zfar_")+MBSTOWCS(node->GetName())+L".bin";
					exportAnimData(GetKeyControlInterface(animatable),exporter((CameraObject*)camera->GetMaxObject()),getAnimDataExportAbsDir()+name);
					output+=FORMATW(L"set_camera_node_zfar_animdata \"%s\" \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str(),(getAnimDataExportRelDir()+name).c_str());
				}
			}

			node->ReleaseIGameObject();
		}
		else if(isBoneNode(node))
		{
			Box3 bb;
			node->GetIGameObject()->GetMaxObject()->GetDeformBBox(0,bb,NULL);
			Matrix3 trans;
			trans.SetRotateX(-PI*0.5f);
			Matrix3 objtm=node->GetMaxNode()->GetObjectTM(0);
			Matrix3 nodetm=node->GetMaxNode()->GetNodeTM(0);
			nodetm.Invert();
			objtm*=nodetm;
			bb=bb*objtm;
			bb=bb*trans;

			output+=FORMATW(L"create_bone_node \"%s\" %f %f %f %f %f %f\r\n",MBSTOWCS(node->GetName()).c_str(),bb.pmin.x,bb.pmin.y,bb.pmin.z,bb.pmax.x,bb.pmax.y,bb.pmax.z);
		}
		else
		{
			output+=FORMATW(L"create_node \"%s\"\r\n",MBSTOWCS(node->GetName()).c_str());
		}
	}
	output+=L"\r\n";

	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];
		//Mat4 m=toMat4(node->GetLocalTM(0));
		Mat4 m=getLocalTM(node,0);
		Vec3 pos,scale;
		z3D::Core::Quat rot;
		m.factorSRT(scale,rot,pos);
		//if(isCameraNode(node) || isLightNode(node))
			//rot=z3D::Core::Quat(Mat3::rotation(Vec3::xAxis,-90)*rot.asMat3());
		output+=FORMATW(L"set_node_pos \"%s\" %f %f %f\r\n",(MBSTOWCS(node->GetName())).c_str(),pos.x,pos.y,pos.z);
		output+=FORMATW(L"set_node_rot \"%s\" %f %f %f %f\r\n",(MBSTOWCS(node->GetName())).c_str(),rot.x,rot.y,rot.z,rot.w);
		output+=FORMATW(L"set_node_scale \"%s\" %f %f %f\r\n",(MBSTOWCS(node->GetName())).c_str(),scale.x,scale.y,scale.z);
	}
	output+=L"\r\n";

	UniqueMap<IGameNode*> node_map=UniqueMap<IGameNode*>::fromVector(array_node);
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];
		if(node_map[node->GetNodeParent()])
			output+=FORMATW(L"set_node_parent \"%s\" \"%s\"\r\n",(MBSTOWCS(node->GetName())).c_str(),(MBSTOWCS(node->GetNodeParent()->GetName())).c_str());
	}
	output+=L"\r\n";
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];
		bool cast_shadow=node->GetMaxNode()->CastShadows()?true:false;
		bool recv_shadow=node->GetMaxNode()->RcvShadows()?true:false;
		output+=FORMATW(L"set_node_shadow_mode \"%s\" %d %d \r\n",(MBSTOWCS(node->GetName())).c_str(),cast_shadow?1:0,recv_shadow?1:0);
	}

	//if(ExportOptions::text_version)
	{
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),output.length()*2);
	}

	return true;
}

#endif

void exportScene()
{
	showLogWindow((intptr_t)GetCOREInterface()->GetMAXHWnd());

	node_original_name0.clear();
	node_original_name.clear();
	material_original_name0.clear();
	material_original_name.clear();

	syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session starts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";

	if(!checkInvalidNodeNames())
	{
		rollbackRenamedNodeNames0();
		syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session aborts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
		return;
	}
	if(!checkDuplicatedNodeNames())
	{
		rollbackRenamedNodeNames();
		rollbackRenamedNodeNames0();
		syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session aborts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
		return;
	}
	if(!checkInvalidMaterialNames())
	{
		rollbackRenamedMaterialNames0();
		rollbackRenamedNodeNames();
		rollbackRenamedNodeNames0();
		syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session aborts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
		return;
	}
	if(!checkDuplicatedMaterialNames())
	{
		rollbackRenamedMaterialNames();
		rollbackRenamedMaterialNames0();
		rollbackRenamedNodeNames();
		rollbackRenamedNodeNames0();
		syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session aborts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
		return;
	}

	wcs dir = CONVERT(chooseFolder(L"choose output folder", CONVERT(ExportOptions::default_export_dir)));
	if(!dir.length())
	{
		rollbackRenamedMaterialNames();
		rollbackRenamedMaterialNames0();
		rollbackRenamedNodeNames();
		rollbackRenamedNodeNames0();

		syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session aborts >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";

		return;
	}
	export_base_dir = dir;
	export_mesh_dir = dir + L"/" + ExportOptions::mesh_dir + L"/";
	export_texture_dir = dir + L"/" + ExportOptions::texture_dir + L"/";
	export_animdata_dir = dir + L"/" + ExportOptions::animdata_dir + L"/";
	create_directory_hierarchy(export_base_dir);
	create_directory_hierarchy(export_mesh_dir);
	create_directory_hierarchy(export_texture_dir);
	create_directory_hierarchy(export_animdata_dir);

	MemWriter writer;

	//syslog << "enumSceneStructNode()";
	vector<IGameNode*> array_node = enumSceneStructNode();

	KFWriter kfwriter;
	wcs animdata_misc_path = getAnimDataDirREL() + L"/" + L"animdata_misc.bin";

	kfwriter.getWriter().write("ZANI", 4);
	kfwriter.getWriter().write((uint32_t)1);

	if(false)
	{
		class CMP
		{
		public:
			static bool		less(IGameNode* e0, IGameNode* e1)
			{
				return strcmp(e0->GetName(), e1->GetName()) < 0;
			}
		};
		sort(array_node.begin(), array_node.end(), &CMP::less);
	}

	vector<SKINNED_MESH> array_skinned_mesh;

	if(true)
	{
		TiXmlDocument doc;

		//syslog << "exportSceneStruct";

		exportSceneStruct(array_node, doc, array_skinned_mesh, kfwriter, animdata_misc_path);
		FILE* fp = NULL;
		_wfopen_s(&fp, (export_mesh_dir + L"scene.xml").c_str(), L"w");
		if(fp)
		{
			doc.SaveFile(fp);

			fclose(fp);
		}
	}

	//syslog << "exportTMController3()";
	exportTMController3(array_node);
	
	if(true)
	{
		TiXmlDocument doc;

		doc.InsertEndChild(TiXmlDeclaration("1.0", "utf-8", ""));

		TiXmlHandle root(doc.InsertEndChild(TiXmlElement("skins")));

		for(size_t i = 0; i < array_skinned_mesh.size(); ++i)
		{
			const SKINNED_MESH& s = array_skinned_mesh[i];

			zTiXmlElement& e = *(zTiXmlElement*)root.ToElement()->InsertEndChild(TiXmlElement("skin"));
			e.SetAttribute("node", MBSTOWCS(s.node->GetName()));
			e.SetAttribute("src", s.filepath);
		}

		FILE* fp = NULL;
		_wfopen_s(&fp, (export_mesh_dir + L"scene_skin.xml").c_str(), L"w");
		if(fp)
		{
			doc.SaveFile(fp);

			fclose(fp);
		}
	}

	exportUserText(array_node,writer);
	FileSystem::getFileSystem(L"WIN32")->saveFile(export_mesh_dir+L"scene_user.txt", writer.retrieveAll());

	if(kfwriter.chunks())
	{
		FileSystem::getFileSystem(L"WIN32")->saveFile(getExportBaseDir() + L"/" + animdata_misc_path, kfwriter.getWriter().retrieveAll());
	}

	rollbackRenamedMaterialNames();
	rollbackRenamedMaterialNames0();
	rollbackRenamedNodeNames();
	rollbackRenamedNodeNames0();

	if(true)
	{
		//FLASHWINFO info = {sizeof(FLASHWINFO), g_interface->GetMAXHWnd(), FLASHW_ALL | FLASHW_TIMERNOFG, 10, 0};
		FLASHWINFO info = {sizeof(FLASHWINFO), g_interface->GetMAXHWnd(), FLASHW_ALL, 10, 0};
		FlashWindowEx(&info);
		//FlashWindow(GetCOREInterface()->GetMAXHWnd(), TRUE);
	}

	syslog << L"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< export session ends >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
}

class SceneExporter
{
private:
	class TriObjectDeleter
	{
	private:
		TriObject*	_tri;
		int			_deleteIt;
	public:
		TriObjectDeleter(TriObject* tri,int deleteIt)
		{
			_tri=tri;
			_deleteIt=deleteIt;
		}
		~TriObjectDeleter()
		{
			if(_deleteIt)
				_tri->DeleteMe();
		}
	};
protected:
	map<IGameNode*,wcs>				_map_meshnode_exported_filename;
	map<Object*,wcs>				_exported_meshnode_geometry;
	map<IGameNode*,wcs>				_map_meshnode_exported_mtl_filename;
	map<Mtl*,wcs>					_exported_material;
	map<uint32_t,wcs>				_map_image_checksum_exported_filename;
	map<IGameNode*,wcs>				_map_meshnode_exported_skin;
	map<Modifier*,wcs>				_exported_skin;
	map<Animatable*,wcs>			_exported_animdata;
private:
	void		reset()
	{
		_map_meshnode_exported_filename.clear();
		_exported_meshnode_geometry.clear();
		_map_meshnode_exported_mtl_filename.clear();
		_exported_material.clear();
		_map_image_checksum_exported_filename.clear();
		_map_meshnode_exported_skin.clear();
		_exported_skin.clear();
		_exported_animdata.clear();
	}
private:
	bool exportAnimData(Animatable* animatable, AnimDataExporter& exporter, const wcs& output_filepath)
	{
		IKeyControl* keyctl=GetKeyControlInterface(animatable);
		if(!keyctl || keyctl->GetNumKeys() <= 0)
			return false;

		MemWriter writer;

		REAL ticks_divider = (REAL)GetTicksPerFrame();

		REAL speed = (REAL)GetFrameRate();

		ticks_divider *= speed;
		speed = 1.0f;

		map<TimeValue,bool> map_timevalue;
		enumKeyTimeValueRecur(animatable,map_timevalue);
		vector<TimeValue> array_timevalue;
		convert(map_timevalue,array_timevalue);

		if(array_timevalue.size()>=1 && ExportOptions::sample_every_frame)
			array_timevalue=_array_restricted_timevalue;

		for(int i=0;i<(int)array_timevalue.size();++i)
		{
			writer.write((REAL)(array_timevalue[i]/ticks_divider));
			exporter.write(keyctl,array_timevalue[i],writer);
		}

		FileSystem::getFileSystem(L"WIN32")->saveFile(output_filepath, writer.retrieveAll());

		return true;
	}
private:
	void		getTexmapParams(Texmap* texmap,wcs& filter,wcs& u_wrap,wcs& v_wrap,REAL& offset_u,REAL& offset_v,REAL& tiling_u,REAL& tiling_v,REAL& angle_u,REAL& angle_v,REAL& angle_w,uint32_t& channel)
	{
		if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
			filter=((BitmapTex*)texmap)->GetFilterType()==FILTER_NADA?L"nearest":(((BitmapTex*)texmap)->GetFilterType()==FILTER_SAT?L"linear":L"mipmap");
		else
			filter=L"mipmap";
		int tiling=texmap->GetTextureTiling();
		u_wrap=(tiling&U_WRAP)?L"repeat":L"clamp";
		v_wrap=(tiling&V_WRAP)?L"repeat":L"clamp";

		StdUVGen* uvgen=((BitmapTex*)texmap)->GetUVGen();
		if(uvgen)
		{
			offset_u=uvgen->GetUOffs(0);
			offset_v=uvgen->GetUOffs(0);
			tiling_u=uvgen->GetUScl(0);
			tiling_v=uvgen->GetVScl(0);
			angle_u=uvgen->GetUAng(0);
			angle_v=uvgen->GetVAng(0);
			angle_w=uvgen->GetWAng(0);
		}
		else
		{
			offset_u=0;
			offset_v=0;
			tiling_u=1;
			tiling_v=1;
			angle_u=0;
			angle_v=0;
			angle_w=0;
		}

		channel=(uint32_t)texmap->GetMapChannel();
	}
	void		writeUVGenAnimData(wcs& output,int mtl_index,const wcs& mapname,Texmap* texmap)
	{
		if(texmap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0))
			return;
		StdUVGen* uvgen=((BitmapTex*)texmap)->GetUVGen();
		if(!uvgen)
			return;

		wcs uoff,voff,util,vtil,uang,vang,wang;
		if(true)
		{
			wchar_t animname[][32]={L"U Offset",L"V Offset",L"U Tiling",L"V Tiling",L"U Angle",L"V Angle",L"W Angle"};
			for(int i=0;i<7;++i)
			{
				Animatable* animatable=getSubAnim(getSubAnim(getSubAnim(texmap,L"Coordinates"),L"Parameters"),animname[i]);
				if(animatable && GetKeyControlInterface(animatable))
				{
					if(_exported_animdata.count(animatable))
						continue;
					wcs name=FORMATW(L"animdata_%4d",_exported_animdata.size());
					wcs s=ExportOptions::animdata_dir+L"/"+name+L".bin";
					switch(i)
					{
					case 0: uoff=s; break;
					case 1: voff=s; break;
					case 2: util=s; break;
					case 3: vtil=s; break;
					case 4: uang=s; break;
					case 5: vang=s; break;
					case 6: wang=s; break;
					}
					//if(exportAnimData(animatable,uoff_exporter(uvgen),export_base_dir + L"/" + s))
						//_exported_animdata[animatable]=s;
				}
			}
		}
		if(!uoff.length() && !voff.length() && !util.length() && !vtil.length() && !uang.length() && !vang.length() && !wang.length())
			return;

		output+=FORMATW(L"set_mtl_%s_map_animdata %d \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"\r\n",
			mtl_index,
			mapname.c_str(),
			ESCSTR(uoff).c_str(),ESCSTR(voff).c_str(),
			ESCSTR(util).c_str(),ESCSTR(vtil).c_str(),
			ESCSTR(uang).c_str(),ESCSTR(vang).c_str(),ESCSTR(wang).c_str()
			);
	}
	wcs		exportTexture(fipImage& img)
	{
		uint32_t crc = gen_crc(img.accessPixels(), (int)img.getScanWidth() * img.getHeight());

		wcs filename=FORMATW(L"%08X.png",crc);
		wcs rel_path=ExportOptions::texture_dir+L"/"+filename;

		img.saveU((export_base_dir + L"/" + rel_path).c_str());

		_map_image_checksum_exported_filename[crc] = rel_path;

		return rel_path;
	}
	void		exportMaterial(IGameNode* gamenode)
	{
		vector<StdMat*> array_mtl;
		if(true)
		{
			IGameMaterial* m=gamenode->GetNodeMaterial();
			if(m)
			{
				Mtl* m2=m->GetMaxMaterial();
				if(m2)
				{
					if(_exported_material.count(m2))
						return;
					if(m2->ClassID()==Class_ID(BAKE_SHELL_CLASS_ID,0))
					{
						if(m2->NumSubMtls()>=2)
							m2=m2->GetSubMtl(1);
						else
						{
							syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
							m2=NULL;
						}
					}
					if(m2)
					{
						if(m2->ClassID()==Class_ID(DMTL_CLASS_ID,0))
							array_mtl.push_back((StdMat*)m2);
						else
						{
							if(m2->ClassID()==Class_ID(MULTI_CLASS_ID,0))
							{
								if(!m2->NumSubMtls())
								{
									array_mtl.push_back(NULL);
									syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
								}
								else
								{
									for(int i=0;i<m2->NumSubMtls();++i)
									{
										Mtl* m3=m2->GetSubMtl(i);
										if(m3->ClassID()==Class_ID(DMTL_CLASS_ID,0))
											array_mtl.push_back((StdMat*)m3);
										else
										{
											array_mtl.push_back(NULL);
											syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
										}
									}
								}
							}
							else
							{
								array_mtl.push_back(NULL);
								syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
							}
						}
					}
					else
					{
						array_mtl.push_back(NULL);
						syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
					}
				}
				else
				{
					array_mtl.push_back(NULL);
					syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
				}
			}
			else
			{
				array_mtl.push_back(NULL);
				syslog << FORMATW(L"unsupported material!! exporting node: %s", MBSTOWCS(gamenode->GetName()).c_str());
			}
		}

		const int OUTPUT_TEX_RES = 256;

		// actual export begins...
		wcs output;

		output+=FORMATW(L"resize_mtls %d\r\n\r\n",(int)array_mtl.size());
		for(int i=0;i<(int)array_mtl.size();++i)
		{
			// to be determined ...
			output+=FORMATW(L"set_mtl_render_type %d \"%s\"			// \"unlit\", \"lit\", \"transparent\", \"reflection\"\r\n",i,L"lit");
			output+=FORMATW(L"set_mtl_render_method %d \"%s\"			// \"unlit\", \"lit\", \"selfill\", \"toon\", \"reflection\", \"normal_mapping\", \"sphere_env_mapping\" \r\n",i,L"lit");
			output+=L"\r\n";
			StdMat* mtl=array_mtl[i];
			if(mtl)
			{
				Color c;
				c=mtl->GetAmbient(TIME_NegInfinity);
				if(ExportOptions::export_white_ambient_diffuse_color&&findTexmap(mtl,ID_DI))
					c[0]=c[1]=c[2]=1.0f;
				output+=FORMATW(L"set_mtl_ambient %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				c=mtl->GetDiffuse(TIME_NegInfinity);
				if(ExportOptions::export_white_ambient_diffuse_color&&findTexmap(mtl,ID_DI))
					c[0]=c[1]=c[2]=1.0f;
				output+=FORMATW(L"set_mtl_diffuse %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				c=mtl->GetSpecular(TIME_NegInfinity);
				output+=FORMATW(L"set_mtl_specular %d %f %f %f\r\n",i,c[0],c[1],c[2]);
				output+=FORMATW(L"set_mtl_specular_level %d %f\r\n",i,mtl->GetShinStr(TIME_NegInfinity));
				output+=FORMATW(L"set_mtl_glossiness %d %f\r\n",i,mtl->GetShininess(TIME_NegInfinity));

				output+=FORMATW(L"set_mtl_two_side %d %d\r\n",i,mtl->GetTwoSided());
				output+=FORMATW(L"set_mtl_blend_type %d \"%s\"\r\n",i,(mtl->GetTransparencyType()==TRANSP_FILTER)?L"filter":((mtl->GetTransparencyType()==TRANSP_ADDITIVE)?L"additive":L"subtractive"));
				output+=FORMATW(L"set_mtl_opacity %d %f\r\n",i,mtl->GetOpacity(g_interface->GetTime()));

				if(true)
				{
					Texmap* texmap;
					texmap=findTexmap(mtl,ID_DI);
					if(texmap)
					{
						wcs rel_path;

						Texmap* alpha_texmap=findTexmap(mtl,ID_OP);
						if(!alpha_texmap)
						{
							bool copy_fail=false;
							if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
							{
								wcs src_path=MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
								wcs filename=src_path.substr(src_path.rfind('\\')+1);
								rel_path=ExportOptions::texture_dir+L"/"+filename;

								// the reason of copy failure is that the texture file is not in the right place, however, sometimes, 3ds max can still display the texture because the texture is in the same directory as the max file. so there are two cases, one is that the texture file is actually in the same directory as the max file, the other is that simply the texture isn't anywhere, to simply deal with this, just let max output the texture for us and save it
								copy_fail=CopyFileW(src_path.c_str(),(export_texture_dir+filename).c_str(),FALSE)?false:true;
							}
							if(texmap->ClassID()!=Class_ID(BMTEX_CLASS_ID, 0)||copy_fail)
							{
								fipImage diffuse=outputTexmap(texmap,ExportOptions::generated_tex_size,false);
								rel_path=exportTexture(diffuse);
							}
						}
						else
						{
							bool done=false;
							while(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0)&&alpha_texmap->ClassID()==Class_ID(BMTEX_CLASS_ID, 0))
							{
								fipImage diffuse,alpha;
								if(!diffuse.load(((BitmapTex*)texmap)->GetMapName()))
								{
									syslog << L"cannot get " + MBSTOWCS(((BitmapTex*)texmap)->GetMapName());
									break;
								}
								if(!alpha.load(((BitmapTex*)alpha_texmap)->GetMapName()))
								{
									syslog << L"cannot get " + MBSTOWCS(((BitmapTex*)alpha_texmap)->GetMapName());
									break;
								}

								diffuse.convertTo32Bits();
								alpha.convertTo32Bits();

								diffuse.rescale(max(diffuse.getWidth(),alpha.getWidth()),max(diffuse.getHeight(),alpha.getHeight()),FILTER_BOX);
								alpha.rescale(max(diffuse.getWidth(),alpha.getWidth()),max(diffuse.getHeight(),alpha.getHeight()),FILTER_BOX);

								if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
								{
									fipImage diffuse2;
									diffuse.getChannel(diffuse2,FICC_ALPHA);
									diffuse2.convertTo32Bits();
									diffuse=diffuse2;
								}
								if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
								{
									fipImage alpha2;
									alpha.getChannel(alpha2,FICC_ALPHA);
									alpha=alpha2;
								}
								else
									alpha.convertTo8Bits();

								diffuse.setChannel(alpha,FICC_ALPHA);

								rel_path=exportTexture(diffuse);

								done = true;
								break;
							}
							if(!done)
							{
								fipImage diffuse=outputTexmap(texmap,OUTPUT_TEX_RES,false);
								fipImage alpha=outputTexmap(alpha_texmap,OUTPUT_TEX_RES,false);

								if(texmap->ClassID()==Class_ID(BMTEX_CLASS_ID,0))
								{
									if(((BitmapTex*)texmap)->GetAlphaAsRGB(false))
									{
										fipImage diffuse2;
										diffuse.getChannel(diffuse2,FICC_ALPHA);
										diffuse2.convertTo32Bits();
										diffuse=diffuse2;
									}
								}
								if(alpha_texmap->ClassID()==Class_ID(BMTEX_CLASS_ID,0))
								{
									if(((BitmapTex*)alpha_texmap)->GetAlphaAsMono(false))
									{
										fipImage alpha2;
										alpha.getChannel(alpha2,FICC_ALPHA);
										alpha=alpha2;
									}
									else
										alpha.convertTo8Bits();
								}
								else
									alpha.convertTo8Bits();

								diffuse.setChannel(alpha,FICC_ALPHA);

								rel_path=exportTexture(diffuse);
							}
						}

						wcs filter,u_wrap,v_wrap;
						REAL offset_u,offset_v,tiling_u,tiling_v,angle_u,angle_v,angle_w;
						uint32_t channel;

						getTexmapParams(texmap,filter,u_wrap,v_wrap,offset_u,offset_v,tiling_u,tiling_v,angle_u,angle_v,angle_w,channel);
						if(alpha_texmap)
							filter=L"linear";

						output+=FORMATW(L"set_mtl_diffuse_map %d \"%s\" \"%s\" \"%s\" \"%s\" %f %f %f %f %f %f %f %d\r\n",i,
							ESCSTR(rel_path).c_str(),filter.c_str(),u_wrap.c_str(),v_wrap.c_str(),
							offset_u,offset_v,
							tiling_u,tiling_v,
							angle_u,angle_v,angle_w,
							channel
							);

						writeUVGenAnimData(output,i,L"diffuse",texmap);
					}
				}
				output+=FORMATW(L"set_mtl_layer_index %d %d			// manual transparent drawing order sorting, the lower the closer, integer only\r\n",i,0);
			}
			else
			{
				// write some default material parameters ...
				COLORREF color=gamenode->GetMaxNode()->GetWireColor();

				output+=FORMATW(L"set_mtl_ambient %d %f %f %f\r\n",i,(color&0xff)/255.0f,((color>>8)&0xff)/255.0f,((color>>16)&0xff)/255.0f);
				output+=FORMATW(L"set_mtl_diffuse %d %f %f %f\r\n",i,(color&0xff)/255.0f,((color>>8)&0xff)/255.0f,((color>>16)&0xff)/255.0f);
				output+=FORMATW(L"set_mtl_specular %d %f %f %f\r\n",i,1.0f,1.0f,1.0f);
				output+=FORMATW(L"set_mtl_specular_level %d %f\r\n",i,0.0f);
				output+=FORMATW(L"set_mtl_glossiness %d %f\r\n",i,0.1f);
				output+=FORMATW(L"set_mtl_two_side %d %d\r\n",i,0);
				output+=FORMATW(L"set_mtl_blend_type %d \"%s\"\r\n",i,L"filter");
				output+=FORMATW(L"set_mtl_opacity %d %f\r\n",i,1.0f);
			}
		}


		MemWriter writer;
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),(int)output.length()*2);

		if(true)
		{
			wcs s=ExportOptions::mesh_dir+L"/"+FORMATW(L"mtl_%04d.bin",(uint32_t)_exported_material.size());
			FileSystem::getFileSystem(L"WIN32")->saveFile(export_base_dir+L"/"+s, writer.retrieveAll());
			if(gamenode->GetNodeMaterial() && gamenode->GetNodeMaterial()->GetMaxMaterial())
				_exported_material[gamenode->GetNodeMaterial()->GetMaxMaterial()] = s;
			else
			{
				// nothing to handle
			}
			_map_meshnode_exported_mtl_filename[gamenode] = s;
		}
	}
	void		exportMesh(IGameNode* gamenode)
	{
		INode* node=gamenode->GetMaxNode();

		Object* obj=node->GetObjOrWSMRef();
		if(!obj)
			return;

		if(_exported_meshnode_geometry.count(obj))
		{
			_map_meshnode_exported_filename[gamenode]=_exported_meshnode_geometry[obj];
			return;
		}

		int deleteIt;
		TriObject* tri=GetTriObjectFromNode(node,deleteIt);
		TriObjectDeleter trideleter(tri,deleteIt);


		Mat4 vert_trans=toMat4(node->GetObjTMAfterWSM(0))*ROTXNEG90_4*getWorldTM(node).inverse();
		Mat3 norm_trans=((Mat3)vert_trans).inverse().transpose();

		Mat3 test_flip_mat=(Mat3)toMat4(node->GetObjTMAfterWSM(0));
		bool flip=false;
		if((Vec3(test_flip_mat[0])^Vec3(test_flip_mat[1]))*Vec3(test_flip_mat[2])<0)
			flip=true;

		Mesh& mesh=tri->mesh;
		mesh.buildRenderNormals();

		if(flip)
		{
			for(int i=0;i<mesh.getNumFaces();++i)
				mesh.FlipNormal(i);
		}

		map< int,vector<int> > map_mtl_array_face_idx;
		for(int i=0;i<mesh.getNumFaces();++i)
			map_mtl_array_face_idx[mesh.getFaceMtlIndex(i)].push_back(i);

		vector<int> exp_face_idx_to_face_idx;
		{
			map< int,vector<int> >::iterator iter;
			for(iter=map_mtl_array_face_idx.begin();iter!=map_mtl_array_face_idx.end();++iter)
			{
				const vector<int>& array_face_idx=iter->second;
				for(int i=0;i<(int)array_face_idx.size();++i)
					exp_face_idx_to_face_idx.push_back(array_face_idx[i]);
			}
		}

		vector< Vec3 > verts;

		for(int i=0;i<mesh.getNumVerts();++i)
			verts.push_back(toVec3(mesh.getVert(i))*vert_trans);

		vector< Vec3 > normals;
		vector< INT3 > face_noridx;
		if(true)
		{
			map<RNormal*,int> map_rnor_noridx;
			for(int i=0;i<(int)mesh.getNumFaces();++i)
			{
				Face& f=mesh.faces[i];
				int face_idx=-1;
				INT3 ni;
				for(int j=0;j<3;++j)
				{
					RNormal* rnor=NULL;
					if(f.smGroup)
						rnor=getFaceVertexNormal(mesh,i,j);
					if(!rnor)
					{
						if(face_idx==-1)
						{
							face_idx=(int)normals.size();
							normals.push_back((toVec3(mesh.getFaceNormal(i))*norm_trans).normal_safe());
						}
						ni[j]=face_idx;
					}
					else
					{
						if(!map_rnor_noridx.count(rnor))
						{
							map_rnor_noridx[rnor]=(int)normals.size();
							normals.push_back((toVec3(rnor->getNormal())*norm_trans).normal_safe());
						}
						ni[j]=map_rnor_noridx[rnor];
					}
				}
				face_noridx.push_back(ni);
			}
		}

		// actual export begins ...
		MemWriter writer;

		// reserve 4 bytes for verification code
		writer.write((uint32_t)0);

		// write number of vertices
		writer.write((uint32_t)verts.size());
		if(!verts.size())
			return;

		// write the vertex position array
		writer.write(&verts[0],(int)(sizeof(Vec3)*verts.size()));

		// write number of normals
		writer.write((uint32_t)normals.size());
		if(!normals.size())
			return;

		// write the vertex normal array
		writer.write(&normals[0],(int)(sizeof(Vec3)*normals.size()));

		// write number of faces
		writer.write((uint32_t)mesh.getNumFaces());
		if(!mesh.getNumFaces())
			return;
		if(true)
		{
			int count=mesh.getNumFaces();
			for(int i=0;i<count;++i)
			{
				int fi=exp_face_idx_to_face_idx[i];
				Face& f=mesh.faces[fi];
				int n0,n1,n2;

				n0=face_noridx[fi][0];
				n1=face_noridx[fi][1];
				n2=face_noridx[fi][2];

				// write face's vertex position indices and normal indices
				writer.write((uint32_t)f.v[0]);
				writer.write((uint32_t)f.v[1]);
				writer.write((uint32_t)f.v[2]);
				writer.write((uint32_t)n0);
				writer.write((uint32_t)n1);
				writer.write((uint32_t)n2);
			}
		}

		// write mapped vertex attribute information
		if(true)
		{
			int count=mesh.getNumMaps();
			for(int i=0;i<count;++i)
			{
				if(!mesh.mapSupport(i))
					continue;
				if(mesh.getNumFaces()!=mesh.Map(i).fnum)
					continue;

				uint32_t c;

				// write which channel, -1 to indicate the end
				writer.write((uint32_t)i);

				// write number of mapped vertices
				writer.write(c=(uint32_t)mesh.getNumMapVerts(i));
				UVVert* v=mesh.mapVerts(i);
				for(int j=0;j<(int)c;++j)
				{
					// write mapped vertex attributes
					writer.write((float)v->x);
					writer.write((float)v->y);

					// skip z components
					//writer.write((float)v->z);
					++v;
				}
				TVFace* f=mesh.mapFaces(i);
				c=(uint32_t)mesh.getNumFaces();
				for(int j=0;j<(int)c;++j)
				{
					TVFace* f2=f+exp_face_idx_to_face_idx[j];

					// write mapped face mapped vertex indices
					writer.write((uint32_t)f2->t[0]);
					writer.write((uint32_t)f2->t[1]);
					writer.write((uint32_t)f2->t[2]);
				}
			}
		}

		// end writing mapped vertex attribute information
		writer.write((uint32_t)-1);

		// writing submeshes
		if(true)
		{
			uint32_t last=0;

			// write number of submeshes
			writer.write((uint32_t)map_mtl_array_face_idx.size());

			map< int,vector<int> >::iterator iter;
			for(iter=map_mtl_array_face_idx.begin();iter!=map_mtl_array_face_idx.end();++iter)
			{
				writer.write(last);										// start face
				writer.write((uint32_t)iter->second.size());			// face count
				writer.write((uint32_t)iter->first);					// material id

				last+=(uint32_t)iter->second.size();
			}
		}

		// write the verification code
		if(true)
		{
			uint32_t code;
			memcpy(&code,"MESH",4);
			code ^= (uint32_t)writer.length();
			code ^= REVERSE_BITS((uint32_t)writer.length());

			writer.write(code);
		}

		wcs s=ExportOptions::mesh_dir+L"/"+FORMATW(L"mesh_%04d.bin",(uint32_t)_exported_meshnode_geometry.size());
		FileSystem::getFileSystem(L"WIN32")->saveFile(export_base_dir+L"/"+s, writer.retrieveAll());
		_map_meshnode_exported_filename[gamenode]=s;
	}
public:
	void		exportScene()
	{
		reset();

		checkDuplicatedNodeNames();

		wcs dir = CONVERT(chooseFolder(L"choose output folder", CONVERT(ExportOptions::default_export_dir)));
		if(!dir.length())
			return;
		export_base_dir=dir;
		export_mesh_dir=dir+L"/"+ExportOptions::mesh_dir+L"/";
		export_texture_dir=dir+L"/"+ExportOptions::texture_dir+L"/";
		export_animdata_dir=dir+L"/"+ExportOptions::animdata_dir+L"/";
		create_directory_hierarchy(export_base_dir);
		create_directory_hierarchy(export_mesh_dir);
		create_directory_hierarchy(export_texture_dir);
		create_directory_hierarchy(export_animdata_dir);

		MemWriter writer;
		vector<IGameNode*> array_node=enumSceneStructNode();
		/*
		for(int i=0;i<(int)array_node.size();++i)
			resetXForm(array_node[i]->GetMaxNode());
			*/

		// must have all meshes exported then export scene, because from then on mesh nodes can be instances, so they may reference just one mesh geometry
		// user text however is applied to node, so no way to avoid duplication
		// for material, different mesh nodes which are instances of one kind may have different materials, because materials are applied to nodes
		// for controllers, well its getting very complicated ever, controllers can be instanced, wow, for the gorgeousness of z3D, i am striving to make it perfect like no other
		/*
		exportSceneStruct(array_node,writer);
		FileSystem::getFileSystem(L"WIN32").saveFile(export_mesh_dir+L"scene.txt",&writer);
		*/

		vector<IGameNode*> array_skinned_node;

		MemWriter mesh_writer,mtl_writer,skin_writer;

		int i;
		for(i=0;i<(int)array_node.size();++i)
			if(isMeshNode(array_node[i]))
			{
				// export mesh node's geometry first, examine inside's object's pointer, take care GetObjOrWSMRef()
				exportMesh(array_node[i]);

				// export mesh node's material
				exportMaterial(array_node[i]);

				/*
				// export mesh
				exportNodeMesh(array_node[i],mesh_writer,mtl_writer);
				if(ExportOptions::text_version && false)
					FileSystem::getFileSystem(L"WIN32").saveFile(export_mesh_dir+wstring(L"mesh_")+MBSTOWCS(array_node[i]->GetName())+L".txt",&mesh_writer);
				else
					FileSystem::getFileSystem(L"WIN32").saveFile(export_mesh_dir+wstring(L"mesh_")+MBSTOWCS(array_node[i]->GetName())+L".bin",&mesh_writer);

				// export mesh mtl
				FileSystem::getFileSystem(L"WIN32").saveFile(export_mesh_dir+L"mtl_"+MBSTOWCS(array_node[i]->GetName())+L".txt",&mtl_writer);

				// export mesh skin controller
				if(exportSkin(array_node[i],skin_writer))
				{
					if(ExportOptions::text_version && false)
						FileSystem::getFileSystem(L"WIN32").saveFile(export_animdata_dir+wstring(L"raw_skin_")+MBSTOWCS(array_node[i]->GetName())+L".txt",&skin_writer);
					else
						FileSystem::getFileSystem(L"WIN32").saveFile(export_animdata_dir+wstring(L"raw_skin_")+MBSTOWCS(array_node[i]->GetName())+L".bin",&skin_writer);

					array_skinned_node.push_back(array_node[i]);
				}
				*/
			}
		exportTMController2(array_node);
		
		exportSceneSkinController(array_skinned_node,writer);
		FileSystem::getFileSystem(L"WIN32")->saveFile(export_mesh_dir+L"scene_skin.txt", writer.retrieveAll());

		rollbackRenamedNodeNames();

		if(true)
		{
			Config cfg(export_mesh_dir+L"scene_raw_skin.txt");
			int i;
			for(i=0;i<(int)array_skinned_node.size();++i)
			{
				cfg.setWString(FORMATW(L"raw_skin_%d_node",i),MBSTOWCS(array_skinned_node[i]->GetName()));
				cfg.setWString(FORMATW(L"raw_skin_%d_filepath",i),wcs(L"raw_skin_")+MBSTOWCS(array_skinned_node[i]->GetName())+L".bin");
			}
		}

		exportUserText(array_node,writer);
		FileSystem::getFileSystem(L"WIN32")->saveFile(export_mesh_dir+L"scene_user.txt", writer.retrieveAll());

		if(true)
		{
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			GetStartupInfoW(&si);
			wchar_t buffer[4096];
			wcscpy_s(buffer,(EXE_DIR()+L"/"+FORMATW(L"plugins/rawskin2skin work_dir=\"%s\";mesh_dir=\"%s\";animdata_dir=\"%s\"",ESCSTR(export_base_dir).c_str(),ESCSTR(ExportOptions::mesh_dir).c_str(),ESCSTR(ExportOptions::animdata_dir).c_str())).c_str());
			if(CreateProcessW(0,buffer,0,0,FALSE,0,0,export_base_dir.c_str(),&si,&pi))
			{
				WaitForSingleObject(pi.hProcess,INFINITE);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			else
			{
				MessageBoxW(g_interface->GetMAXHWnd(),L"Cannot execute rawskin2skin\n\nMake sure .exe and .dll are copied",L"Warning",0);
			}
		}
		if(true)
		{
			FLASHWINFO info={sizeof(FLASHWINFO),g_interface->GetMAXHWnd(),FLASHW_ALL,10,0};
			FlashWindowEx(&info);
		}
	}
};

bool exportSceneSkinController(const vector<IGameNode*> array_node,MemWriter& writer)
{
	writer.clear();

	wcs output;

	int i;
	for(i=0;i<(int)array_node.size();++i)
	{
		/*
		if(ExportOptions::text_version)
			output+=FORMATW(L"create_skin_controller \"%s\" \"%s\" \"%s\"\r\n",(MBSTOWCS(array_node[i]->GetName())+L"_skin").c_str(),MBSTOWCS(array_node[i]->GetName()).c_str(),ESCSTR(ExportOptions::mesh_dir+L"/"+MBSTOWCS(array_node[i]->GetName())+L"_skin.txt").c_str());
		else
			output+=FORMATW(L"create_skin_controller \"%s\" \"%s\" \"%s\"\r\n",(MBSTOWCS(array_node[i]->GetName())+L"_skin").c_str(),MBSTOWCS(array_node[i]->GetName()).c_str(),ESCSTR(ExportOptions::mesh_dir+L"/"+MBSTOWCS(array_node[i]->GetName())+L"_skin.bin").c_str());
			*/
		output+=FORMATW(L"create_skin_controller \"%s\" \"%s\" \"%s\"\r\n",(wcs(L"skin_")+MBSTOWCS(array_node[i]->GetName())).c_str(),MBSTOWCS(array_node[i]->GetName()).c_str(),ESCSTR(getAnimDataDir()+L"skin_"+MBSTOWCS(array_node[i]->GetName())+L".bin").c_str());
	}

	//if(ExportOptions::text_version)
	{
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),output.length()*2);
	}

	return true;
}

bool exportUserText(const vector<IGameNode*> array_node,MemWriter& writer)
{
	writer.clear();

	wcs output;

	int i;
	for(i=0;i<(int)array_node.size();++i)
	{
		TSTR str;
		array_node[i]->GetMaxNode()->GetUserPropBuffer(str);
		if(str.Length())
			output+=FORMATW(L"set_user_text \"%s\" \"%s\"\r\n",MBSTOWCS(array_node[i]->GetName()).c_str(),ESCSTR(MBSTOWCS((char*)str)).c_str());
	}

	//if(ExportOptions::text_version)
	{
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),output.length()*2);
	}

	return true;
}


