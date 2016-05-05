
#pragma warning(disable:4819)

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#ifdef _DEBUG
	#include <assert.h>
#endif

#include "maxscrpt/maxscrpt.h"
#include "maxscrpt/maxobj.h"
#include "modstack.h"

#include "MeshExport.h"

INode*
_get_valid_node(MAXNode* _node, TCHAR* errmsg) {
	if (is_node(_node))
	{	if(deletion_check_test(_node))
			throw RuntimeError("Attempted to access to deleted object");
	}
	else
		throw RuntimeError (errmsg);
	return _node->to_node();
}

#define get_valid_node(_node, _fn) _get_valid_node((MAXNode*)_node, #_fn##" requires a node")

/* --------------------- Node reset transform methods --------------------------------- */

#define CLUSTOSM_CLASS_ID			0x25215824

Value*
ResetXForm_cf(Value** arg_list, int count)
{
	// ResetXForm <node>
	check_arg_count(ResetXForm, 1, count);
	INode* node = get_valid_node((MAXNode*)arg_list[0], ResetXForm);
	TimeValue t = MAXScript_time();

	Matrix3 ntm, ptm, rtm(1), piv(1), tm;

	Object*			obj = node->GetObjectRef();

	SimpleMod *mod = (SimpleMod*)MAXScript_interface->CreateInstance(OSM_CLASS_ID,Class_ID(CLUSTOSM_CLASS_ID,0));
	if (mod == NULL)
		return &false_value;

	// Get Parent and Node TMs
	ntm = node->GetNodeTM(t);
	ptm = node->GetParentTM(t);

	// Compute the relative TM
	ntm = ntm * Inverse(ptm);

	// The reset TM only inherits position
	rtm.SetTrans(ntm.GetTrans());

	// Set the node TM to the reset TM		
	tm = rtm*ptm;
	node->SetNodeTM(t, tm);

	// Compute the pivot TM
	piv.SetTrans(node->GetObjOffsetPos());
	PreRotateMatrix(piv,node->GetObjOffsetRot());
	ApplyScaling(piv,node->GetObjOffsetScale());

	// Reset the offset to 0
	node->SetObjOffsetPos(Point3(0,0,0));
	node->SetObjOffsetRot(IdentQuat());
	node->SetObjOffsetScale(ScaleValue(Point3(1,1,1)));

	// Take the position out of the matrix since we don't reset position
	ntm.NoTrans();

	// Apply the offset to the TM
	ntm = piv * ntm;

	// Apply a derived object to the node's object
	IDerivedObject *dobj = CreateDerivedObject(obj);

	// Apply the transformation to the mod.
	SetXFormPacket pckt(ntm);
	mod->tmControl->SetValue(t,&pckt);

	BOOL was_editing = MAXScript_interface->GetCommandPanelTaskMode() == TASK_MODE_MODIFY &&
		MAXScript_interface->GetSelNodeCount() > 0 ; // && node->Selected();

	// clear any problem command modes
	MAXScript_interface->StopCreating();
	MAXScript_interface->ClearPickMode();
	if (was_editing)
		MAXScript_interface7->SuspendEditing();

	// Add the bend modifier to the derived object.
	dobj->AddModifier(mod);

	// Replace the node's object
	node->SetObjectRef(dobj);

	needs_redraw_set();
	SetSaveRequiredFlag(TRUE);

	MAXScript_interface7->InvalidateObCache(node);

	dobj->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	node->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	node->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);

	if (was_editing)
		MAXScript_interface7->ResumeEditing();

	return &true_value;
}

void resetXForm(INode* node)
{
	MAXNode n(node);
	Value* v=&n;
	ResetXForm_cf(&v,1);
}
