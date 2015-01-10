// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#include "stdafx.h"

#include "../include/Signal.h"
#include "../include/ResultCombiners.h"
#include <boost/test/auto_unit_test.hpp>

template <bool retval>
struct CombinerTester1 : public QuickSig::Trackable<>
{
	bool DefaultRetval(bool)
	{
		return retval;
	}

	bool PassthroughVal(bool b)
	{
		return b;
	}
};

int val1(int a)
{
	return a;
}

int val2(int a)
{
	return a * 2;
}

int val3(int a)
{
	return a * 3;
}

struct DisconnectionTest : public QuickSig::Trackable<>
{
	int val(int a)
	{
		return a;
	}
};

//also a good test of connection/disconnection
BOOST_AUTO_TEST_CASE(ResultCombinerTest)
{
	QuickSig::Signal1<bool, QuickSig::All<false>> sig1;
	CombinerTester1<true> allTester;
	BOOST_CHECK(sig1.Emit(true));
	sig1.Connect(&allTester, &CombinerTester1<true>::DefaultRetval);
	sig1.Connect(&allTester, &CombinerTester1<true>::PassthroughVal);
	BOOST_CHECK(!sig1.Emit(false));
	BOOST_CHECK(sig1.Emit(true));
	sig1.DisconnectAll();
	BOOST_CHECK(sig1.Emit(false));

	CombinerTester1<false> anyTester;
	QuickSig::Signal1<bool, QuickSig::Any<false>> sig2;
	BOOST_CHECK(!sig2.Emit(true));
	sig2.Connect(&anyTester, &CombinerTester1<false>::DefaultRetval);
	sig2.Connect(&anyTester, &CombinerTester1<false>::PassthroughVal);
	BOOST_CHECK(!sig2.Emit(false));
	BOOST_CHECK(sig2.Emit(true));
	anyTester.DisconnectAllSlots();
	BOOST_CHECK(!sig2.Emit(true));

	typedef fastdelegate::FastDelegate1<int, int> TestDel;
	BOOST_CHECK(TestDel(&val1) == &val1);

	QuickSig::Signal1<int, QuickSig::Sum<int>> sig3;
	BOOST_CHECK_EQUAL(sig3.Emit(5), 0);
	BOOST_CHECK_EQUAL(sig3.Emit(5, 10), 10);
	sig3.Connect(&val1);
	sig3.Connect(&val2);
	sig3.Connect(&val3);
	BOOST_CHECK_EQUAL(sig3.Emit(0), 0);
	BOOST_CHECK_EQUAL(sig3.Emit(1), 6);
	BOOST_CHECK_EQUAL(sig3.Emit(2, 3), 15);
	sig3.Disconnect(&val1);
	BOOST_CHECK_EQUAL(sig3.Emit(1), 5);
	sig3.DisconnectAll();
	BOOST_CHECK_EQUAL(sig3.Emit(2), 0);

	DisconnectionTest disc;
	sig3.Connect(&disc, &DisconnectionTest::val);
	BOOST_CHECK_EQUAL(sig3.Emit(2), 2);
	sig3.Disconnect(&disc, &DisconnectionTest::val);
	BOOST_CHECK_EQUAL(sig3.Emit(2), 0);
}
