// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "SignalsFwd.h"
#include <set>
#include <utility>

namespace QuickSig {

namespace detail {
class TrackableBase
{
public:
	struct SignalDescriptor
	{
		SignalNotification DisconnectAll;

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
		TrackableCopyNotification TrackableCopied;
#endif

		bool operator < (const SignalDescriptor& rhs) const
		{
			return DisconnectAll < rhs.DisconnectAll;
		}

		size_t ConnectedSlots;

		SignalDescriptor()
			: ConnectedSlots(0)
		{}
		SignalDescriptor(const SignalNotification& signalId)
			: DisconnectAll(signalId)
			, ConnectedSlots(0)
		{}
	};

	typedef std::set<SignalDescriptor> ConnectedSignals_t;
	typedef ConnectedSignals_t::iterator SignalIterator;
	ConnectedSignals_t m_Connections;
};

class TrackableAccess;
}

template <class SyncPolicy=NoSync>
class Trackable : private SyncPolicy::Sync, private detail::TrackableBase
#if !defined(QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY) && !defined(QUICKSIG_NO_BOOST)
	, private boost::noncopyable
#endif
{
public:
	void DisconnectAllSlots()
	{
		Lock lk(m_Mutex);
		ConnectedSignals_t connections;
		connections.swap(m_Connections);
		for (ConnectedSignals_t::const_iterator it=connections.begin(); it!=connections.end(); ++it)
		{
			it->DisconnectAll(static_cast<TrackableBase*>(this));
		}
	}

protected:
	~Trackable()
	{
		DisconnectAllSlots();
	}

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
	Trackable() {}
	Trackable(const Trackable& rhs)
	{
		*this = rhs;
	}

	Trackable& operator = (const Trackable& rhs)
	{
		if (this != &rhs)
		{
			Lock lk1(m_Mutex);
			Lock lk2(rhs.m_Mutex);
			for (ConnectedSignals_t::const_iterator it=rhs.m_Connections.begin(); it!=rhs.m_Connections.end(); ++it)
			{
				it->TrackableCopied(this, &rhs);
			}
		}
		return *this;
	}
#endif

private:
	friend class detail::TrackableAccess;

	void SignalDestroyed(const detail::SignalNotification& signalId)
	{
		Lock lk(m_Mutex);
		ConnectedSignals_t::iterator it = m_Connections.find(signalId);
		if (it != m_Connections.end())
		{
			m_Connections.erase(it);
		}
	}

	void SlotDestroyed(const detail::SignalNotification& signalId)
	{
		Lock lk(m_Mutex);
		ConnectedSignals_t::iterator it = m_Connections.find(signalId);
		if (it != m_Connections.end())
		{
			assert(it->ConnectedSlots > 0);
			if (--it->ConnectedSlots == 0)
				m_Connections.erase(it);
		}
	}
};

}
