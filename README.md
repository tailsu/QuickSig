QuickSig
========

An implementation of the Observer pattern (a.k.a. signals and slots) for C++. It is based on the popular `FastDelegate` library, which makes it about 20 times faster than the otherwise excellent `boost::signals` library. It supports automatic connection management with support for copy-construction of delegated objects, return value combiners, slot prioritization, synchronization and allocator policies and flexible connection storage.