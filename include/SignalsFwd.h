// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "Config.h"
#include "FastDelegate.h"

#if defined(QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY) && defined(FASTDELEGATE_USESTATICFUNCTIONHACK)
#error The fastdelegate's static function hack is incompatible with signal copy constructibility
#endif

#include <cassert>
#include <memory>

#ifndef QUICKSIG_NO_BOOST
#	include <boost/noncopyable.hpp>
#endif

namespace QuickSig {

class NoSync;
class ArrayStorage;
class NoResult;
class StdAllocator;

struct DefaultPolicies
{
	typedef NoSync SyncPolicy;
	typedef ArrayStorage StoragePolicy;
	typedef NoResult ResultPolicy;
	typedef StdAllocator AllocatorPolicy;
};

typedef int Priority_t;

namespace detail {

class TrackableBase;

typedef fastdelegate::FastDelegate1<TrackableBase*> SignalNotification;
typedef fastdelegate::FastDelegate1<const SignalNotification&> TrackableNotification;

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
typedef fastdelegate::FastDelegate2<TrackableBase*, const TrackableBase*> TrackableCopyNotification; // (new, old)
#endif

}

class NoSync : public virtual DefaultPolicies
{
public:
	struct Sync
	{
		struct Mutex {} m_Mutex;
		struct Lock
		{
			Lock(Mutex) {}
		};
	};
};

class StdAllocator : public virtual DefaultPolicies
{
public:
	struct Impl
	{
		template <class T>
		struct Allocator
		{
			typedef std::allocator<T> Type;
		};

	};
	typedef Impl AllocatorPolicy;
};

}
