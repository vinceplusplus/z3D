
#include "MeshExport.h"

bool exportTMController(const vector<IGameNode*> array_node,MemWriter& writer,bool at_least_1)
{
	writer.clear();

	wcs output;
	//Matrix3 trans;
	//trans.SetRotateX(-PI*0.5f);

	REAL ticks_divider=(REAL)GetTicksPerFrame();

	REAL speed=(REAL)GetFrameRate();

	// why speed?? it is to preserve frame position the same as in 3ds max
	// but now it is not used because maintaining it would be troublesome if multiple animation using different frame per second
	ticks_divider*=speed;
	speed=1.0f;

	int i,j;

	int ctls=0;
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];

		if(!at_least_1&&!anyKeyFrame(node->GetMaxNode()))
			continue;
		++ctls;
	}
	if(ExportOptions::text_version)
	{
		output+=FORMATW(L"set_speed %f\r\n",speed);
	}
	else
	{
		writer.write(speed);
	}
	if(ExportOptions::text_version)
	{
	}
	else
	{
		writer.write((int)ctls);
	}
	for(i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];

		if(!at_least_1&&!anyKeyFrame(node->GetMaxNode()))
			continue;

		if(ExportOptions::text_version)
			output+=FORMATW(L"create_tm_controller \"%s\" \"%s\"\r\n",(MBSTOWCS(node->GetName())+L"_tm").c_str(),(MBSTOWCS(node->GetName())).c_str());
		else
		{
			writer.write((MBSTOWCS(node->GetName())+L"_tm"));
			writer.write(MBSTOWCS(node->GetName()));
		}
		output+=L"\r\n";
		try
		{
			vector<TimeValue> key_timevalue;

			getNodePositionKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);

			if(ExportOptions::text_version)
				output+=FORMATW(L"resize_pos_keys %d\r\n",key_timevalue.size());
			else
				writer.write((int)key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				//Mat4 m=toMat4(GMatrix(node->GetMaxNode()->GetNodeTM(key_timevalue[j])))*(node->GetMaxNode()->GetParentNode()?toMat4(GMatrix(node->GetMaxNode()->GetParentNode()->GetNodeTM(key_timevalue[j]))).inverse():Mat4::identity);
				//Mat4 m=toMat4(node->GetWorldTM(key_timevalue[j]));
				//Mat4 m=toMat4(GMatrix(node->GetMaxNode()->GetObjTMBeforeWSM(key_timevalue[j])));
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);
				if(ExportOptions::text_version)
					output+=FORMATW(L"set_pos_key %d %f %f %f %f\r\n",j,(REAL)key_timevalue[j]/ticks_divider,pos[0],pos[1],pos[2]);
				else
				{
					writer.write((float)key_timevalue[j]/ticks_divider);
					writer.write(&pos,sizeof(pos));
				}
			}
			output+=L"\r\n";

			getNodeRotationKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);

			if(ExportOptions::text_version)
				output+=FORMATW(L"resize_rot_keys %d\r\n",key_timevalue.size());
			else
				writer.write((int)key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);

				if(isCameraNode(node))
					rot=z3D::Core::Quat(ROTXNEG90_3*rot.asMat3());

				if(ExportOptions::text_version)
					output+=FORMATW(L"set_rot_key %d %f %f %f %f %f\r\n",j,(REAL)key_timevalue[j]/ticks_divider,rot[0],rot[1],rot[2],rot[3]);
				else
				{
					writer.write((float)key_timevalue[j]/ticks_divider);
					writer.write(&rot,sizeof(rot));
				}
			}
			output+=L"\r\n";

			getNodeScaleKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);

			if(ExportOptions::text_version)
				output+=FORMATW(L"resize_scale_keys %d\r\n",key_timevalue.size());
			else
				writer.write((int)key_timevalue.size());
			for(j=0;j<(int)key_timevalue.size();++j)
			{
				GMatrix mat=node->GetLocalTM(key_timevalue[j]);
				Mat4 m=toMat4(mat);
				Vec3 scale,pos;
				z3D::Core::Quat rot;
				m.factorSRT(scale,rot,pos);
				if(ExportOptions::text_version)
					output+=FORMATW(L"set_scale_key %d %f %f %f %f\r\n",j,(REAL)key_timevalue[j]/ticks_divider,scale[0],scale[1],scale[2]);
				else
				{
					writer.write((float)key_timevalue[j]/ticks_divider);
					writer.write(&scale,sizeof(scale));
				}
			}
			output+=L"\r\n";

		}
		catch(...){syslog << wcs(L"exception occurs while exporting keyframe data of node ") + MBSTOWCS(node->GetName());}
		output+=L"\r\n";
	}

	if(ExportOptions::text_version)
	{
		writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
		writer.write((void*)output.c_str(),output.length()*2);
	}

	return true;
}

void exportTMController2(const vector<IGameNode*>& array_node)
{
	REAL ticks_to_sec=(REAL)(1.0/(GetTicksPerFrame()*GetFrameRate()));

	wcs output;
	for(int i=0;i<(int)array_node.size();++i)
	{
		IGameNode* node=array_node[i];
		if(!anyKeyFrame(node->GetMaxNode()))
			continue;
		try
		{
			wcs pos_path,rot_path,scale_path;

			vector<TimeValue> key_timevalue;

			getNodePositionKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			if(key_timevalue.size())
			{
				MemWriter writer;
				for(int j=0;j<(int)key_timevalue.size();++j)
				{
					//Mat4 m=toMat4(node->GetLocalTM(key_timevalue[j]));
					Mat4 m=getLocalTM(node,key_timevalue[j]);
					Vec3 pos;z3D::Core::Quat rot;Vec3 scale;
					m.factorSRT(scale,rot,pos);

					writer.write((float)(key_timevalue[j]*ticks_to_sec));
					writer.write(&pos,sizeof(pos));
				}
				wcs path = getAnimDataDir() + L"tm_pos_" + MBSTOWCS(node->GetName())+L".bin";
				wcs save_path = export_base_dir+L"/"+path;
				FileSystem::getFileSystem(L"WIN32")->saveFile(save_path, writer.retrieveAll());
				pos_path=path;
				pos_path=ESCSTR(pos_path);
			}

			getNodeRotationKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			if(key_timevalue.size())
			{
				MemWriter writer;
				for(int j=0;j<(int)key_timevalue.size();++j)
				{
					//Mat4 m=toMat4(node->GetLocalTM(key_timevalue[j]));
					Mat4 m=getLocalTM(node,key_timevalue[j]);
					Vec3 pos;z3D::Core::Quat rot;Vec3 scale;
					m.factorSRT(scale,rot,pos);

					/*
					if(isCameraNode(node) || isLightNode(node))
						rot=z3D::Core::Quat(Mat3::rotation(Vec3::xAxis,-90)*rot.asMat3());
						*/

					writer.write((float)(key_timevalue[j]*ticks_to_sec));
					writer.write(&rot,sizeof(rot));
				}
				wcs path=getAnimDataDir()+L"tm_rot_"+MBSTOWCS(node->GetName())+L".bin";
				wcs save_path=export_base_dir+L"/"+path;
				FileSystem::getFileSystem(L"WIN32")->saveFile(save_path, writer.retrieveAll());
				rot_path=path;
				rot_path=ESCSTR(rot_path);
			}

			getNodeScaleKeyFrameTimeValue(node->GetMaxNode(),key_timevalue);
			if(key_timevalue.size())
			{
				MemWriter writer;
				for(int j=0;j<(int)key_timevalue.size();++j)
				{
					//Mat4 m=toMat4(node->GetLocalTM(key_timevalue[j]));
					Mat4 m=getLocalTM(node,key_timevalue[j]);
					Vec3 pos;z3D::Core::Quat rot;Vec3 scale;
					m.factorSRT(scale,rot,pos);

					writer.write((float)(key_timevalue[j]*ticks_to_sec));
					writer.write(&scale,sizeof(scale));
				}
				wcs path=getAnimDataDir()+L"tm_scale_"+MBSTOWCS(node->GetName())+L".bin";
				wcs save_path=export_base_dir+L"/"+path;
				FileSystem::getFileSystem(L"WIN32")->saveFile(save_path, writer.retrieveAll());
				scale_path=path;
				scale_path=ESCSTR(scale_path);
			}

			output+=FORMATW(L"create_tm_controller \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"\r\n",(wcs(L"tm_")+MBSTOWCS(node->GetName())).c_str(),MBSTOWCS(node->GetName()).c_str(),ESCSTR(pos_path).c_str(),ESCSTR(rot_path).c_str(),ESCSTR(scale_path).c_str());
		}
		catch(...)
		{
			syslog << wcs(L"exception occurs while exporting keyframe data of node ") + MBSTOWCS(node->GetName());
		}
	}
	MemWriter writer;
	writer.write((unsigned short)UNICODE_BYTE_ORDER_MARK);
	writer.write((void*)output.c_str(),output.length()*2);

	FileSystem::getFileSystem(L"WIN32")->saveFile(export_base_dir + wcs(L"/") + getMeshDir() + L"scene_tm.txt", writer.retrieveAll());
}

void exportTMController3(const vector<IGameNode*>& array_node)
{
	class pos_getter: public KeyGetter<Vec3>
	{
	public:
		IGameNode*			node;
		pos_getter(IGameNode* node_): node(node_) {}
		Vec3				get(TimeValue t)
		{
			Mat4 m = getLocalTM(node, t);
			Vec3 pos; z3D::Core::Quat rot; Vec3 scale;
			m.factorSRT(scale, rot, pos);
			return pos;
		}
	};
	class rot_getter: public KeyGetter<z3D::Core::Quat>
	{
	public:
		IGameNode*			node;
		rot_getter(IGameNode* node_): node(node_) {}
		z3D::Core::Quat		get(TimeValue t)
		{
			Mat4 m = getLocalTM(node, t);
			Vec3 pos; z3D::Core::Quat rot; Vec3 scale;
			m.factorSRT(scale, rot, pos);
			return rot;
		}
	};
	class scale_getter: public KeyGetter<Vec3>
	{
	public:
		IGameNode*			node;
		scale_getter(IGameNode* node_): node(node_) {}
		Vec3				get(TimeValue t)
		{
			Mat4 m = getLocalTM(node, t);
			Vec3 pos; z3D::Core::Quat rot; Vec3 scale;
			m.factorSRT(scale, rot, pos);
			return scale;
		}
	};

	wcs path = getAnimDataDirREL() +L"/" + L"tm_scene.bin";
	wcs save_path = getExportBaseDir() + L"/" + path;

	KFWriter kfwriter;

	kfwriter.getWriter().write("ZANI", 4);
	kfwriter.getWriter().write((uint32_t)1);

	TiXmlDocument doc;

	doc.InsertEndChild(TiXmlDeclaration("1.0", "utf-8", ""));

	TiXmlHandle root(doc.InsertEndChild(TiXmlElement("tms")));

	map<TimeValue, bool> test_keyframe;

	for(size_t i = 0; i < array_node.size(); ++i)
	{
		IGameNode* node = array_node[i];

		//if(!anyKeyFrame(node->GetMaxNode()))
		//	continue;

		test_keyframe.clear();
		enumKeyTimeValueRecur(node->GetMaxNode()->GetTMController(), test_keyframe);
		if(!test_keyframe.size())
		{
			if(!ExportOptions::sample_every_frame)
				continue;
			if(true)
			{
				Interval anim_interval = GetCOREInterface()->GetAnimRange();
				Mat4 inv_tm = getLocalTM(node, anim_interval.Start()).inverse();
				int frame_ticks = GetTicksPerFrame();
				bool do_export = false;
				for(TimeValue t = anim_interval.Start() + frame_ticks; t <= anim_interval.End(); t += frame_ticks)
				{
					Mat4 tm2 = getLocalTM(node, t);
					if(!(tm2 * inv_tm).testIdentity(0.0001f))
					{
						do_export = true;
						break;
					}
				}
				if(!do_export)
					continue;
			}
		}

		zTiXmlElement& e = *(zTiXmlElement*)root.ToElement()->InsertEndChild(TiXmlElement("tm"));
		e.SetAttribute("node", MBSTOWCS(node->GetName()));

		if(node->GetMaxNode()->GetTMController())
		{
			size_t chunk_idx;

			Animatable* animatable;

			animatable = node->GetMaxNode()->GetTMController()->GetPositionController();
			animatable = animatable ? animatable : node->GetMaxNode()->GetTMController();
			if(kfwriter.exportChunk(animatable, pos_getter(node), chunk_idx, true))
			{
				zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("pos"));
				e2.SetAttribute("src", path);
				e2.SetAttribute("chunk", (int32_t)chunk_idx);
			}

			animatable = node->GetMaxNode()->GetTMController()->GetRotationController();
			animatable = animatable ? animatable : node->GetMaxNode()->GetTMController();
			if(kfwriter.exportChunk(animatable, rot_getter(node), chunk_idx, true))
			{
				zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("rot"));
				e2.SetAttribute("src", path);
				e2.SetAttribute("chunk", (int32_t)chunk_idx);
			}

			animatable = node->GetMaxNode()->GetTMController()->GetScaleController();
			animatable = animatable ? animatable : node->GetMaxNode()->GetTMController();
			if(kfwriter.exportChunk(animatable, scale_getter(node), chunk_idx, true))
			{
				zTiXmlElement& e2 = *(zTiXmlElement*)e.InsertEndChild(TiXmlElement("scale"));
				e2.SetAttribute("src", path);
				e2.SetAttribute("chunk", (int32_t)chunk_idx);
			}
		}

		if(!e.FirstChildElement())
			root.ToElement()->RemoveChild(&e);
	}

	if(kfwriter.chunks())
		FileSystem::getFileSystem(L"WIN32")->saveFile(save_path, kfwriter.getWriter().retrieveAll());

	wcs xml_path = getMeshDirABS() + L"/" + L"scene_tm.xml";
	if(true)
	{
		FILE* fp = NULL;
		_wfopen_s(&fp, xml_path.c_str(), L"w");
		if(fp)
		{
			doc.SaveFile(fp);

			fclose(fp);
		}
	}
}

