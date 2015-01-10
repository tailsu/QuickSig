// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "ResultCombiner.h"

namespace QuickSig {

template <bool ShortCircuitEvaluation>
struct All : public virtual DefaultPolicies
{
	struct Result : public ResultCombiner<bool>
	{
		template <class Enumerator>
		bool Process(Enumerator& E)
		{
			bool result = true;
			while (E.HasMore())
				if (!E.CallSlot())
				{
					result = false;
					if (ShortCircuitEvaluation)
						break;
				}
			return result;
		}
	};
	typedef Result ResultPolicy;
};

template <bool ShortCircuitEvaluation>
struct Any : public virtual DefaultPolicies
{
	struct Result : public ResultCombiner<bool>
	{
		template <class Enumerator>
		bool Process(Enumerator& E)
		{
			bool result = false;
			while (E.HasMore())
				if (E.CallSlot())
				{
					result = true;
					if (ShortCircuitEvaluation)
						break;
				}
			return result;
		}
	};
	typedef Result ResultPolicy;
};

template <typename R>
struct Sum : public virtual DefaultPolicies
{
	struct Result : public ResultCombiner<R>
	{
		Result(R value = 0)
			: m_Value(value)
		{
		}

		template <class Enumerator>
		R Process(Enumerator& E)
		{
			while (E.HasMore())
				m_Value += E.CallSlot();
			return m_Value;
		}

		R m_Value;
	};
	typedef Result ResultPolicy;
};

}
