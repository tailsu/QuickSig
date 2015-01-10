// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

// FastSignals.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FastSignals.h"
#include "ResultCombiners.h"
#include "Synchronization.h"
#include "SetStorage.h"
#include "FixedArrayStorage.h"

//#include <boost/pool/pool_alloc.hpp>
//
//struct PoolAllocator : public virtual DefaultPolicies
//{
//	struct Impl
//	{
//		template <class T>
//		struct Allocator : boost::pool_allocator<T>
//		{
//		};
//	};
//	typedef Impl AllocatorPolicy;
//};

struct Slotted : private Trackable<>
{
	friend class QuickSig::TrackableAccess;

	const char* m_Tag;
	Slotted(const char* tag)
	{
		m_Tag = tag;
	}

	void foo(int a)
	{
		cout << m_Tag << "::foo: " << a << endl;
	}
	void bar(int a)
	{
		cout << m_Tag << "::bar: " << a << endl;
	}

	void hook()
	{
		__hook(&Slotted::DoEvent, (Slotted*)this, &Slotted::noop);
		__hook(&Slotted::DoEvent, (Slotted*)this, &Slotted::noop1);
		__hook(&Slotted::DoEvent, (Slotted*)this, &Slotted::noop2);
		__hook(&Slotted::DoEvent, (Slotted*)this, &Slotted::noop3);
		__hook(&Slotted::DoEvent, (Slotted*)this, &Slotted::noop4);
	}

	__event void DoEvent(int x);

	void Go()
	{
		__raise DoEvent(15);
	}


	void noop(int a) {}
	void noop1(int a) {}
	void noop2(int a) {}
	void noop3(int a) {}
	void noop4(int a) {}

	void operator () (int a)
	{
	}
};

#include "../Utility/Duration.h"

void PerformanceTest()
{
	using namespace boost;

	Slotted slotted("perf"); 
	slotted.hook();

	typedef Signal1<int, FixedArrayStorage<20>> SigType;
	//SigType& FastSig = *new SigType;
	SigType FastSig;
	FastSig.Connect(&slotted, &Slotted::noop);
	FastSig.Connect(&slotted, &Slotted::noop);
	FastSig.Connect(&slotted, &Slotted::noop);
	FastSig.Connect(&slotted, &Slotted::noop);
	FastSig.Connect(&slotted, &Slotted::noop);
	FastSig.Connect(&slotted, &Slotted::operator ());

	//signal<void (int)> sig;
	//sig.connect(bind(&Slotted::noop, &slotted, _1));
	//sig.connect(bind(&Slotted::noop, &slotted, _1));
	//sig.connect(bind(&Slotted::noop, &slotted, _1));
	//sig.connect(bind(&Slotted::noop, &slotted, _1));
	//sig.connect(bind(&Slotted::noop, &slotted, _1));
	Duration dur;
	const int ITERATIONS = 20000000;

	//for (int i=0; i<ITERATIONS; ++i)
	//	sig(10);
	//dur.Stop();
	//cout << dur.Seconds() << endl;

	dur.Start();
	for (int i=0; i<ITERATIONS; ++i)
		FastSig.Emit(10);
	dur.Stop();
	cout << dur.Seconds() << endl;

	//dur.Start();
	//for (int i=0; i<ITERATIONS; ++i)
	//	slotted.Go();
	//dur.Stop();
	//cout << dur.Seconds() << endl;
}


int main()
{
	Slotted a("a");
	{
	Signal<void (int)> Sig;
	{
		Slotted b("b");

		Sig.Connect(&a, &Slotted::foo);
		Sig.Connect(&a, &Slotted::bar);
		Sig.Connect(&b, &Slotted::foo);
		Sig.Connect(&b, &Slotted::bar);

		Slotted c(a);
		c.m_Tag = "pesho";

		Sig.Emit(20);
	}
	Sig.Emit(10);
	}

	//Signal<void (int, float)> sig3;

	//PerformanceTest();
}

