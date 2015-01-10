// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#include "stdafx.h"

#include "../include/Signal.h"

#include <boost/test/auto_unit_test.hpp>

struct foo : public QuickSig::Trackable<>
{
	void bar() {}
};

BOOST_AUTO_TEST_CASE(UnsortedVectorWithPriority)
{
	QuickSig::Signal0<QuickSig::ArrayStorage> sig;
	
	foo f;

	//sig.Connect(&f, &foo::bar, 5); // should assert
}
