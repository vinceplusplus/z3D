
#include "gruntRuntime.h"

#include "../z3D/Core/nt_mem_pool_inl.h"
#include "../z3D/Core/mem_pool_inl.h"
#include "../z3D/Core/allocators_inl.h"
#include "../z3D/Core/aligned_storage_inl.h"
#include "../z3D/Core/functor_inl.h"

#include "gruntLexer.h"
#include "gruntParser.h"
#include "gruntEval.h"

gruntMemCell::gruntMemCell(gruntMemCellPool* pool, uint64_t index, const wcs& name)
	: _pool(pool), _index(index), _name(name), _parent(NULL)
{
	_type = (size_t)TYPE_INTEGER;
	_value_integer = 0;
}

gruntMemCell::~gruntMemCell()
{
	// we don't need to check if it is still attaching to a parent, gruntMemCellPool::deleteCell() will do this for us
	asInteger();
}
void							gruntMemCell::_setType(TYPE type)
{
	_type = (_type & 0x80000000) | (type & 0x7fffffff);
}
gruntMemCellPool*				gruntMemCell::getPool() const
{
	return _pool;
}
uint64_t						gruntMemCell::getIndex() const
{
	return _index;
}
const wcs&						gruntMemCell::getName() const
{
	return _name;
}
gruntMemCell*					gruntMemCell::getParent() const
{
	return _parent;
}
gruntMemCell::TYPE				gruntMemCell::getType() const
{
	return (TYPE)(_type & 0x7fffffff);
}
bool							gruntMemCell::isReturnValue() const
{
	return !!(_type & 0x80000000);
}
int64_t&						gruntMemCell::asInteger()
{
	switch(getType())
	{
	case TYPE_INTEGER:
		return _value_integer;

	case TYPE_FLOAT:
		_setType(TYPE_INTEGER);
		return _value_integer = (int64_t)_value_float;

	case TYPE_STRING:
		_setType(TYPE_INTEGER);
		_value_integer = _wcstoi64(_value_string.c_str(), NULL, 10);
		_value_string.clear();
		return _value_integer;

	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	}
	_setType(TYPE_INTEGER);
	return _value_integer = 0;
}
float&							gruntMemCell::asFloat()
{
	switch(getType())
	{
	case TYPE_FLOAT:
		return _value_float;

	case TYPE_INTEGER:
		_setType(TYPE_FLOAT);
		return _value_float = (float)_value_integer;

	case TYPE_STRING:
		_setType(TYPE_FLOAT);
		_value_float = (float)wcstod(_value_string.c_str(), NULL);
		_value_string.clear();
		return _value_float;

	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	}
	_setType(TYPE_FLOAT);
	return _value_float = 0.0f;
}
wcs&							gruntMemCell::asString()
{
	switch(getType())
	{
	case TYPE_STRING:
		return _value_string;

	case TYPE_INTEGER:
		_setType(TYPE_STRING);
		return _value_string = FORMATW(L"%I64d", _value_integer);

	case TYPE_FLOAT:
		_setType(TYPE_STRING);
		return _value_string = FORMATW(L"%f", _value_float);

	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	case TYPE_FLOWCTRL:
		break;
	}
	_setType(TYPE_STRING);
	return _value_string = L"";
}

#include "../z3D/Core/z_off_debug_new.h"

gruntMemCell&					gruntMemCell::asDictionary()
{
	switch(getType())
	{
	case TYPE_DICTIONARY:
		return *this;

	case TYPE_STRING:
		_value_string.clear();
		break;
	}
	_setType(TYPE_DICTIONARY);
	new (_value_dictionary.allocate()) DICT(DICT::key_compare(), gruntMemCell::fba<pair<const DICT::key_type, DICT::mapped_type>>(_pool->getRuntime()->getElementMemPool()));
	return *this;
}

#include "../z3D/Core/z_on_debug_new.h"

int32_t&						gruntMemCell::asFunction()
{
	switch(getType())
	{
	case TYPE_FUNCTION:
		return *(int32_t*)&_value_integer;

	case TYPE_STRING:
		_value_string.clear();
		break;
	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	}
	_setType(TYPE_FUNCTION);
	_value_integer = -1I32;
	return *(int32_t*)&_value_integer;
}

gruntMemCell::FLOWCTRL&			gruntMemCell::asFlowCtrl()
{
	switch(getType())
	{
	case TYPE_FLOWCTRL:
		return *(FLOWCTRL*)&_value_integer;

	case TYPE_STRING:
		_value_string.clear();
		break;
	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	}
	_setType(TYPE_FLOWCTRL);
	_value_integer = 0;
	return *(FLOWCTRL*)&_value_integer;
}
void							gruntMemCell::setFunctionBinding(const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>& func, void* opaque)
{
	switch(getType())
	{
	case TYPE_STRING:
		_value_string.clear();
		break;
	case TYPE_DICTIONARY:
		destroyDictionary();
		break;
	}
	_setType(TYPE_FUNCTION_BINDING);
	_func_bind_func = func;
	_func_bind_opaque = opaque;
}

void							gruntMemCell::flagAsReturnValue(bool is_return_value)
{
	_type = (_type & 0x7fffffff) | (((size_t)is_return_value) << 31);
}

void							gruntMemCell::destroyDictionary()
{
	clearDictionary();
	_value_dictionary.destroy();
}

void							gruntMemCell::clearDictionary()
{
	Z_ASSERT( getType() == TYPE_DICTIONARY );

	for(DICT::iterator iter = _value_dictionary.get().begin(); iter != _value_dictionary.get().end(); ++iter)
	{
		iter->second->_parent = NULL;							// avoid triggering call to removeElement() to remove this cell from its parent
		_pool->deleteCell(iter->second->getIndex());
	}
	_value_dictionary.get().clear();
}
void							gruntMemCell::addElement(gruntMemCell* cell)
{
	asDictionary();
	DICT::iterator iter = _value_dictionary.get().find(cell->getName());
	if(iter == _value_dictionary.get().end())
		_value_dictionary.get()[cell->getName()] = cell;
	else
	{
		iter->second->_parent = NULL;
		_pool->deleteCell(iter->second->getIndex());
		iter->second = cell;
	}
	cell->_parent = this;
}
gruntMemCell*					gruntMemCell::getElement(const wcs& name) const
{
	DICT::const_iterator iter = _value_dictionary.get().find(name);
	if(iter == _value_dictionary.get().end())
		return NULL;
	return iter->second;
}
gruntMemCell*					gruntMemCell::removeElement(const wcs& name)
{
	DICT::iterator iter = _value_dictionary.get().find(name);
	if(iter == _value_dictionary.get().end())
		return NULL;
	gruntMemCell* c = iter->second;
	c->_parent = NULL;
	_value_dictionary.get().erase(iter);
	return c;
}
const gruntMemCell::DICT&		gruntMemCell::getDictionary() const
{
	return _value_dictionary.get();
}

const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>&
								gruntMemCell::getFunctionBindingFunc() const
{
	return _func_bind_func;
}
void*							gruntMemCell::getFunctionBindingOpaque() const
{
	return _func_bind_opaque;
}

int64_t							gruntMemCell::toInteger() const
{
	switch(getType())
	{
	case TYPE_INTEGER:
		return _value_integer;
	case TYPE_FLOAT:
		return (int64_t)_value_float;
	case TYPE_STRING:
		return _wcstoi64(_value_string.c_str(), NULL, 10);
	}
	return 0;
}
float							gruntMemCell::toFloat() const
{
	switch(getType())
	{
	case TYPE_INTEGER:
		return (float)_value_integer;
	case TYPE_FLOAT:
		return _value_float;
	case TYPE_STRING:
		return (float)wcstod(_value_string.c_str(), NULL);
	}
	return 0.0f;
}
wcs								gruntMemCell::toString() const
{
	switch(getType())
	{
	case TYPE_INTEGER:
		return FORMATW(L"%I64d", _value_integer);
		break;
	case TYPE_FLOAT:
		return FORMATW(L"%f", _value_float);
		break;
	case TYPE_STRING:
		return _value_string;
	}
	return L"";
}

int32_t							gruntMemCell::toFunction() const
{
	switch(getType())
	{
	case TYPE_FUNCTION:
		return *(int32_t*)&_value_integer;
	}
	return -1I32;
}

gruntMemCell::FLOWCTRL			gruntMemCell::toFlowCtrl() const
{
	switch(getType())
	{
	case TYPE_FLOWCTRL:
		return *(FLOWCTRL*)&_value_integer;
	}
	return FLOWCTRL_BREAK;
}

bool							gruntMemCell::toBool() const
{
	switch(getType())
	{
	case TYPE_INTEGER:
		return !!_value_integer;
	case TYPE_FLOAT:
		return !!_value_float;
	}
	return false;
}

bool							gruntMemCell::isTemp() const
{
	return !_parent;
}

bool							gruntMemCell::deleteThisIfTemp()
{
	if(!isTemp())
		return false;
	getPool()->deleteCell(getIndex());
	return true;
}
void							gruntMemCell::setScope(SCOPE scope)
{
	_value_integer = (uint64_t)scope;
}
gruntMemCell::SCOPE				gruntMemCell::getScope() const
{
	return (SCOPE)_value_integer;
}

gruntMemCellPool::gruntMemCellPool(gruntRuntime* runtime)
	:	_runtime(runtime),
		_pool(POOL::key_compare(), fba<pair<const POOL::key_type, POOL::mapped_type>>(runtime->getMemCellPoolMemPool()))
{
	_next_key = 0;
}
gruntMemCellPool::~gruntMemCellPool()
{
	clear();
}
gruntRuntime*					gruntMemCellPool::getRuntime() const
{
	return _runtime;
}
#include "../z3D/Core/z_off_debug_new.h"
gruntMemCell*					gruntMemCellPool::addCell(const wcs& name)
{
	gruntMemCell* c;
	_pool[_next_key] = c = new (_runtime->getCellMemPool()->alloc()) gruntMemCell(this, _next_key, name);
	++_next_key;
	return c;
}
#include "../z3D/Core/z_on_debug_new.h"
gruntMemCell*					gruntMemCellPool::getCell(uint64_t key) const
{
	POOL::const_iterator iter = _pool.find(key);
	if(iter != _pool.end())
		return iter->second;
	return NULL;
}
void							gruntMemCellPool::deleteCell(uint64_t key)
{
	POOL::iterator iter = _pool.find(key);
	Z_ASSERT( iter != _pool.end() );
	if(iter != _pool.end())
	{
		gruntMemCell* c = iter->second;

		if(c->getParent())
			c->getParent()->removeElement(c->getName());

		Z_STATIC_ASSERT( (SAME_TYPE<POOL::mapped_type, gruntMemCell*>::value), DECLARATION_DEFINITION_DISCREPANCY );
		iter->second->~gruntMemCell();
		_runtime->getCellMemPool()->dealloc(iter->second);

		_pool.erase(iter);
	}
}
void							gruntMemCellPool::clear()
{
	while(_pool.size())
		deleteCell(_pool.begin()->second->getIndex());
}

gruntRuntime::gruntRuntime()
	:	_cell_mp(1024),
		_element_mp(1024),
		_mem_pool_mp(1024),
		_mem_pool(this)
{
	_globals = _mem_pool.addCell(L"$");
	_globals->asDictionary();

	_current_scope = _globals;
	_current_scope_temps = NULL;
}
gruntRuntime::~gruntRuntime()
{
}
void							gruntRuntime::reset()
{
	_mem_pool.clear();

	_globals = _mem_pool.addCell(L"$");
	_globals->asDictionary();

	_current_scope = _globals;
	_current_scope_temps = NULL;
}
void							gruntRuntime::parse(const wcs& s)
{
	ANTLR3_INPUT_STREAM* is = antlr3NewUCS2StringInPlaceStream((ANTLR3_UINT16*)s.c_str(), (ANTLR3_UINT32)s.length(), (ANTLR3_UINT16*)L"");
	gruntLexer* lexer = gruntLexerNew(is);
	ANTLR3_COMMON_TOKEN_STREAM* ts = antlr3CommonTokenStreamSourceNew(0, TOKENSOURCE(lexer));
	gruntParser* parser = gruntParserNew(ts);

	gruntParser_prog_return ast = parser->prog(parser);

	if(parser->pParser->rec->state->errorCount > 0)
	{
		printf("SYNTAX ERROR\n");
	}
	else if(ts->p < (ptrdiff_t)ts->tokens->count)
	{
		ANTLR3_COMMON_TOKEN* tok = (ANTLR3_COMMON_TOKEN*)ts->tokens->get(ts->tokens, ts->p);
		ANTLR3_STRING* s = tok->getText(tok);
		_putws(FORMATW(L"unexpected token '%s' @ %d:%d\n", s->chars, tok->getLine(tok), tok->getCharPositionInLine(tok)).c_str());
	}
	else
	{
		ANTLR3_COMMON_TREE_NODE_STREAM* tns = antlr3CommonTreeNodeStreamNewTree(ast.tree, ANTLR3_SIZE_HINT);

		gruntEval* eval = gruntEvalNew(tns);

		eval->pTreeParser->rec->state->userp = this;

		eval->prog(eval);

		eval->free(eval);

		tns->free(tns);
	}

	parser->free(parser);
	ts->free(ts);
	lexer->free(lexer);
	is->free(is);
}
gruntMemCell*					gruntRuntime::getGlobals() const
{
	return _globals;
}
void							gruntRuntime::enterScope(gruntMemCell::SCOPE scope)
{
	gruntMemCell* c = _mem_pool.addCell(L"$");
	c->asDictionary();
	c->setScope(scope);
	_current_scope->addElement(c);
	_current_scope = c;
	_current_scope_temps = NULL;
}
void							gruntRuntime::leaveScope()
{
	Z_ASSERT( _current_scope->getParent() );
	if(!_current_scope->getParent())
		return;
	gruntMemCell* c = _current_scope;
	_current_scope = c->getParent();
	_current_scope_temps = NULL;
	_mem_pool.deleteCell(c->getIndex());
}
gruntMemCell*					gruntRuntime::addCell(const wcs& name)
{
	gruntMemCell* c = _mem_pool.addCell(name);
	return c;
}
gruntMemCell*					gruntRuntime::addLocalCell(const wcs& name)
{
	gruntMemCell* c = _mem_pool.addCell(name);
	_current_scope->addElement(c);
	return c;
}
gruntMemCell*					gruntRuntime::addTempCell()
{
	if(!_current_scope_temps)
	{
		_current_scope_temps = _current_scope->getElement(L"$t");
		if(!_current_scope_temps)
		{
			_current_scope_temps = addCell(L"$t");
			_current_scope->addElement(_current_scope_temps);
		}
	}
	gruntMemCell* c = _mem_pool.addCell(FORMATW(L"%d", _current_scope_temps->asDictionary().getDictionary().size()));
	_current_scope_temps->addElement(c);
	return c;
}
gruntMemCell*					gruntRuntime::getIdentifierCell(const wcs& name)
{
	gruntMemCell* c = NULL;
	gruntMemCell* s = _current_scope;

	while(s)
	{
		if((c = s->getElement(name)) != NULL)
			break;
		if(s->getScope() == gruntMemCell::SCOPE_FUNCTION)
			break;
		s = s->getParent();
	}
	if(!c && s && s->getParent())
		c = _globals->getElement(name);
	if(c)
		return c;
	c = addCell(name);
	_globals->addElement(c);
	return c;
}
void							gruntRuntime::bindFunc(const functor3<gruntMemCell*, void*, gruntRuntime*, gruntMemCell*>& func, void* opaque, const wcs& name, bool current_scope)
{
	gruntMemCell* f = addCell(name);
	f->setFunctionBinding(func, opaque);
	if(!current_scope)
		getGlobals()->addElement(f);
	else
		_current_scope->addElement(f);
}
bool							gruntRuntime::canBreak() const
{
	gruntMemCell* c = _current_scope;
	while(c)
	{
		if(c->getScope() == gruntMemCell::SCOPE_ITERATION || c->getScope() == gruntMemCell::SCOPE_SWITCH)
			return true;
		if(c->getScope() == gruntMemCell::SCOPE_FUNCTION)
			return false;
		c = c->getParent();
	}
	return false;
}
bool							gruntRuntime::canContinue() const
{
	gruntMemCell* c = _current_scope;
	while(c)
	{
		if(c->getScope() == gruntMemCell::SCOPE_ITERATION)
			return true;
		if(c->getScope() == gruntMemCell::SCOPE_FUNCTION)
			return false;
		c = c->getParent();
	}
	return false;
}
bool							gruntRuntime::canReturn() const
{
	gruntMemCell* c = _current_scope;
	while(c)
	{
		if(c->getScope() == gruntMemCell::SCOPE_FUNCTION)
			return true;
		c = c->getParent();
	}
	return false;
}
void							gruntRuntime::clearTemp()
{
	if(_current_scope_temps)
		_current_scope_temps->clearDictionary();
}
mem_pool<gruntMemCell>*			gruntRuntime::getCellMemPool() const
{
	return _cell_mp.get();
}
nt_mem_pool<gruntMemCell::fba<char>::block_size, gruntMemCell::fba<char>::block_alignment>*
								gruntRuntime::getElementMemPool() const
{
	return _element_mp.get();
}
nt_mem_pool<gruntMemCellPool::fba<char>::block_size, gruntMemCellPool::fba<char>::block_alignment>*
								gruntRuntime::getMemCellPoolMemPool() const
{
	return _mem_pool_mp.get();
}
gruntMemCell*					gruntRuntime::getThisFunc() const
{
	gruntMemCell* c = _current_scope;
	while(c)
	{
		if(c->getScope() == gruntMemCell::SCOPE_FUNCTION)
			return c->getElement(L"$fp");
		c = c->getParent();
	}
	return NULL;
}

gruntMemCell*					gruntRuntime::op_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e1->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		e0->asInteger() = e1->toInteger();
		break;
	case gruntMemCell::TYPE_FLOAT:
		e0->asFloat() = e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		e0->asString() = e1->toString();
		break;
	case gruntMemCell::TYPE_DICTIONARY:
		{
			e0->asDictionary().clearDictionary();

			const gruntMemCell::DICT& d = e1->getDictionary();
			for(gruntMemCell::DICT::const_iterator iter = d.begin(); iter != d.end(); ++iter)
			{
				gruntMemCell* c1 = iter->second;
				gruntMemCell* c0 = addCell(c1->getName());
				op_assign(c0, c1);
				e0->addElement(c0);
			}
		}
		break;
	case gruntMemCell::TYPE_FUNCTION:
		e0->asFunction() = e1->toFunction();
		break;
	case gruntMemCell::TYPE_FLOWCTRL:
		e0->asFlowCtrl() = e1->toFlowCtrl();
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_add_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			e0->asInteger() += e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			e0->asFloat() += e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asFloat() += e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_STRING:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asString() += e1->toString();
			break;
		}
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_sub_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			e0->asInteger() -= e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			e0->asFloat() -= e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asFloat() -= e1->toFloat();
			break;
		}
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_mul_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			e0->asInteger() *= e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			e0->asFloat() *= e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asFloat() *= e1->toFloat();
			break;
		}
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_div_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			e0->asInteger() /= e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			e0->asFloat() /= e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asFloat() /= e1->toFloat();
			break;
		}
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_mod_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			e0->asInteger() %= e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			e0->asFloat() = FMOD(e0->toFloat(), e1->toFloat());
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			e0->asFloat() = FMOD(e0->toFloat(), e1->toFloat());
			break;
		}
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_shift_left_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	e0->asInteger() <<= e1->toInteger();
	return e0;
}
gruntMemCell*					gruntRuntime::op_shift_right_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	e0->asInteger() >>= e1->toInteger();
	return e0;
}
gruntMemCell*					gruntRuntime::op_and_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	e0->asInteger() &= e1->toInteger();
	return e0;
}
gruntMemCell*					gruntRuntime::op_or_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	e0->asInteger() |= e1->toInteger();
	return e0;
}
gruntMemCell*					gruntRuntime::op_xor_assign(gruntMemCell* e0, gruntMemCell* e1)
{
	e0->asInteger() ^= e1->toInteger();
	return e0;
}
gruntMemCell*					gruntRuntime::op_logical_or(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = !!e0->toInteger() || !!e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_logical_and(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() && e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_bitwise_or(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() | e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_bitwise_xor(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() ^ e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_bitwise_and(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() & e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_equal(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() == e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() == e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() == e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() == e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_not_equal(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() != e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() != e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() != e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() != e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_less(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() < e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() < e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() < e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() < e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_less_equal(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() <= e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() <= e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() <= e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() <= e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_greater(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() > e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() > e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() > e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() > e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_greater_equal(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_FLOAT:
			c->asInteger() = e0->toFloat() >= e1->toFloat();
			break;
		default:
			c->asInteger() = e0->toInteger() >= e1->toInteger();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asInteger() = e0->toFloat() >= e1->toFloat();
		break;
	case gruntMemCell::TYPE_STRING:
		c->asInteger() = e0->toString() >= e1->toString();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_shift_left(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() << e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_shift_right(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = e0->toInteger() >> e1->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_add(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();

	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			c->asInteger() = e0->toInteger() + e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			c->asFloat() = e0->toFloat() + e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asFloat() = e0->toFloat() + e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_STRING:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asString() = e0->toString() + e1->toString();
			break;
		}
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_sub(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();

	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			c->asInteger() = e0->toInteger() - e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			c->asFloat() = e0->toFloat() - e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asFloat() = e0->toFloat() - e1->toFloat();
			break;
		}
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_mod(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();

	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			c->asInteger() = e0->toInteger() % e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			c->asFloat() = FMOD(e0->toFloat(), e1->toFloat());
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asFloat() = FMOD(e0->toFloat(), e1->toFloat());
			break;
		}
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_mul(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();

	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			c->asInteger() = e0->toInteger() * e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			c->asFloat() = e0->toFloat() * e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asFloat() = e0->toFloat() * e1->toFloat();
			break;
		}
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_div(gruntMemCell* e0, gruntMemCell* e1)
{
	gruntMemCell* c = addTempCell();

	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_STRING:
			c->asInteger() = e0->toInteger() / e1->toInteger();
			break;
		case gruntMemCell::TYPE_FLOAT:
			c->asFloat() = e0->toFloat() / e1->toFloat();
			break;
		}
		break;
	case gruntMemCell::TYPE_FLOAT:
		switch(e1->getType())
		{
		case gruntMemCell::TYPE_INTEGER:
		case gruntMemCell::TYPE_FLOAT:
		case gruntMemCell::TYPE_STRING:
			c->asFloat() = e0->toFloat() / e1->toFloat();
			break;
		}
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_bitwise_not(gruntMemCell* e0)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = ~e0->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_logical_not(gruntMemCell* e0)
{
	gruntMemCell* c = addTempCell();
	c->asInteger() = !e0->toInteger();
	return c;
}
gruntMemCell*					gruntRuntime::op_negate(gruntMemCell* e0)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		c->asInteger() = -e0->toInteger();
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asFloat() = -e0->toFloat();
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_pre_incr(gruntMemCell* e0)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		++e0->asInteger();
		break;
	case gruntMemCell::TYPE_FLOAT:
		++e0->asFloat();
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_pre_decr(gruntMemCell* e0)
{
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		--e0->asInteger();
		break;
	case gruntMemCell::TYPE_FLOAT:
		--e0->asFloat();
		break;
	}
	return e0;
}
gruntMemCell*					gruntRuntime::op_post_incr(gruntMemCell* e0)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		c->asInteger() = e0->asInteger()++;
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asFloat() = e0->asFloat()++;
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_post_decr(gruntMemCell* e0)
{
	gruntMemCell* c = addTempCell();
	switch(e0->getType())
	{
	case gruntMemCell::TYPE_INTEGER:
		c->asInteger() = e0->asInteger()--;
		break;
	case gruntMemCell::TYPE_FLOAT:
		c->asFloat() = e0->asFloat()--;
		break;
	}
	return c;
}
gruntMemCell*					gruntRuntime::op_element(gruntMemCell* e0, const wcs& e1)
{
	const wcs& name = e1;
	gruntMemCell* c = e0->asDictionary().getElement(name);
	if(c)
		return c;
	c = addCell(name);
	e0->addElement(c);
	return c;
}
gruntMemCell*					gruntRuntime::op_index(gruntMemCell* e0, gruntMemCell* e1)
{
	return op_element(e0, e1->toString());
}
