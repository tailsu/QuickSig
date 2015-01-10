// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "StoragePolicy.h"
#include <set>

namespace QuickSig {

class SetStorage : public virtual DefaultPolicies
{
public:
	struct Impl
	{
		template <class Slot, class Allocator>
		class Storage
		{
		public:
			typedef std::set<Slot, std::less<Slot>, Allocator> SlotContainer;
			typedef typename SlotContainer::const_iterator SlotIterator;

			void Add(const Slot& c)
			{
				m_Slots.insert(c);
			}

			template <class Pred>
			void RemoveIf(Pred p)
			{
				m_Slots.erase(
					std::remove_if(m_Slots.begin(), m_Slots.end(), p),
					SlotsEnd());
			}

			SlotIterator SlotsBegin() const
			{
				return m_Slots.begin();
			}
			SlotIterator SlotsEnd() const
			{
				return m_Slots.end();
			}
			void DeleteSlots()
			{
				m_Slots.clear();
			}
		private:
			SlotContainer m_Slots;
		};
	};
	typedef Impl StoragePolicy;
};

}
