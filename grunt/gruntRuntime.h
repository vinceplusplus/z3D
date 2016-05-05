
#pragma once

#include "../z3D/Core/Header.h"

using namespace z3D::Core;

class gruntMemCellPool;

class gruntMemCell;

class gruntRuntime;

class gruntMemCell
{
public:
	typedef enum _TYPE
	{
		TYPE_INTEGER = 0,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_DICTIONARY,
		TYPE_FUNCTION,

		// internal use
		TYPE_FLOWCTRL,
		TYPE_FUNCTION_BINDING,
	} TYPE;

	typedef enum _SCOPE
	{
		SCOPE_BLOCK = 0,
		SCOPE_SWITCH,
		SCOPE_ITERATION,
		SCOPE_FUNCTION,
	} SCOPE;

	typedef enum _FLOWCTRL
	{
		FLOWCTRL_BREAK = 0,
		FLOWCTRL_CONTINUE,
	} FLOWCTRL;

	Z_DEFINE_FIXED_BLOCK_ALLOCATOR(fba, 52, 4);

	typedef map<wcs, gruntMemCell*, less<wcs>, fba<pair<const wcs, gruntMemCell*>>> DICT;
private:
	gruntMemCellPool*				_pool;
	uint64_t						_index;							// index in the pool
	wcs								_name;							// either the key in a dictionary or the element name in a structure
	gruntMemCell*					_parent;
private:
	size_t							_type;
	union
	{
		int64_t							_value_integer;				// scope value for scope type, node index for function type
		float							_value_float;
	};
	wcs								_value_string;
	typed_aligned_storage<DICT, sizeof(DICT), Z_GET_ALIGNMENT<DICT>::value, false>
									_value_dictionary;
	functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>
									_func_bind_func;
	void*							_func_bind_opaque;
private:
	gruntMemCell(gruntMemCellPool* pool, uint64_t index, const wcs& name);
	~gruntMemCell();
private:
	void							_setType(TYPE type);
public:
	gruntMemCellPool*				getPool() const;
	uint64_t						getIndex() const;
	const wcs&						getName() const;
	gruntMemCell*					getParent() const;
public:
	TYPE							getType() const;
	bool							isReturnValue() const;
public:
	int64_t&						asInteger();
	float&							asFloat();
	wcs&							asString();
	gruntMemCell&					asDictionary();
	int32_t&						asFunction();
	FLOWCTRL&						asFlowCtrl();
	void							setFunctionBinding(const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>& func, void* opaque);
public:
	void							flagAsReturnValue(bool is_return_value);
private:
	void							destroyDictionary();
public:
	void							clearDictionary();
	void							addElement(gruntMemCell* cell);
	gruntMemCell*					getElement(const wcs& name) const;
	gruntMemCell*					removeElement(const wcs& name);
	const DICT&						getDictionary() const;
public:
	const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>&
									getFunctionBindingFunc() const;
	void*							getFunctionBindingOpaque() const;
public:
	int64_t							toInteger() const;
	float							toFloat() const;
	wcs								toString() const;
	int32_t							toFunction() const;
	FLOWCTRL						toFlowCtrl() const;
	bool							toBool() const;
public:
	bool							isTemp() const;
public:
	bool							deleteThisIfTemp();
public:								// used internally
	void							setScope(SCOPE scope);
	SCOPE							getScope() const;
public:
	friend class gruntMemCellPool;
	template<class T> friend class Z_ALIGNMENT_QUERY;
};

class gruntMemCellPool
{
public:
	Z_DEFINE_FIXED_BLOCK_ALLOCATOR(fba, 40, 8);
	typedef map<uint64_t, gruntMemCell*, less<uint64_t>, fba<pair<const uint64_t, gruntMemCell*>>> POOL;
private:
	gruntRuntime*					_runtime;
	POOL							_pool;
	uint64_t						_next_key;
public:
	gruntMemCellPool(gruntRuntime* runtime);
	~gruntMemCellPool();
public:
	gruntRuntime*					getRuntime() const;
public:
	gruntMemCell*					addCell(const wcs& name);
	gruntMemCell*					getCell(uint64_t key) const;
	void							deleteCell(uint64_t key);
	void							clear();
};

class gruntFuncDef
{
public:
	int32_t							name_index;
	int32_t							param_start;
	int32_t							param_end;
	int32_t							stat_index;
};

class gruntRuntime
{
private:
	mem_pool_keeper<gruntMemCell, true>
									_cell_mp;
	nt_mem_pool_keeper<gruntMemCell::fba<char>::block_size, gruntMemCell::fba<char>::block_alignment, true>
									_element_mp;
	nt_mem_pool_keeper<gruntMemCellPool::fba<char>::block_size, gruntMemCellPool::fba<char>::block_alignment, true>
									_mem_pool_mp;
private:
	gruntMemCellPool				_mem_pool;
	gruntMemCell*					_globals;

	gruntMemCell*					_current_scope;
	gruntMemCell*					_current_scope_temps;
public:
	gruntRuntime();
	~gruntRuntime();
public:
	void							reset();
public:
	void							parse(const wcs& s);
public:
	gruntMemCell*					getGlobals() const;
public:
	void							enterScope(gruntMemCell::SCOPE scope);
	void							leaveScope();
public:
	gruntMemCell*					addCell(const wcs& name);
public:
	gruntMemCell*					addLocalCell(const wcs& name);
	gruntMemCell*					addTempCell();
public:
	gruntMemCell*					getIdentifierCell(const wcs& name);
public:
	void							bindFunc(const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>& func, void* opaque, const wcs& name, bool current_scope = false);
public:
	bool							canBreak() const;
	bool							canContinue() const;
	bool							canReturn() const;
public:
	void							clearTemp();
public:
	mem_pool<gruntMemCell>*			getCellMemPool() const;
	nt_mem_pool<gruntMemCell::fba<char>::block_size, gruntMemCell::fba<char>::block_alignment>*
									getElementMemPool() const;
	nt_mem_pool<gruntMemCellPool::fba<char>::block_size, gruntMemCellPool::fba<char>::block_alignment>*
									getMemCellPoolMemPool() const;
public:
	gruntMemCell*					getThisFunc() const;
public:
	gruntMemCell*					op_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_add_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_sub_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_mul_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_div_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_mod_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_shift_left_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_shift_right_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_and_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_or_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_xor_assign(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_logical_or(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_logical_and(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_bitwise_or(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_bitwise_xor(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_bitwise_and(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_equal(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_not_equal(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_less(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_less_equal(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_greater(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_greater_equal(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_shift_left(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_shift_right(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_add(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_sub(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_mod(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_mul(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_div(gruntMemCell* e0, gruntMemCell* e1);
	gruntMemCell*					op_bitwise_not(gruntMemCell* e0);
	gruntMemCell*					op_logical_not(gruntMemCell* e0);
	gruntMemCell*					op_negate(gruntMemCell* e0);
	gruntMemCell*					op_pre_incr(gruntMemCell* e0);
	gruntMemCell*					op_pre_decr(gruntMemCell* e0);
	gruntMemCell*					op_post_incr(gruntMemCell* e0);
	gruntMemCell*					op_post_decr(gruntMemCell* e0);
	gruntMemCell*					op_element(gruntMemCell* e0, const wcs& e1);
	gruntMemCell*					op_index(gruntMemCell* e0, gruntMemCell* e1);
};


typedef gruntMemCell gruntRValue;
