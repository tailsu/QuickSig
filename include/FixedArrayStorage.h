// This file is part of the QuickSig library.
// Copyright 2008 Stefan Dragnev.
// The library and all its files are distributed under the MIT license.
// See the full text of the license in the accompanying LICENSE.txt file or at http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "StoragePolicy.h"

namespace QuickSig {

template <size_t MaxSlots>
class FixedArrayStorage : public virtual DefaultPolicies
{
public:
	struct Impl
	{
		template <class Slot, class /*Allocator*/>
		class Storage
		{
		public:
			typedef Slot SlotContainer[MaxSlots];
			typedef const Slot* SlotIterator;

			Storage()
			{
				m_SlotCount = 0;
			}

			void Add(const Slot& c)
			{
				assert(m_SlotCount < MaxSlots);
				m_Slots[m_SlotCount++] = c;
			}

			template <class Pred>
			void RemoveIf(Pred p)
			{
				Slot* newEnd = std::remove_if(m_Slots, m_Slots+m_SlotCount, p);
				m_SlotCount = newEnd - m_Slots;
			}

			SlotIterator SlotsBegin() const
			{
				return m_Slots;
			}
			SlotIterator SlotsEnd() const
			{
				return m_Slots + m_SlotCount;
			}

			void DeleteSlots()
			{
				m_SlotCount = 0;
			}
		private:
			SlotContainer m_Slots;
			size_t m_SlotCount;
		};
	};
	typedef Impl StoragePolicy;
};

}