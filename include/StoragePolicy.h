// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "SignalsFwd.h"
#include "Trackable.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace QuickSig {

namespace detail {
	class TrackableAccess;
}
using detail::TrackableAccess;

namespace detail {

class TrackableAccess
{
public:
	template <class Trackable>
	static void AddRefConnectedSignal(Trackable* tracked, const SignalNotification& signalId
#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
		, const TrackableCopyNotification& copyNotification
#endif
		)
	{
		assert(tracked);
		typename Trackable::Lock lk(tracked->m_Mutex);
		AddRefConnectedSignalImpl(tracked, signalId
#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
			, copyNotification
#endif
		);
	}

	static void AddRefConnectedSignalImpl(TrackableBase* tracked, const SignalNotification& signalId
#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
		, const TrackableCopyNotification& copyNotification
#endif
		)
	{
		std::pair<TrackableBase::SignalIterator, bool> insertion = tracked->m_Connections.insert(signalId);
		insertion.first->ConnectedSlots++;

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
		if (insertion.second)
		{
			insertion.first->TrackableCopied = copyNotification;
		}
#endif
	}

	template <class Trackable>
	static void MakeCallbacks(Trackable* tracked, TrackableNotification& sigDtorCallback, TrackableNotification& connectionReleasedCallback)
	{
		assert(tracked);
		// no lock
		Bind<Trackable>(sigDtorCallback, tracked, &Trackable::SignalDestroyed);
		Bind<Trackable>(connectionReleasedCallback, tracked, &Trackable::SlotDestroyed);
	}

	template <class Trackable>
	static TrackableBase* ConvertToBase(Trackable* tracked)
	{
		// if your program fails to compile here, then the class that
		// has slots doesn't inherit from Trackable<>, or the used
		// inheritance isn't public and TrackableAccess isn't made a
		// friend to it.
		return static_cast<TrackableBase*>(tracked);
	}

private:
	template <class Trackable, class Delegate, typename A>
	static void Bind(Delegate& delg, Trackable* tracked, void (Trackable::*memfun)(A) )
	{ // needed so that the compiler performs the cast to Trackable<> before the bind takes place.
		// Otherwise, FastDelegate will attepmt the conversion, which may not work if Trackable<>
		// isn't inherited publicly
		delg.bind(tracked, memfun);
	}
};

template <class Delegate_t>
class SlotData
{
public:
	TrackableBase* Tracked;
	Delegate_t SlotDelegate;

	SlotData()
	{
		TrackableInst = NULL;
		Tracked = NULL;
	}

	bool Equivalent(const SlotData& rhs) const
	{
		return (Tracked == rhs.Tracked)
			&& (SlotDelegate == rhs.SlotDelegate || SlotDelegate.empty());
	}

	template <typename ComparableFunction>
	bool Equivalent(ComparableFunction f) const
	{
		// FastDelegate?::operator == isn't const, so we have to do this (otherwise unnecessary) cast
		return const_cast<SlotData*>(this)->SlotDelegate == f;
	}

	void SignalDestroyed(const SignalNotification& signalId) const
	{
		Call(SignalDestroyedFunc, signalId);
	}

	void SlotDestroyed(const SignalNotification& signalId) const
	{
		Call(SlotDestroyedFunc, signalId);
	}

	void SetDestructionDelegates(TrackableNotification& signalDestroyed, TrackableNotification& slotDestroyed)
	{
		assert(signalDestroyed);
		assert(slotDestroyed);

		MementoAccess acc;
		acc.SetMementoFrom(signalDestroyed.GetMemento());
		TrackableInst = acc.GetThis();
		SignalDestroyedFunc = acc.GetFunction();

		acc.SetMementoFrom(slotDestroyed.GetMemento());
		assert(TrackableInst == acc.GetThis());
		SlotDestroyedFunc = acc.GetFunction();
	}

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
	void ShiftDelegatesThis(ptrdiff_t amt) 
	{
		assert(amt != 0);
#define FASTSIGNALS_SHIFT(ptr, amt) reinterpret_cast<GenericClass*>(reinterpret_cast<char*>(ptr) + (amt))
		TrackableInst = FASTSIGNALS_SHIFT(TrackableInst, amt);

		MementoAccess slotShifter;
		slotShifter.SetMementoFrom(SlotDelegate.GetMemento());
		GenericClass* slotThis = slotShifter.GetThis();
		slotShifter.SetThis(FASTSIGNALS_SHIFT(slotThis, amt));
		SlotDelegate.SetMemento(slotShifter);
#undef FASTSIGNALS_SHIFT
	}
#endif

protected:
	typedef fastdelegate::detail::GenericClass GenericClass;

	class MementoAccess : public fastdelegate::DelegateMemento
	{
	public:
		typedef DelegateMemento::GenericMemFuncType GenericMemFunc;

		using DelegateMemento::SetMementoFrom;

		GenericClass* GetThis()
		{
			return m_pthis;
		}

		void SetThis(GenericClass* pthis)
		{
			assert(pthis);
			m_pthis = pthis;
		}

		GenericMemFuncType GetFunction()
		{
			return m_pFunction;
		}

		void SetFunction(GenericMemFuncType fun)
		{
			m_pFunction = fun;
		}

		MementoAccess(GenericClass* pthis, GenericMemFuncType func)
		{
			assert(pthis);
			assert(func);

			m_pthis = pthis;
			m_pFunction = func;
			m_pStaticFunction = 0;
		}

		MementoAccess(Delegate_t delg)
		{
			static_cast<fastdelegate::DelegateMemento&>(*this) = delg.GetMemento();
		}

		MementoAccess() {}
	};
	typedef typename MementoAccess::GenericMemFunc GenericMemFunc;

	GenericClass* TrackableInst;
	GenericMemFunc SignalDestroyedFunc;
	GenericMemFunc SlotDestroyedFunc;

	void Call(GenericMemFunc f, const SignalNotification& signalId) const
	{
		if (!TrackableInst)
			return;

		assert(f);
		assert(signalId);

		TrackableNotification delg;
		delg.SetMemento(MementoAccess(TrackableInst, f));
		delg(signalId);
	}
};

template <class Delegate_t>
class Slot : public SlotData<Delegate_t>
{
public:
	Priority_t Priority;

	Slot(const Delegate_t& slotDelegate = Delegate_t(), Priority_t priority = 0)
	{
		assert(slotDelegate);
		SlotDelegate = slotDelegate;
		Priority = priority;
	}

	bool operator < (const Slot& rhs) const
	{
		return std::make_pair(Priority, std::make_pair(Tracked, SlotDelegate))
			> std::make_pair(rhs.Priority, std::make_pair(rhs.Tracked, rhs.SlotDelegate));
	}

#ifdef QUICKSIG_USE_GOOGLE_HASH
	static Slot MakeInvalidSlot(int uniqueId)
	{
		MementoAccess memento(0, reinterpret_cast<GenericMemFunc>(uniqueId));
		return Slot(Delegate_t(memento));
	}

	struct Hash
	{
		size_t operator () (const Slot& slot) const
		{
			union
			{
				GenericMemFunc func;
				size_t value;
			} caster;
			MementoAccess memento(slot.SlotDelegate);
			caster.func = memento.GetFunction();
			return reinterpret_cast<size_t>(memento.GetThis())
				^ caster.value;
		}
	};

	bool operator == (const Slot& rhs) const
	{
		return Equivalent(static_cast<const SlotData<Delegate_t>&>(rhs));
	}
#endif
};

template <class Delegate_t, class ConcreteStoragePolicy, class SyncPolicy, class AllocatorPolicy>
class StoragePolicy
	: public SyncPolicy
	, public ConcreteStoragePolicy::template Storage<Slot<Delegate_t>,
		AllocatorPolicy::template Allocator<Slot<Delegate_t> >::Type>
{
public:
	typedef Slot<Delegate_t> Slot;
	typedef SlotData<Delegate_t> SlotData;
	typedef typename Storage::SlotContainer SlotContainer;
	typedef typename Storage::SlotIterator SlotIterator;
	typedef typename AllocatorPolicy::template Allocator<Slot>::Type Allocator;

	struct TrackedRemovalMatcher
	{
		const SlotData& m_Slot;
		const SignalNotification& m_SignalId;

		TrackedRemovalMatcher(const SlotData& slot, const SignalNotification& signalId)
			: m_Slot(slot)
			, m_SignalId(signalId)
		{
			assert(signalId);
		}

		bool operator () (const SlotData& slot) const
		{
			// C++ Standard 25.2.7.5:
			// Complexity: Exactly (last - first) applications of the corresponding predicate.
			// i.e. this predicate will be applied exactly once for each element, so we can
			// safely do some clean-up before removal.
			bool willBeRemoved = m_Slot.Equivalent(slot);
			if (willBeRemoved)
			{
				slot.SlotDestroyed(m_SignalId);
			}
			return willBeRemoved;
		}
	};
	template <class StaticFunc>
	struct StaticFuncRemovalMatcher
	{
		StaticFuncRemovalMatcher(StaticFunc func)
		{
			m_Function = func;
		}

		bool operator () (const SlotData& slot) const
		{
			return slot.Equivalent(m_Function);
		}

		StaticFunc m_Function;
	};

	~StoragePolicy()
	{
		DisconnectAll();
	}

	template <class Trackable>
	bool Connect(Trackable* tracked, Delegate_t& delg, Priority_t priority)
	{
		assert(delg);
		Lock lk(m_Mutex);

		if (SlotExists(delg))
			return false;

		Slot slot(delg, priority);
		slot.Tracked = TrackableAccess::ConvertToBase(tracked);
		if (tracked)
		{
			TrackableAccess::AddRefConnectedSignal(tracked, MakeDisconnectTrackable()
#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
				, MakeTrackableCopy()
#endif
				);

			TrackableNotification signalDestroyed, slotDestroyed;
			TrackableAccess::MakeCallbacks(tracked, signalDestroyed, slotDestroyed);
			slot.SetDestructionDelegates(signalDestroyed, slotDestroyed);
		}
		return Add(slot);
	}

	template <class Trackable>
	void Disconnect(Trackable* tracked, Delegate_t& delg)
	{
		assert(delg);
		Lock lk(m_Mutex);

		SlotData slot;
		slot.Tracked = TrackableAccess::ConvertToBase(tracked);
		slot.SlotDelegate = delg;

		RemoveIf(TrackedRemovalMatcher(slot, MakeDisconnectTrackable()));
	}

	template <class StaticFunction>
	void Disconnect(StaticFunction func)
	{
		assert(func);
		Lock lk(m_Mutex);
		RemoveIf(StaticFuncRemovalMatcher<StaticFunction>(func));
	}

	void DisconnectAll()
	{
		Lock lk(m_Mutex);
		for (SlotIterator it = SlotsBegin(); it != SlotsEnd(); ++it)
		{
			it->SignalDestroyed(MakeDisconnectTrackable());
		}
		DeleteSlots();
	}

	template <class ComparableFunction>
	bool SlotExists(const ComparableFunction& f)
	{
		Lock lk(m_Mutex);
		for (SlotIterator it=SlotsBegin(); it!=SlotsEnd(); ++it)
			if (it->Equivalent(f))
				return true;
		return false;
	}

private:
	SignalNotification MakeDisconnectTrackable()
	{
		return SignalNotification(this, &StoragePolicy::Disconnect);
	}

	void Disconnect(TrackableBase* tracked)
	{
		assert(tracked);
		Lock lk(m_Mutex);

		SlotData slot;
		slot.Tracked = tracked;

		RemoveIf(TrackedRemovalMatcher(slot, MakeDisconnectTrackable()));
	}

#ifdef QUICKSIG_ENABLE_COPY_CONSTRUCTIBILITY
	TrackableCopyNotification MakeTrackableCopy()
	{
		return TrackableCopyNotification(this, &StoragePolicy::CopyTrackable);
	}

	void CopyTrackable(TrackableBase* newTracked, const TrackableBase* oldTracked)
	{
		assert(newTracked);
		assert(oldTracked);

		std::vector<Slot, Allocator> newSlots;
		for (SlotIterator it = SlotsBegin(); it != SlotsEnd(); ++it)
		{
			const Slot& slot = *it;
			if (slot.Tracked != oldTracked)
				continue;

			Slot newSlot = slot;
			newSlot.Tracked = newTracked;
			newSlot.ShiftDelegatesThis(reinterpret_cast<const char*>(newTracked) - reinterpret_cast<const char*>(oldTracked));
			TrackableAccess::AddRefConnectedSignalImpl(newTracked, MakeDisconnectTrackable(), MakeTrackableCopy());
			newSlots.push_back(newSlot);
		}
		for (size_t i=0; i<newSlots.size(); ++i)
		{
			bool success = Add(newSlots[i]);
			assert(success);
		}
	}
#endif
};

}

class ArrayStorage : public virtual DefaultPolicies
{
public:
	struct Impl
	{
		template <class Slot, class Allocator>
		class Storage
		{
		public:
			typedef std::vector<Slot, Allocator> SlotContainer;
			typedef typename SlotContainer::const_iterator SlotIterator;

			bool Add(const Slot& c)
			{
				assert(c.Priority == 0);
				m_Slots.push_back(c);
				return true;
			}

			template <class Pred>
			void RemoveIf(Pred p)
			{
				m_Slots.erase(
					std::remove_if(m_Slots.begin(), m_Slots.end(), p),
					SlotsEnd());
			}

			SlotIterator SlotsBegin() const
			{
				return m_Slots.begin();
			}
			SlotIterator SlotsEnd() const
			{
				return m_Slots.end();
			}

			void DeleteSlots()
			{
				m_Slots.clear();
			}
		protected:
			SlotContainer m_Slots;
		};
	};
	typedef Impl StoragePolicy;
};

class SortedArrayStorage : public virtual DefaultPolicies
{
public:
	struct Impl
	{
		template <class Slot, class Allocator>
		class Storage : public ArrayStorage::Impl::Storage<Slot, Allocator>
		{
		public:
			bool Add(const Slot& c)
			{
				SlotIterator pos = std::lower_bound(SlotsBegin(), SlotsEnd(), c);
				m_Slots.insert(pos, c);
			}
		};
	};
	typedef Impl StoragePolicy;
};

}
