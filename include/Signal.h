// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "StoragePolicy.h"
#include "ResultCombiner.h"

namespace QuickSig
{

namespace detail {
template <class Base, int I>
struct Discriminator : public Base {};
}

template <class P1=detail::DefaultPoliciesArg, class P2=detail::DefaultPoliciesArg, class P3=detail::DefaultPoliciesArg, class P4=detail::DefaultPoliciesArg>
struct Policies
	: public detail::Discriminator<P1, 1>
	, public detail::Discriminator<P2, 2>
	, public detail::Discriminator<P3, 3>
	, public detail::Discriminator<P4, 4>
{};

namespace detail {
struct DefaultPoliciesArg : public virtual DefaultPolicies {};
}

template <typename Sig, class Policies=detail::DefaultPoliciesArg>
class Signal;

#if 8 < QUICKSIG_MAX_ARITY
# error Selected arity greater than maximum available
#endif

//N=0
#if QUICKSIG_MAX_ARITY >= 0

template <class Policies=detail::DefaultPoliciesArg>
class Signal0
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate0<R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)())
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)())
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)())
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)())
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last)
				: m_Current(first)
				, m_Last(last)
				
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg();
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd());
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R>
class Signal<R ()>
	: public Signal0< >
{};

template <typename R, typename Policies>
class Signal<R (), Policies>
	: public Signal0<Policies>
{};

#endif

//N=1
#if QUICKSIG_MAX_ARITY >= 1

template <class Param1, class Policies=detail::DefaultPoliciesArg>
class Signal1
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate1<Param1, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1>
class Signal<R (Param1)>
	: public Signal1< Param1>
{};

template <typename R, class Param1, typename Policies>
class Signal<R (Param1), Policies>
	: public Signal1<Param1, Policies>
{};

#endif

//N=2
#if QUICKSIG_MAX_ARITY >= 2

template <class Param1, class Param2, class Policies=detail::DefaultPoliciesArg>
class Signal2
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate2<Param1, Param2, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2>
class Signal<R (Param1, Param2)>
	: public Signal2< Param1, Param2>
{};

template <typename R, class Param1, class Param2, typename Policies>
class Signal<R (Param1, Param2), Policies>
	: public Signal2<Param1, Param2, Policies>
{};

#endif

//N=3
#if QUICKSIG_MAX_ARITY >= 3

template <class Param1, class Param2, class Param3, class Policies=detail::DefaultPoliciesArg>
class Signal3
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate3<Param1, Param2, Param3, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3>
class Signal<R (Param1, Param2, Param3)>
	: public Signal3< Param1, Param2, Param3>
{};

template <typename R, class Param1, class Param2, class Param3, typename Policies>
class Signal<R (Param1, Param2, Param3), Policies>
	: public Signal3<Param1, Param2, Param3, Policies>
{};

#endif

//N=4
#if QUICKSIG_MAX_ARITY >= 4

template <class Param1, class Param2, class Param3, class Param4, class Policies=detail::DefaultPoliciesArg>
class Signal4
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate4<Param1, Param2, Param3, Param4, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3, Param4), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3, Param4), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3, Param4))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3, Param4))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3, Param4))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3, Param4))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3, m_p4);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3; Param4 m_p4;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3, p4);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3, class Param4>
class Signal<R (Param1, Param2, Param3, Param4)>
	: public Signal4< Param1, Param2, Param3, Param4>
{};

template <typename R, class Param1, class Param2, class Param3, class Param4, typename Policies>
class Signal<R (Param1, Param2, Param3, Param4), Policies>
	: public Signal4<Param1, Param2, Param3, Param4, Policies>
{};

#endif

//N=5
#if QUICKSIG_MAX_ARITY >= 5

template <class Param1, class Param2, class Param3, class Param4, class Param5, class Policies=detail::DefaultPoliciesArg>
class Signal5
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate5<Param1, Param2, Param3, Param4, Param5, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3, Param4, Param5), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3, Param4, Param5))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3, Param4, Param5))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_p5(p5)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3, m_p4, m_p5);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3; Param4 m_p4; Param5 m_p5;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3, p4, p5);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5>
class Signal<R (Param1, Param2, Param3, Param4, Param5)>
	: public Signal5< Param1, Param2, Param3, Param4, Param5>
{};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, typename Policies>
class Signal<R (Param1, Param2, Param3, Param4, Param5), Policies>
	: public Signal5<Param1, Param2, Param3, Param4, Param5, Policies>
{};

#endif

//N=6
#if QUICKSIG_MAX_ARITY >= 6

template <class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Policies=detail::DefaultPoliciesArg>
class Signal6
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate6<Param1, Param2, Param3, Param4, Param5, Param6, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_p5(p5), m_p6(p6)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3; Param4 m_p4; Param5 m_p5; Param6 m_p6;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3, p4, p5, p6);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6)>
	: public Signal6< Param1, Param2, Param3, Param4, Param5, Param6>
{};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, typename Policies>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6), Policies>
	: public Signal6<Param1, Param2, Param3, Param4, Param5, Param6, Policies>
{};

#endif

//N=7
#if QUICKSIG_MAX_ARITY >= 7

template <class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Policies=detail::DefaultPoliciesArg>
class Signal7
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_p5(p5), m_p6(p6), m_p7(p7)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3; Param4 m_p4; Param5 m_p5; Param6 m_p6; Param7 m_p7;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3, p4, p5, p6, p7);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7)>
	: public Signal7< Param1, Param2, Param3, Param4, Param5, Param6, Param7>
{};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, typename Policies>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7), Policies>
	: public Signal7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Policies>
{};

#endif

//N=8
#if QUICKSIG_MAX_ARITY >= 8

template <class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8, class Policies=detail::DefaultPoliciesArg>
class Signal8
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)
				: m_Current(first)
				, m_Last(last)
				, m_p1(p1), m_p2(p2), m_p3(p3), m_p4(p4), m_p5(p5), m_p6(p6), m_p7(p7), m_p8(p8)
			{}

			bool HasMore() const
			{
				return m_Current != m_Last;
			}

			R CallSlot()
			{
				assert(HasMore());
				const Delegate_t& delg = m_Current->SlotDelegate;
				++m_Current;
				return delg(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			Param1 m_p1; Param2 m_p2; Param3 m_p3; Param4 m_p4; Param5 m_p5; Param6 m_p6; Param7 m_p7; Param8 m_p8;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), p1, p2, p3, p4, p5, p6, p7, p8);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)>
	: public Signal8< Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8>
{};

template <typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8, typename Policies>
class Signal<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8), Policies>
	: public Signal8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Policies>
{};

#endif


}