/** \file
 *  This C header file was generated by $ANTLR version 3.1.3 Mar 17, 2009 19:23:44
 *
 *     -  From the grammar source file : gruntEval.g
 *     -                            On : 2009-03-20 09:47:20
 *     -           for the tree parser : gruntEvalTreeParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The tree parser gruntEval has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pgruntEval, which is returned from a call to gruntEvalNew().
 *
 * The methods in pgruntEval are  as follows:
 *
 *  - void      pgruntEval->prog(pgruntEval)
 *  - void      pgruntEval->asnot(pgruntEval)
 *  - gruntMemCell*      pgruntEval->basic_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->return_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->stat_no_scope(pgruntEval)
 *  - gruntMemCell*      pgruntEval->compound_stat_no_scope(pgruntEval)
 *  - gruntMemCell*      pgruntEval->compound_stat(pgruntEval)
 *  - void      pgruntEval->local_declaration_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->function_definition(pgruntEval)
 *  - gruntEval_get_func_def_return      pgruntEval->get_func_def(pgruntEval)
 *  - gruntEval_parameter_list_return      pgruntEval->parameter_list(pgruntEval)
 *  - gruntMemCell*      pgruntEval->selection_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->switch_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->switch_default_exec(pgruntEval)
 *  - gruntMemCell*      pgruntEval->if_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->iteration_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->do_while_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->while_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->for_stat(pgruntEval)
 *  - gruntMemCell*      pgruntEval->expr(pgruntEval)
 *  - gruntMemCell*      pgruntEval->conditional_expr(pgruntEval)
 *  - gruntMemCell*      pgruntEval->call_func_expr(pgruntEval)
 *  - gruntMemCell*      pgruntEval->argument_list(pgruntEval)
 *  - gruntMemCell*      pgruntEval->dictionary_value(pgruntEval)
 * 
 * 
 * 
 * 
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_gruntEval_H
#define _gruntEval_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct gruntEval_Ctx_struct gruntEval, * pgruntEval;



extern "C++"
{
#include "gruntRuntime.h"
#include "../z3D/Core/z_off_debug_new.h"
};


#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */
typedef struct gruntEval_get_func_def_return_struct
{
    pANTLR3_BASE_TREE       start;
    pANTLR3_BASE_TREE       stop;   
    ANTLR3_INT32 param_start;
    ANTLR3_INT32 param_end;
    ANTLR3_INT32 block_index;
}
    gruntEval_get_func_def_return;

typedef struct gruntEval_parameter_list_return_struct
{
    pANTLR3_BASE_TREE       start;
    pANTLR3_BASE_TREE       stop;   
    int32_t param_start;
    int32_t param_end;
}
    gruntEval_parameter_list_return;



/** Context tracking structure for gruntEval
 */
struct gruntEval_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_TREE_PARSER	    pTreeParser;


     void (*prog)	(struct gruntEval_Ctx_struct * ctx);
     void (*asnot)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*basic_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*return_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*stat_no_scope)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*compound_stat_no_scope)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*compound_stat)	(struct gruntEval_Ctx_struct * ctx);
     void (*local_declaration_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*function_definition)	(struct gruntEval_Ctx_struct * ctx);
     gruntEval_get_func_def_return (*get_func_def)	(struct gruntEval_Ctx_struct * ctx);
     gruntEval_parameter_list_return (*parameter_list)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*selection_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*switch_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*switch_default_exec)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*if_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*iteration_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*do_while_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*while_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*for_stat)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*expr)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*conditional_expr)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*call_func_expr)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*argument_list)	(struct gruntEval_Ctx_struct * ctx);
     gruntMemCell* (*dictionary_value)	(struct gruntEval_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred1_gruntEval)	(struct gruntEval_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred2_gruntEval)	(struct gruntEval_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred3_gruntEval)	(struct gruntEval_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred4_gruntEval)	(struct gruntEval_Ctx_struct * ctx);
    // Delegated rules
    const char * (*getGrammarFileName)();
    void	    (*free)   (struct gruntEval_Ctx_struct * ctx);
        
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pgruntEval gruntEvalNew         (pANTLR3_COMMON_TREE_NODE_STREAM instream);
ANTLR3_API pgruntEval gruntEvalNewSSD      (pANTLR3_COMMON_TREE_NODE_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the tree parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define OP_BITWISE_AND      21
#define HEXADECIMAL_INTEGER_LITERAL      69
#define OP_LOGICAL_AND      18
#define OP_MUL_ASSIGN      8
#define SWITCH_CASE_LABEL      51
#define CP_BLOCK      46
#define EOF      -1
#define T__93      93
#define T__94      94
#define OP_MOD      32
#define T__91      91
#define T__92      92
#define STRING_LITERAL      65
#define T__90      90
#define OP_NOT_EQUAL      22
#define IF_STAT      49
#define OP_PRE_DECR      41
#define PARAMETER_LIST      48
#define T__99      99
#define OP_LOGICAL_OR      17
#define T__98      98
#define T__97      97
#define T__96      96
#define T__95      95
#define OP_GREATER_EQUAL      27
#define OP_LOGICAL_NOT      38
#define OP_GREATER      26
#define T__80      80
#define T__81      81
#define T__82      82
#define T__83      83
#define THIS_FUNC      61
#define OP_OR_ASSIGN      14
#define OP_ADD      30
#define SWITCH_STAT      50
#define FOR_STAT      52
#define OP_MOD_ASSIGN      10
#define OP_UNARY_MINUS      4
#define OP_DIV      34
#define FUNC_DEF      59
#define STRING_ESC_SEQ      67
#define T__85      85
#define T__84      84
#define T__87      87
#define OP_SUB_ASSIGN      7
#define T__86      86
#define T__89      89
#define T__88      88
#define T__126      126
#define WHILE_STAT      53
#define T__125      125
#define OP_POST_INCR      35
#define T__128      128
#define T__127      127
#define WS      74
#define T__129      129
#define COMMA_BLOCK      45
#define OP_XOR_ASSIGN      15
#define LINE_COMMAND      76
#define SL_COMMENT      72
#define OP_EQUAL      23
#define STRING_BASIC_CHAR      66
#define DECIMAL_INTEGER_LITERAL      68
#define OP_SHIFT_RIGHT_ASSIGN      12
#define T__130      130
#define T__131      131
#define T__132      132
#define T__79      79
#define T__133      133
#define T__78      78
#define T__134      134
#define OP_BITWISE_NOT      37
#define T__77      77
#define OP_CALL_FUNC      44
#define OP_ASSIGN      5
#define OP_CONDITIONAL      16
#define OP_BITWISE_OR      19
#define T__118      118
#define T__119      119
#define T__116      116
#define T__117      117
#define T__114      114
#define T__115      115
#define T__124      124
#define OP_MUL      33
#define T__123      123
#define OP_PRE_INCR      40
#define T__122      122
#define T__121      121
#define T__120      120
#define OP_BITWISE_XOR      20
#define LOCAL_DECLARATION_STAT      55
#define OP_LESS      24
#define ML_COMMENT      73
#define OP_SUB      31
#define OP_LESS_EQUAL      25
#define OP_INDEX      43
#define T__107      107
#define OP_SHIFT_RIGHT      29
#define T__108      108
#define OP_ELEMENT      42
#define T__109      109
#define IDENTIFIER      64
#define T__103      103
#define T__104      104
#define BREAK_STAT      57
#define T__105      105
#define T__106      106
#define T__111      111
#define T__110      110
#define T__113      113
#define ARGUMENT_LIST      47
#define T__112      112
#define OP_DIV_ASSIGN      9
#define FLOAT_EXPONENT      71
#define DICT_ELEM_VALUE      63
#define OP_SHIFT_LEFT      28
#define T__102      102
#define T__101      101
#define T__100      100
#define INCLUDE_FILENAME      75
#define OP_POST_DECR      36
#define RETURN_STAT      60
#define DO_WHILE_STAT      54
#define CONTINUE_STAT      58
#define EMPTY_EXPR      56
#define FLOAT_LITERAL      70
#define OP_AND_ASSIGN      13
#define OP_ADD_ASSIGN      6
#define OP_NEGATE      39
#define OP_SHIFT_LEFT_ASSIGN      11
#define DICT_VALUE      62
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for gruntEval
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
