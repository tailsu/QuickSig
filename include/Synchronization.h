// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "Config.h"

#ifdef QUICKSIG_NO_BOOST
#error Synchronization policies require Boost.Mutex
#endif

#include <boost/thread/recursive_mutex.hpp>

namespace QuickSig {

struct SyncBase
{
	typedef boost::recursive_mutex Mutex;
	typedef Mutex::scoped_lock Lock;
};

struct InstanceLock : public virtual DefaultPolicies
{
	struct Sync : public SyncBase
	{
		mutable Mutex m_Mutex;
	};
	typedef Sync SyncPolicy;
};

namespace detail {
template <int LinkerWorkaround>
struct StaticMutexProvider : public SyncBase
{
	static Mutex m_Mutex;
};

template <int LinkerWorkaround>
SyncBase::Mutex StaticMutexProvider<LinkerWorkaround>::m_Mutex;
}

struct ProcesswideLock : public virtual DefaultPolicies
{
	struct Sync : public detail::StaticMutexProvider<0>
	{};
	typedef Sync SyncPolicy;
};

}
