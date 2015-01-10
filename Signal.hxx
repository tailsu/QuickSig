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

@VARARGS

template <@CLASSARGS, class Policies=detail::DefaultPoliciesArg>
class Signal@NUM
{
private:
	typedef typename Policies::ResultPolicy::Result Result;

public:
	typedef typename Result::Result_t R;
	typedef fastdelegate::FastDelegate@NUM<@SELARGS, R> Delegate_t;

	template <class C>
	bool Connect(C* self, R (C::*func)(@SELARGS), Priority_t priority=0)
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.Connect(self, delg, priority);
	}

	bool Connect(R (*func)(@SELARGS), Priority_t priority=0)
	{
		return m_SlotStorage.Connect(static_cast<Trackable<NoSync>*>(NULL), Delegate_t(func), priority);
	}

	template <class C>
	void Disconnect(C* self, R (C::*func)(@SELARGS))
	{
		m_SlotStorage.Disconnect(self, Delegate_t(self, func));
	}

	void Disconnect(R (*func)(@SELARGS))
	{
		m_SlotStorage.Disconnect(func);
	}

	void DisconnectAll()
	{
		m_SlotStorage.DisconnectAll();
	}

	template <class C>
	bool HasSlot(C* self, R (C::*func)(@SELARGS))
	{
		Delegate_t delg(self, func);
		return m_SlotStorage.SlotExists(delg);
	}

	bool HasSlot(R (*func)(@SELARGS))
	{
		return m_SlotStorage.SlotExists(func);
	}

	R Emit(@FUNCARGS, Result resultProcessor = Result()) const
	{
		typedef typename SlotStorage::SlotIterator SlotIterator;
		class SlotCaller
		{
		public:
			SlotCaller(SlotIterator first, SlotIterator last, @FUNCARGS)
				: m_Current(first)
				, m_Last(last)
				, @MEMBERINIT
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
				return delg(@MEMBERCALL);
			}

		private:
			SlotIterator m_Current;
			SlotIterator m_Last;

			@MEMBERDECL;
		};

		typename SlotStorage::Lock lk(m_SlotStorage.m_Mutex);
		SlotCaller slotCaller(m_SlotStorage.SlotsBegin(), m_SlotStorage.SlotsEnd(), @INVOKEARGS);
		return resultProcessor.Process(slotCaller);
	}

private:
	typedef typename Policies::AllocatorPolicy::Impl Allocator;
	typedef typename Policies::StoragePolicy::Impl ConcreteStoragePolicy;
	typedef typename Policies::SyncPolicy::Sync SyncPolicy;

	typedef detail::StoragePolicy<Delegate_t, ConcreteStoragePolicy, SyncPolicy, Allocator> SlotStorage;

	SlotStorage m_SlotStorage;
};

template <typename R, @CLASSARGS>
class Signal<R (@SELARGS)>
	: public Signal@NUM< @SELARGS>
{};

template <typename R, @CLASSARGS, typename Policies>
class Signal<R (@SELARGS), Policies>
	: public Signal@NUM<@SELARGS, Policies>
{};

@ENDVAR

}