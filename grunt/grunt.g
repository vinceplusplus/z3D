grammar grunt;

options
{
	language = C;
	output = AST;
	ASTLabelType = pANTLR3_BASE_TREE;
}

tokens
{
	OP_UNARY_MINUS;
	OP_ASSIGN;
	OP_ADD_ASSIGN;
	OP_SUB_ASSIGN;
	OP_MUL_ASSIGN;
	OP_DIV_ASSIGN;
	OP_MOD_ASSIGN;
	OP_SHIFT_LEFT_ASSIGN;
	OP_SHIFT_RIGHT_ASSIGN;
	OP_AND_ASSIGN;
	OP_OR_ASSIGN;
	OP_XOR_ASSIGN;

	OP_CONDITIONAL;

	OP_LOGICAL_OR;
	OP_LOGICAL_AND;

	OP_BITWISE_OR;
	OP_BITWISE_XOR;
	OP_BITWISE_AND;

	OP_NOT_EQUAL;
	OP_EQUAL;

	OP_LESS;
	OP_LESS_EQUAL;
	OP_GREATER;
	OP_GREATER_EQUAL;

	OP_SHIFT_LEFT;
	OP_SHIFT_RIGHT;

	OP_ADD;
	OP_SUB;
	OP_MOD;
	OP_MUL;
	OP_DIV;

	OP_POST_INCR;
	OP_POST_DECR;
	OP_BITWISE_NOT;
	OP_LOGICAL_NOT;
	OP_NEGATE;
	OP_PRE_INCR;
	OP_PRE_DECR;

	OP_ELEMENT;
	OP_INDEX;
	OP_CALL_FUNC;

	COMMA_BLOCK;
	CP_BLOCK;

	ARGUMENT_LIST;
	PARAMETER_LIST;

	IF_STAT;

	SWITCH_STAT;
	SWITCH_CASE_LABEL;

	FOR_STAT;
	WHILE_STAT;
	DO_WHILE_STAT;

	LOCAL_DECLARATION_STAT;

	EMPTY_EXPR;

	BREAK_STAT;
	CONTINUE_STAT;

	FUNC_DEF;
	RETURN_STAT;

	THIS_FUNC;

	DICT_VALUE;
	DICT_ELEM_VALUE;
}

@header
{

extern "C++"
{

#include "../z3D/Core/Header.h"
using namespace z3D::Core;
#include "../z3D/Core/z_off_debug_new.h"

	wcs		grunt_unescape_string(const wcs& s);
}

}

@members
{
	static void	patch_STRING_LITERAL(pANTLR3_COMMON_TOKEN token, pgruntParser ctx)
	{
		wcs s = grunt_unescape_string(wcs((const wchar_t*)token->getText(token)->chars + 1, (size_t)(token->getText(token)->len - 2)));
		token->setText(token, ctx->adaptor->strFactory->newStr(ctx->adaptor->strFactory, (ANTLR3_UINT8*)s.c_str()));
	}
}

prog
@after
{
	ANTLR3_BASE_TREE* tree = $prog.tree;
	if(tree)
	{
		ANTLR3_STRING* s = tree->toStringTree(tree);
		_putws(wstring((const wchar_t*)s->chars, s->len).c_str());
	}
}

	:	(stat)*
	;

stat
	:	(comma_stat ';') => comma_stat ';'	-> comma_stat
	|	compound_stat
	|	selection_stat
	|	iteration_stat
	|	local_declaration_stat ';'		-> local_declaration_stat
	|	';'					-> EMPTY_EXPR
	|	'break' ';'				-> BREAK_STAT
	|	'continue' ';'				-> CONTINUE_STAT
	|	'return' expr? ';'			-> ^(RETURN_STAT expr?)
	;

local_declaration_stat
	:	'local' (IDENTIFIER ('=' expr)? -> ^(LOCAL_DECLARATION_STAT IDENTIFIER expr)) (',' IDENTIFIER ('=' expr)? -> $local_declaration_stat ^(LOCAL_DECLARATION_STAT IDENTIFIER expr))*
	;

function_definition
	:	'function' parameter_list compound_stat
		-> ^(FUNC_DEF parameter_list compound_stat)
	;

selection_stat
	:	if_stat
	|	switch_stat
	;

if_stat
	:	'if' '(' expr ')' s1 = stat ((('else') => 'else' s2 = stat) | )
		-> ^(IF_STAT expr $s1 $s2)
	;

switch_stat
	:	'switch' '(' (expr -> expr) ')' '{' ((switch_case_label -> $switch_stat switch_case_label) | (stat -> $switch_stat stat))* '}' -> ^(SWITCH_STAT $switch_stat)
	;

switch_case_label
	:	'case' expr ':'		-> ^(SWITCH_CASE_LABEL expr)
	|	'default' ':'		-> SWITCH_CASE_LABEL
	;

iteration_stat
	:	for_stat
	|	while_stat
	|	do_while_stat
	;

for_stat
	:	'for' '(' for_init_expr ';' for_cond_expr ';' for_loop_expr ')' stat
		-> ^(FOR_STAT for_init_expr for_cond_expr for_loop_expr stat)
	;

for_init_expr
	:	local_declaration_stat
	|	comma_stat
	|	->	EMPTY_EXPR
	;

for_cond_expr
	:	expr
	|	->	EMPTY_EXPR
	;

for_loop_expr
	:	comma_stat
	|	->	EMPTY_EXPR
	;

while_stat
	:	'while' '(' expr ')' stat
		-> ^(WHILE_STAT expr stat)
	;

do_while_stat
	:	'do' compound_stat 'while' '(' expr ')' ';'
		-> ^(DO_WHILE_STAT compound_stat expr)
	;

compound_stat
	:	'{' stat* '}' -> ^(CP_BLOCK stat*)
	;

comma_stat
	:	(e += expr -> expr) (',' e += expr  -> ^(COMMA_BLOCK $e+))*
	;

parameter_list
	:	'(' ( -> ^(PARAMETER_LIST) | (p += IDENTIFIER (',' p += IDENTIFIER)*) -> ^(PARAMETER_LIST $p*)) ')'
	;

argument_list
	:	'(' ( | a += expr (',' a += expr)*) ')' -> ^(ARGUMENT_LIST $a*)
	;

expr
	:	assignment_expr
	;

assignment_expr
	:	conditional_expr (
				-> conditional_expr
			|	('=' assignment_expr -> ^(OP_ASSIGN conditional_expr assignment_expr))
			|	('+=' assignment_expr -> ^(OP_ADD_ASSIGN conditional_expr assignment_expr))
			|	('-=' assignment_expr -> ^(OP_SUB_ASSIGN conditional_expr assignment_expr))
			|	('*=' assignment_expr -> ^(OP_MUL_ASSIGN conditional_expr assignment_expr))
			|	('/=' assignment_expr -> ^(OP_DIV_ASSIGN conditional_expr assignment_expr))
			|	('%=' assignment_expr -> ^(OP_MOD_ASSIGN conditional_expr assignment_expr))
			|	('<<=' assignment_expr -> ^(OP_SHIFT_LEFT_ASSIGN conditional_expr assignment_expr))
			|	('>>=' assignment_expr -> ^(OP_SHIFT_RIGHT_ASSIGN conditional_expr assignment_expr))
			|	('&=' assignment_expr -> ^(OP_AND_ASSIGN conditional_expr assignment_expr))
			|	('|=' assignment_expr -> ^(OP_OR_ASSIGN conditional_expr assignment_expr))
			|	('^=' assignment_expr -> ^(OP_XOR_ASSIGN conditional_expr assignment_expr))
		)
	;

conditional_expr
	:	(logical_or_expr -> logical_or_expr) ('?' expr ':' conditional_expr -> ^(OP_CONDITIONAL logical_or_expr expr conditional_expr))?
	;

logical_or_expr
	:	(logical_and_expr -> logical_and_expr) ('||' logical_and_expr -> ^(OP_LOGICAL_OR $logical_or_expr logical_and_expr))*
	;

logical_and_expr
	:	(bitwise_or_expr -> bitwise_or_expr) ('&&' bitwise_or_expr -> ^(OP_LOGICAL_AND $logical_and_expr bitwise_or_expr))*
	;

bitwise_or_expr
	:	(bitwise_xor_expr -> bitwise_xor_expr) ('|' bitwise_xor_expr -> ^(OP_BITWISE_OR $bitwise_or_expr bitwise_xor_expr))*
	;

bitwise_xor_expr
	:	(bitwise_and_expr -> bitwise_and_expr) ('^' bitwise_and_expr -> ^(OP_BITWISE_XOR $bitwise_xor_expr bitwise_and_expr))*
	;

bitwise_and_expr
	:	(equality_expr -> equality_expr) ('&' equality_expr -> ^(OP_BITWISE_AND $bitwise_and_expr equality_expr))*
	;

equality_expr
	:	(inequality_expr -> inequality_expr) (
				('==' inequality_expr -> ^(OP_EQUAL $equality_expr inequality_expr))
			|	('!=' inequality_expr -> ^(OP_NOT_EQUAL $equality_expr inequality_expr))
		)*
	;

inequality_expr
	:	(bitwise_shift_expr -> bitwise_shift_expr) (
				('<' bitwise_shift_expr -> ^(OP_LESS $inequality_expr bitwise_shift_expr))
			|	('<=' bitwise_shift_expr -> ^(OP_LESS_EQUAL $inequality_expr bitwise_shift_expr))
			|	('>' bitwise_shift_expr -> ^(OP_GREATER $inequality_expr bitwise_shift_expr))
			|	('>=' bitwise_shift_expr -> ^(OP_GREATER_EQUAL $inequality_expr bitwise_shift_expr))
		)*
	;

bitwise_shift_expr
	:	(add_expr -> add_expr) (
				('<<' add_expr -> ^(OP_SHIFT_LEFT $bitwise_shift_expr add_expr))
			|	('>>' add_expr -> ^(OP_SHIFT_RIGHT $bitwise_shift_expr add_expr))
		)*
	;

add_expr
	:	(mul_expr -> mul_expr) (
				('+' mul_expr -> ^(OP_ADD $add_expr mul_expr))
			|	('-' mul_expr -> ^(OP_SUB $add_expr mul_expr))
		)*
	;

mul_expr
	:	(pre_unary_expr -> pre_unary_expr) (
				('%' pre_unary_expr -> ^(OP_MOD $mul_expr pre_unary_expr))
			|	('*' pre_unary_expr -> ^(OP_MUL $mul_expr pre_unary_expr))
			|	('/' pre_unary_expr -> ^(OP_DIV $mul_expr pre_unary_expr))
		)*
	;

// tested ++i++ in c++ compiler to solve the ambiguity of the same precedence of pre and post unary operator

pre_unary_expr
	:	post_unary_expr
	|	'~' pre_unary_expr	->	^(OP_BITWISE_NOT pre_unary_expr)
	|	'!' pre_unary_expr	->	^(OP_LOGICAL_NOT pre_unary_expr)
	|	'-' pre_unary_expr	->	^(OP_NEGATE pre_unary_expr)
	|	'+' pre_unary_expr	->	pre_unary_expr
	|	'++' pre_unary_expr	->	^(OP_PRE_INCR pre_unary_expr)
	|	'--' pre_unary_expr	->	^(OP_PRE_DECR pre_unary_expr)
	;

post_unary_expr
	:	(primary_expr -> primary_expr) (
				'++' -> ^(OP_POST_INCR $post_unary_expr)
			|	'--' -> ^(OP_POST_DECR $post_unary_expr)
			|	'.' IDENTIFIER -> ^(OP_ELEMENT $post_unary_expr IDENTIFIER)
			|	'[' expr ']' -> ^(OP_INDEX $post_unary_expr expr)
			|	argument_list -> ^(OP_CALL_FUNC $post_unary_expr argument_list)
		)*
	;



primary_expr
	:	string_literal_rule
	|	integer_literal_rule
	|	float_literal_rule
	|	'(' expr ')'		->	expr
	|	IDENTIFIER
	|	function_definition
	|	'__this_func__'		->	THIS_FUNC
	|	dictionary_value
	;

dictionary_value
	:	'{' ( -> ^(DICT_VALUE) | (('.' (k = IDENTIFIER | k = STRING_LITERAL {patch_STRING_LITERAL($k, ctx);}) '=' expr -> ^(DICT_ELEM_VALUE $k expr)) (',' '.' (k = IDENTIFIER | k = STRING_LITERAL {patch_STRING_LITERAL($k, ctx);}) '=' expr -> $dictionary_value ^(DICT_ELEM_VALUE $k expr))* -> ^(DICT_VALUE $dictionary_value))) '}'
	;

string_literal_rule
	:	STRING_LITERAL
		{
			patch_STRING_LITERAL($STRING_LITERAL, ctx);
		}
	;

IDENTIFIER
	:	('_' | 'a'..'z' | 'A'..'Z') ('_' | '0'..'9' | 'a'..'z' | 'A'..'Z')*
	;

// DO NOT directly use STRING_LITERAL, please see string_literal_rule for the reason
STRING_LITERAL
	:	'"' ((STRING_BASIC_CHAR | STRING_ESC_SEQ)*) '"'
	;

integer_literal_rule
	:	DECIMAL_INTEGER_LITERAL
	|	HEXADECIMAL_INTEGER_LITERAL
	;

DECIMAL_INTEGER_LITERAL
	:	'0'
	|	'1'..'9' ('0'..'9')*
	;

HEXADECIMAL_INTEGER_LITERAL
	:	'0x' ('0'..'9' | 'a'..'f' | 'A'..'F')+
	;

float_literal_rule
	:	FLOAT_LITERAL
	;

FLOAT_LITERAL
	:
		'.' ('0'..'9')+ FLOAT_EXPONENT? ('f' | 'F')?
	|	('0'..'9')+	(
					('.' ('0'..'9')* FLOAT_EXPONENT? ('f' | 'F')?)
				|	(FLOAT_EXPONENT ('f' | 'F')?)
				|	(('f' | 'F'))
				)
	;

fragment
FLOAT_EXPONENT
	:	('e' | 'E') ('+' | '-')? ('0'..'9')+
	;

fragment
STRING_BASIC_CHAR
	:	~('"' | '\\')
	;

fragment
STRING_ESC_SEQ
	:	'\\"'
	|	'\\\\'
	|	'\\\''
	|	'\\r'
	|	'\\n'
	|	'\\t'
	|	'\\u' ('0'..'9' | 'a'..'f' | 'A'..'F') ('0'..'9' | 'a'..'f' | 'A'..'F') ('0'..'9' | 'a'..'f' | 'A'..'F') ('0'..'9' | 'a'..'f' | 'A'..'F')
	;

SL_COMMENT
	:	'//' (~'\n')* '\n'?
		{
			$channel = HIDDEN;
		}
	;

ML_COMMENT
	:	'/*' (options{greedy = false;} : .)* '*/'
		{
			$channel = HIDDEN;
		}
	;

WS
	:	(' ' | '\t' | '\r' | '\n')+ {$channel = HIDDEN;}
	;

fragment
INCLUDE_FILENAME
	:	(STRING_BASIC_CHAR | STRING_ESC_SEQ)+
	;

LINE_COMMAND
	:	'#' (' ' | '\t' | '\r' | '\n')*
		(
			(
				'include' (' ' | '\t' | '\r' | '\n')* '"' fn = INCLUDE_FILENAME '"'
				{
					ANTLR3_STRING* fn_str = $fn.text;
					wprintf(L"including: \%s\n", fn_str->chars);
					fn_str = fn_str->to8(fn_str);
					ANTLR3_INPUT_STREAM* is = antlr3AsciiFileStreamNew(fn_str->chars);
					if(is)
						PUSHSTREAM(is);
				}
			)
		)
		{$channel = HIDDEN;}
	;