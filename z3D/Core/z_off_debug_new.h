
#pragma push_macro("malloc")
#pragma push_macro("free")
#pragma push_macro("calloc")
#pragma push_macro("realloc")
#pragma push_macro("_recalloc")
#pragma push_macro("_expand")
#pragma push_macro("_msize")

#pragma push_macro("new")

#undef malloc
#undef free
#undef calloc
#undef realloc
#undef _recalloc
#undef _expand
#undef _msize

#undef new
