#include "stdafx.h"

#include "../include/Signal.h"
#include "../include/HashSetStorage.h"

#include <boost/test/auto_unit_test.hpp>
#include <hash_map>

struct foo : public QuickSig::Trackable<>
{
	void bar() {}
};

BOOST_AUTO_TEST_CASE(HashTest)
{
	QuickSig::Signal0<QuickSig::HashSetStorage> sig;
	foo f;
	sig.Connect(&f, &foo::bar);
	sig.Disconnect(&f, &foo::bar);

	sig.Connect(&f, &foo::bar);

	sig.Emit();
}
