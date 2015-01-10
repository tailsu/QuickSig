// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "SignalsFwd.h"

namespace QuickSig {

template <typename Result>
class ResultCombiner
{
public:
	typedef Result Result_t;
};

class NoResult : public virtual DefaultPolicies
{
public:
	struct Result : public ResultCombiner<void>
	{
		template <class Enumerator>
		void Process(Enumerator& E)
		{
			while (E.HasMore())
				E.CallSlot();
		}
	};
	typedef Result ResultPolicy;
};

}
