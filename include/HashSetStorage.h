// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "Config.h"

#ifndef QUICKSIG_USE_GOOGLE_HASH
#error Google Hash isn't enabled
#endif

#include "StoragePolicy.h"

#include <google/dense_hash_set>

namespace QuickSig {

	class HashSetStorage : public virtual DefaultPolicies
	{
	public:
		struct Impl
		{
			template <class Slot, class Allocator>
			class Storage
			{
			public:
				typedef google::dense_hash_set<Slot, typename Slot::Hash, std::equal_to<Slot>, Allocator> SlotContainer;
				typedef typename SlotContainer::const_iterator SlotIterator;

				void Add(const Slot& c)
				{
					assert(c.Priority == 0);
					m_Slots.insert(c);
				}

				template <class Pred>
				void RemoveIf(Pred p)
				{
					for (SlotContainer::iterator it = m_Slots.begin(); it != m_Slots.end(); )
						if (p(*it))
							m_Slots.erase(it++);
						else
							++it;
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
