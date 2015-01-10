#pragma once

#include "Config.h"

#ifdef QUICKSIG_USE_GOOGLE_HASH
#	include <google/dense_hash_set>
#	define QUICKSIG_SET(TYPE) google::dense_hash_set<TYPE>
#	define QUICKSIG_SETA(TYPE, ALLOC) google::dense_hash_set<TYPE, google::SPARSEHASH_HASH<Value>, STL_NAMESPACE::equal_to<Value>, ALLOC>
#else
#	include <set>
#	define QUICKSIG_SET(TYPE) std::set<TYPE>
#	define QUICKSIG_SETA(TYPE, ALLOC) std::set<TYPE, std::less<TYPE>, ALLOC>
#endif
