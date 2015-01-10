#include "stdafx.h"

#include "../include/Signal.h"
#include "../include/Synchronization.h"

#include <boost/thread/thread.hpp>
#include <boost/mpl/list.hpp>
#include <boost/bind.hpp>
#include <boost/test/auto_unit_test.hpp>

void f1() {}
void f2() {}

template <class Signal, class Sink>
void Test(Signal& signal, Sink& sink)
{
	for (int i=0; i<1000000; ++i)
	{
		switch (rand()%13)
		{
			//doesn't work due to race condition
		case 0: if (!signal.HasSlot(&sink, &Sink::func1)) signal.Connect(&sink, &Sink::func1); break;
		case 1: if (!signal.HasSlot(&sink, &Sink::func2)) signal.Connect(&sink, &Sink::func2); break;
		case 2: if (!signal.HasSlot(&sink, &Sink::func3)) signal.Connect(&sink, &Sink::func3); break;
		case 3: signal.Disconnect(&sink, &Sink::func1); break;
		case 4: signal.Disconnect(&sink, &Sink::func2); break;
		case 5: signal.Disconnect(&sink, &Sink::func3); break;
		case 6: signal.Emit(); break;
		case 7: signal.DisconnectAll(); break;
		case 8: sink.DisconnectAllSlots(); break;
		case 9: if (!signal.HasSlot(&f1)) signal.Connect(&f1); break;
		case 10: if (!signal.HasSlot(&f2)) signal.Connect(&f2); break;
		case 11: signal.Disconnect(&f1); break;
		case 12: signal.Disconnect(&f2); break;
		}
	}
}

typedef boost::mpl::list<QuickSig::InstanceLock, QuickSig::ProcesswideLock> Mutexes;
BOOST_AUTO_TEST_CASE_TEMPLATE(MultithreadingTest, Sync, Mutexes)
{
	typedef QuickSig::Signal0<Sync> Signal;
	Signal signal;

	struct Sink : QuickSig::Trackable<Sync>
	{
		void func1() {}
		void func2() {}
		void func3() {}
	} sink;

	using namespace boost;
	thread t1(bind(&Test<Signal, Sink>, ref(signal), ref(sink)));
	thread t2(bind(&Test<Signal, Sink>, ref(signal), ref(sink)));
	thread t3(bind(&Test<Signal, Sink>, ref(signal), ref(sink)));
	thread t4(bind(&Test<Signal, Sink>, ref(signal), ref(sink)));
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
