tree grammar gruntEval;

options
{
	language = C;
	tokenVocab = grunt;
	ASTLabelType = pANTLR3_BASE_TREE;
}

@header
{
extern "C++"
{
#include "gruntRuntime.h"
#include "../z3D/Core/functor_inl.h"
#include "../z3D/Core/z_off_debug_new.h"
};
}

@members
{
	static ANTLR3_INT32		getCurNodeIndex(pgruntEval ctx)
	{
		ANTLR3_TREE_PARSER* parser = PARSER;
		return parser->getTreeNodeStream(parser)->p;
	}
	template <class T>
	T				exec_rule(pgruntEval ctx, ANTLR3_INT32 node_index, T (*rule) (pgruntEval))
	{
		ANTLR3_TREE_PARSER* parser = PARSER;
		ANTLR3_COMMON_TREE_NODE_STREAM* tns = parser->getTreeNodeStream(parser);
		tns->push(tns, node_index);
		T ret = (*rule)(ctx);
		tns->pop(tns);
		return ret;
	}
	static void			exec_rule(pgruntEval ctx, ANTLR3_INT32 node_index, void (*rule) (pgruntEval))
	{
		ANTLR3_TREE_PARSER* parser = PARSER;
		ANTLR3_COMMON_TREE_NODE_STREAM* tns = parser->getTreeNodeStream(parser);
		tns->push(tns, node_index);
		(*rule)(ctx);
		tns->pop(tns);
	}
	static size_t			exec_in_switch_block(pgruntEval ctx, ANTLR3_INT32 node_index, gruntMemCell*& ret)
	{
		gruntMemCell* retval;
		retval = exec_rule(ctx, node_index, &stat);
		if(retval)
		{
			if(retval->getType() == gruntMemCell::TYPE_FLOWCTRL)
			{
				switch(retval->toFlowCtrl())
				{
				case gruntMemCell::FLOWCTRL_BREAK:
					retval->getPool()->deleteCell(retval->getIndex());
					return false;
				case gruntMemCell::FLOWCTRL_CONTINUE:
					ret = retval;
					return false;
				default:
					Z_ASSERT( false );
					break;
				}
			}
			else if(retval->isReturnValue())
			{
				ret = retval;
				return false;
			}
			else
			{
				Z_ASSERT( false );
			}
		}
		return true;
	}
#define RUNTIME ((gruntRuntime*)PSRSTATE->userp)
}

prog
	:	stat*
	;

// any single node or tree, work around for 3.1 ~ 3.1.2, and currently the grammar is written for 3.1.2
asnot
	:	^((~(UP | DOWN)) asnot*)
	|	.
	//:	.
	;

basic_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
	}
	:	(
			e = expr
		|	^(COMMA_BLOCK expr+)
		|	selection_stat			{$value = $selection_stat.value;}
		|	iteration_stat			{$value = $iteration_stat.value;}
		|	local_declaration_stat
		|	BREAK_STAT			{if(runtime.canBreak()) {$value = runtime.addCell(L""); $value->asFlowCtrl() = gruntMemCell::FLOWCTRL_BREAK;} else printf("can't break\n");}
		|	CONTINUE_STAT			{if(runtime.canContinue()) {$value = runtime.addCell(L""); $value->asFlowCtrl() = gruntMemCell::FLOWCTRL_CONTINUE;} else printf("can't continue\n");}
		|	return_stat			{$value = $return_stat.value;}
		) {runtime.clearTemp();}
	;

return_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 e_index = -1;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(RETURN_STAT ({e_index = getCurNodeIndex(ctx);} asnot)?)
		{
			if(runtime.canReturn())
			{
				if(e_index != -1)
				{
					gruntMemCell* ret = exec_rule(ctx, e_index, &expr);
					$value = runtime.addCell(L"");
					runtime.op_assign($value, ret);
				}
				else
					$value = runtime.addCell(L"");
				$value->flagAsReturnValue(true);
			}
			else
			{
				printf("can't return\n");
			}
		}
	;

stat
	returns [gruntMemCell* value = 0]
	:	basic_stat			{$value = $basic_stat.value;}
	|	compound_stat			{$value = $compound_stat.value;}
	;

stat_no_scope
	returns [gruntMemCell* value = 0]
	:	basic_stat			{$value = $basic_stat.value;}
	|	compound_stat_no_scope		{$value = $compound_stat_no_scope.value;}
	;

compound_stat_no_scope
	returns [gruntMemCell* value = 0]
	@init
	{
		size_t cont_exec = 1;
		gruntMemCell* ret = 0;
	}
	:	^(CP_BLOCK 
			(
			{
				if(cont_exec)
				{
					ret = exec_rule(ctx, getCurNodeIndex(ctx), &stat);
					if(ret)
					{
						if(ret->getType() == gruntMemCell::TYPE_FLOWCTRL /*currently, only break and continue for flow control*/ || ret->isReturnValue())
						{
							$value = ret;
							cont_exec = false;
						}
						else
						{
							Z_ASSERT( false );
						}
					}
				}
			} asnot)*
		)
	;

compound_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
	}
	:	{runtime.enterScope(gruntMemCell::SCOPE_BLOCK);} compound_stat_no_scope {$value = $compound_stat_no_scope.value;} {runtime.leaveScope();}
	;

local_declaration_stat
	@init
	{
		ANTLR3_INT32 e_index = -1;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(LOCAL_DECLARATION_STAT IDENTIFIER ({e_index = getCurNodeIndex(ctx);} expr))
		{
			gruntMemCell* c = runtime.addLocalCell((const wchar_t*)$IDENTIFIER.text->chars);
			if(e_index != -1)
				runtime.op_assign(c, $expr.value);
		}
	;

function_definition
	returns [gruntMemCell* value]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
	}
	:	{$value = runtime.addTempCell(); $value->asFunction() = (int32_t)getCurNodeIndex(ctx);} ^(FUNC_DEF asnot*)
	;

get_func_def
	returns [ANTLR3_INT32 param_start, ANTLR3_INT32 param_end, ANTLR3_INT32 block_index]
	:	^(FUNC_DEF parameter_list ({$block_index = getCurNodeIndex(ctx);} asnot))
		{
			$param_start = $parameter_list.param_start;
			$param_end = $parameter_list.param_end;
		}
	;

parameter_list
	returns [int32_t param_start, int32_t param_end]
	@init
	{
		$param_start = 0x7fffffff;
		$param_end = 0x80000000;
	}
	:	^(PARAMETER_LIST ({$param_start = min($param_start, (int32_t)getCurNodeIndex(ctx));} IDENTIFIER)* {$param_end = (int32_t)getCurNodeIndex(ctx);})
	;

selection_stat
	returns [gruntMemCell* value = 0]
	:	if_stat		{$value = $if_stat.value;}
	|	switch_stat	{$value = $switch_stat.value;}
	;

switch_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 ce_index = -1;
		ANTLR3_INT32 def_index = -1;
		size_t switched = 0;
		size_t broken = 0;
		gruntMemCell* expr_value;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(SWITCH_STAT {runtime.enterScope(gruntMemCell::SCOPE_SWITCH);} e = expr {expr_value = $e.value;}
			(
				(^(SWITCH_CASE_LABEL asnot)) => ^(SWITCH_CASE_LABEL ({if(!switched){ce_index = getCurNodeIndex(ctx); if(runtime.op_equal(expr_value, exec_rule(ctx, ce_index, &expr))->toBool()) switched = 1;}} asnot))
			|	(SWITCH_CASE_LABEL) => SWITCH_CASE_LABEL {def_index = getCurNodeIndex(ctx);}
			|	(
					{
						if(switched && !broken)
						{
							if(!exec_in_switch_block(ctx, getCurNodeIndex(ctx), $value))
								broken = 1;
						}
					}
				asnot)
			)*
			{if(!switched && def_index != -1){$value = exec_rule(ctx, def_index, &switch_default_exec);}}
			{runtime.leaveScope();}
		)
	;

switch_default_exec
	returns [gruntMemCell* value = 0]
	@init
	{
		size_t broken = 0;
	}
	:	(
			(^(SWITCH_CASE_LABEL asnot)) => ^(SWITCH_CASE_LABEL asnot)
		|	(SWITCH_CASE_LABEL) => SWITCH_CASE_LABEL
		|	(
				{
					if(!broken)
					{
						if(!exec_in_switch_block(ctx, getCurNodeIndex(ctx), $value))
							broken = 1;
					}
				}
			asnot)
		)* UP
	;

if_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 c_index = -1, t_index = -1, f_index = -1;
	}
	:	^(IF_STAT ({c_index = getCurNodeIndex(ctx);} asnot) ({t_index = getCurNodeIndex(ctx);} asnot) ({f_index = getCurNodeIndex(ctx);} asnot)?)
		{
			gruntMemCell* v = exec_rule(ctx, c_index, &expr);
			if(v->toBool())
			{
				$value = exec_rule(ctx, t_index, &stat);
			}
			else if(f_index != -1)
			{
				$value = exec_rule(ctx, f_index, &stat);
			}
		}
	;

iteration_stat
	returns [gruntMemCell* value = 0]
	:	for_stat	{$value = $for_stat.value;}
	|	while_stat	{$value = $while_stat.value;}
	|	do_while_stat	{$value = $do_while_stat.value;}
	;

do_while_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 c_index, s_index;
		gruntMemCell* ret = 0;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(DO_WHILE_STAT ({s_index = getCurNodeIndex(ctx);} asnot) ({c_index = getCurNodeIndex(ctx);} asnot))
		{
			runtime.enterScope(gruntMemCell::SCOPE_ITERATION);
			do
			{
				ret = exec_rule(ctx, s_index, &compound_stat_no_scope);
				if(ret)
				{
					if(ret->getType() == gruntMemCell::TYPE_FLOWCTRL)
					{
						if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_BREAK)
							break;
						else if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_CONTINUE)
						{// intentionally empty
						}
						else
						{
							Z_ASSERT( false );
						}
					}
					else if(ret->isReturnValue())
					{
						$value = ret;
						break;
					}
					else
					{
						Z_ASSERT( false );
					}
				}
			}while(exec_rule(ctx, c_index, &expr)->toBool());
			runtime.leaveScope();
		}
	;

while_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 c_index, s_index;
		gruntMemCell* ret = 0;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(WHILE_STAT ({c_index = getCurNodeIndex(ctx);} asnot) ({s_index = getCurNodeIndex(ctx);} asnot))
		{
			runtime.enterScope(gruntMemCell::SCOPE_ITERATION);
			for(;;)
			{
				if(!exec_rule(ctx, c_index, &expr)->toBool())
				{
					break;
				}
				ret = exec_rule(ctx, s_index, &stat_no_scope);
				if(ret)
				{
					if(ret->getType() == gruntMemCell::TYPE_FLOWCTRL)
					{
						if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_BREAK)
							break;
						else if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_CONTINUE)
						{// intentionally empty
						}
						else
						{
							Z_ASSERT( false );
						}
					}
					else if(ret->isReturnValue())
					{
						$value = ret;
						break;
					}
					else
					{
						Z_ASSERT( false );
					}
				}
			}
			runtime.leaveScope();
		}
	;

for_stat
	returns [gruntMemCell* value = 0]
	@init
	{
		ANTLR3_INT32 i_index, c_index, l_index, s_index;
		gruntMemCell* ret = 0;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(FOR_STAT ({i_index = getCurNodeIndex(ctx);} asnot) ({c_index = getCurNodeIndex(ctx);} asnot) ({l_index = getCurNodeIndex(ctx);} asnot) ({s_index = getCurNodeIndex(ctx);} asnot))
		{
			runtime.enterScope(gruntMemCell::SCOPE_ITERATION);

			ret = exec_rule(ctx, i_index, &stat);
			Z_ASSERT( !ret );

			for(;;)
			{
				gruntMemCell* v = exec_rule(ctx, c_index, &expr);
				if(v->toBool())
				{
					runtime.clearTemp();
					ret = exec_rule(ctx, s_index, &stat_no_scope);
					if(ret)
					{
						if(ret->getType() == gruntMemCell::TYPE_FLOWCTRL)
						{
							if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_BREAK)
							{
								ret->getPool()->deleteCell(ret->getIndex());
								break;
							}
							else if(ret->toFlowCtrl() == gruntMemCell::FLOWCTRL_CONTINUE)
							{
								ret->getPool()->deleteCell(ret->getIndex());
							}
							else
							{
								Z_ASSERT( false );
							}
						}
						else if(ret->isReturnValue())
						{
							$value = ret;
							break;
						}
					}

					exec_rule(ctx, l_index, &stat);
				}
				else
					break;
			}
			runtime.leaveScope();
		}
	;

expr
	returns [gruntMemCell* value = 0]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
	}
	:	EMPTY_EXPR					{$value = runtime.addTempCell(); $value->asInteger() = 1;}
	|	conditional_expr				{$value = $conditional_expr.value;}
	|	^(OP_ASSIGN e0=expr e1=expr)			{$value = runtime.op_assign($e0.value, $e1.value);}
	|	^(OP_ADD_ASSIGN e0=expr e1=expr)		{$value = runtime.op_add_assign($e0.value, $e1.value);}
	|	^(OP_SUB_ASSIGN e0=expr e1=expr)		{$value = runtime.op_sub_assign($e0.value, $e1.value);}
	|	^(OP_MUL_ASSIGN e0=expr e1=expr)		{$value = runtime.op_mul_assign($e0.value, $e1.value);}
	|	^(OP_DIV_ASSIGN e0=expr e1=expr)		{$value = runtime.op_div_assign($e0.value, $e1.value);}
	|	^(OP_MOD_ASSIGN e0=expr e1=expr)		{$value = runtime.op_mod_assign($e0.value, $e1.value);}
	|	^(OP_SHIFT_LEFT_ASSIGN e0=expr e1=expr)		{$value = runtime.op_shift_left_assign($e0.value, $e1.value);}
	|	^(OP_SHIFT_RIGHT_ASSIGN e0=expr e1=expr)	{$value = runtime.op_shift_right_assign($e0.value, $e1.value);}
	|	^(OP_AND_ASSIGN e0=expr e1=expr)		{$value = runtime.op_and_assign($e0.value, $e1.value);}
	|	^(OP_OR_ASSIGN e0=expr e1=expr)			{$value = runtime.op_or_assign($e0.value, $e1.value);}
	|	^(OP_XOR_ASSIGN e0=expr e1=expr)		{$value = runtime.op_xor_assign($e0.value, $e1.value);}
	|	^(OP_LOGICAL_OR e0=expr e1=expr)		{$value = runtime.op_logical_or($e0.value, $e1.value);}
	|	^(OP_LOGICAL_AND e0=expr e1=expr)		{$value = runtime.op_logical_and($e0.value, $e1.value);}
	|	^(OP_BITWISE_OR e0=expr e1=expr)		{$value = runtime.op_bitwise_or($e0.value, $e1.value);}
	|	^(OP_BITWISE_XOR e0=expr e1=expr)		{$value = runtime.op_bitwise_xor($e0.value, $e1.value);}
	|	^(OP_BITWISE_AND e0=expr e1=expr)		{$value = runtime.op_bitwise_and($e0.value, $e1.value);}
	|	^(OP_EQUAL e0=expr e1=expr)			{$value = runtime.op_equal($e0.value, $e1.value);}
	|	^(OP_NOT_EQUAL e0=expr e1=expr)			{$value = runtime.op_not_equal($e0.value, $e1.value);}
	|	^(OP_LESS e0=expr e1=expr)			{$value = runtime.op_less($e0.value, $e1.value);}
	|	^(OP_LESS_EQUAL e0=expr e1=expr)		{$value = runtime.op_less_equal($e0.value, $e1.value);}
	|	^(OP_GREATER e0=expr e1=expr)			{$value = runtime.op_greater($e0.value, $e1.value);}
	|	^(OP_GREATER_EQUAL e0=expr e1=expr)		{$value = runtime.op_greater_equal($e0.value, $e1.value);}
	|	^(OP_SHIFT_LEFT e0=expr e1=expr)		{$value = runtime.op_shift_left($e0.value, $e1.value);}
	|	^(OP_SHIFT_RIGHT e0=expr e1=expr)		{$value = runtime.op_shift_right($e0.value, $e1.value);}
	|	^(OP_ADD e0=expr e1=expr)			{$value = runtime.op_add($e0.value, $e1.value);}
	|	^(OP_SUB e0=expr e1=expr)			{$value = runtime.op_sub($e0.value, $e1.value);}
	|	^(OP_MOD e0=expr e1=expr)			{$value = runtime.op_mod($e0.value, $e1.value);}
	|	^(OP_MUL e0=expr e1=expr)			{$value = runtime.op_mul($e0.value, $e1.value);}
	|	^(OP_DIV e0=expr e1=expr)			{$value = runtime.op_div($e0.value, $e1.value);}
	|	^(OP_BITWISE_NOT e0=expr)			{$value = runtime.op_bitwise_not($e0.value);}
	|	^(OP_LOGICAL_NOT e0=expr)			{$value = runtime.op_logical_not($e0.value);}
	|	^(OP_NEGATE e0=expr)				{$value = runtime.op_negate($e0.value);}
	|	^(OP_PRE_INCR e0=expr)				{$value = runtime.op_pre_incr($e0.value);}
	|	^(OP_PRE_DECR e0=expr)				{$value = runtime.op_pre_decr($e0.value);}
	|	^(OP_POST_INCR e0=expr)				{$value = runtime.op_post_incr($e0.value);}
	|	^(OP_POST_DECR e0=expr)				{$value = runtime.op_post_decr($e0.value);}
	|	^(OP_ELEMENT e0=expr IDENTIFIER)		{$value = runtime.op_element($e0.value, (const wchar_t*)$IDENTIFIER.text->chars);}
	|	^(OP_INDEX e0=expr e1=expr)			{$value = runtime.op_index($e0.value, $e1.value);}
	|	call_func_expr					{$value = $call_func_expr.value;}
	|	STRING_LITERAL					{$value = runtime.addTempCell(); $value->asString() = wcs((const wchar_t*)$STRING_LITERAL.text->chars, (size_t)$STRING_LITERAL.text->len);}
	|	DECIMAL_INTEGER_LITERAL				{$value = runtime.addTempCell(); $value->asString() = (const wchar_t*)$DECIMAL_INTEGER_LITERAL.text->chars; $value->asInteger();}
	|	HEXADECIMAL_INTEGER_LITERAL
	|	FLOAT_LITERAL					{$value = runtime.addTempCell(); $value->asString() = (const wchar_t*)$FLOAT_LITERAL.text->chars; $value->asFloat();}
	|	IDENTIFIER					{$value = runtime.getIdentifierCell((const wchar_t*)$IDENTIFIER.text->chars);}
	|	function_definition				{$value = $function_definition.value;}
	|	THIS_FUNC					{$value = runtime.getThisFunc(); if(!$value) $value = runtime.addTempCell();}
	|	dictionary_value				{$value = $dictionary_value.value;}
	;

conditional_expr
	returns [gruntMemCell* value = NULL]
	@init
	{
		ANTLR3_INT32 t_index, f_index;
	}
	:	^(OP_CONDITIONAL e0=expr ({t_index = getCurNodeIndex(ctx);} asnot) ({f_index = getCurNodeIndex(ctx);} asnot))
		{
			if($e0.value->toBool())
			{
				$value = exec_rule(ctx, t_index, &expr);
			}
			else
			{
				$value = exec_rule(ctx, f_index, &expr);
			}
		}
	;

call_func_expr
	returns [gruntMemCell* value]
	@init
	{
		ANTLR3_INT32 a_index;
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(OP_CALL_FUNC e0=expr ({a_index = getCurNodeIndex(ctx);} asnot))
		{
			if($e0.value->getType() == gruntMemCell::TYPE_FUNCTION)
			{
				gruntMemCell* al = exec_rule(ctx, a_index, &argument_list);
				gruntEval_get_func_def_return func_def = exec_rule(ctx, (ANTLR3_INT32)$e0.value->toFunction(), &get_func_def);
				runtime.enterScope(gruntMemCell::SCOPE_FUNCTION);
				runtime.op_assign(runtime.addLocalCell(L"\$fp"), $e0.value);
				ANTLR3_TREE_PARSER* parser = PARSER;
				ANTLR3_COMMON_TREE_NODE_STREAM* tns = parser->getTreeNodeStream(parser);
				ANTLR3_VECTOR* nodes = tns->nodes;
				size_t count = 0;
				for(ANTLR3_INT32 i = func_def.param_start; i < func_def.param_end; ++i)
				{
					ANTLR3_BASE_TREE* n = (ANTLR3_BASE_TREE*)nodes->get(nodes, i);
					ANTLR3_COMMON_TOKEN* token = n->getToken(n);
					ANTLR3_STRING* s = token->getText(token);
					gruntMemCell* a = runtime.addLocalCell((const wchar_t*)s->chars);
					runtime.op_assign(a, al->getElement(FORMATW(L"\%d", count)));
					++count;
				}
				al->getPool()->deleteCell(al->getIndex());
				gruntMemCell* ret = exec_rule(ctx, func_def.block_index, &compound_stat_no_scope);
				runtime.leaveScope();
				$value = runtime.addTempCell();
				if(ret)
				{
					runtime.op_assign($value, ret);
					ret->getPool()->deleteCell(ret->getIndex());
				}
			}
			else if($e0.value->getType() == gruntMemCell::TYPE_FUNCTION_BINDING)
			{
				gruntMemCell* al = exec_rule(ctx, a_index, &argument_list);

				gruntMemCell* ret = $e0.value->getFunctionBindingFunc()($e0.value->getFunctionBindingOpaque(), &runtime, al);
				$value = runtime.addTempCell();
				if(ret)
				{
					runtime.op_assign($value, ret);
					ret->getPool()->deleteCell(ret->getIndex());
				}

				al->getPool()->deleteCell(al->getIndex());
			}
			else
			{
				printf("not a function\n");
				$value = runtime.addTempCell();
			}
		}
	;

argument_list
	returns [gruntMemCell* value]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
		ptrdiff_t next_index = 0;
		$value = runtime.addCell(L"");
		$value->asDictionary();
	}
	:	^(ARGUMENT_LIST (expr {gruntMemCell* e = runtime.addCell(FORMATW(L"\%d", next_index)); ++next_index; runtime.op_assign(e, $expr.value);$value->addElement(e);})*)
	;

dictionary_value
	returns [gruntMemCell* value]
	@init
	{
		gruntRuntime& runtime = *RUNTIME;
	}
	:	^(DICT_VALUE
			{
				$value = runtime.addTempCell();
				$value->asDictionary();
			}
			(^(DICT_ELEM_VALUE (id = IDENTIFIER | s = STRING_LITERAL) expr
				{
					gruntMemCell* e;
					if($id)
						e = runtime.addCell(wcs((const wchar_t*)$id.text->chars, (size_t)$id.text->len));
					else
						e = runtime.addCell(wcs((const wchar_t*)$s.text->chars, (size_t)$s.text->len));
					runtime.op_assign(e, $expr.value);
					$value->addElement(e);
				}
			))*
		)
	;
