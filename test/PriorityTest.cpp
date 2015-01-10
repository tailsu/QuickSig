// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#include "stdafx.h"

#include "../include/Signal.h"
#include "../include/SetStorage.h"

#include <boost/test/auto_unit_test.hpp>


// also test private inheritance
struct PrioritizedCalls : private QuickSig::Trackable<>
{
	friend class QuickSig::TrackableAccess;

	int call;

	PrioritizedCalls() : call(0) {}


	void a() { BOOST_CHECK_EQUAL(call, 3); call++; }
	void b() { BOOST_CHECK_EQUAL(call, 2); call++; }
	void c() { BOOST_CHECK_EQUAL(call, 1); call++; }
	void d() { BOOST_CHECK_EQUAL(call, 0); call++; }

};

template <class Sig>
static void Test()
{
	Sig sig;
	PrioritizedCalls c;
	
	sig.Connect(&c, &PrioritizedCalls::d, 2);
	sig.Connect(&c, &PrioritizedCalls::b, 0);
	sig.Connect(&c, &PrioritizedCalls::c, 1);
	sig.Connect(&c, &PrioritizedCalls::a, -1);
	sig.Emit();
}

BOOST_AUTO_TEST_CASE(PrioritizedCallsTest)
{
	Test<QuickSig::Signal0<QuickSig::SortedArrayStorage>>();
	Test<QuickSig::Signal0<QuickSig::SetStorage>>();
}
