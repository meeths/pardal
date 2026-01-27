#pragma once
#include <algorithm>

#include "PoolHandle.h"
#include "Vector.h"
#include "Base/BaseTypes.h"
#include "Base/DebugHelpers.h"

// Created on 2026-01-26 by franciscom

namespace pdl
{
	template <typename BaseT, typename ImplT>
	class Pool
	{
	private:
		static constexpr uint32 kListEndSentinel = 0xffffffff;

		struct PoolEntry
		{
			PoolEntry() = default;
			explicit PoolEntry(ImplT&& obj) : m_object(std::move(obj))
			{
			}

			ImplT m_object = {};
			uint32 m_generation = 1;
			uint32 m_nextFree = kListEndSentinel;
		};

		uint32 m_freeListHead = kListEndSentinel;
		uint32 m_numObjects = 0;
		Vector<PoolEntry> m_objects;

	public:

		PoolHandle<BaseT> Create(ImplT&& obj)
		{
			uint32 idx = 0;
			if (m_freeListHead != kListEndSentinel)
			{
				idx = m_freeListHead;
				m_freeListHead = m_objects[idx].m_nextFree;
				m_objects[idx].m_object = std::move(obj);
			}
			else
			{
				idx = static_cast<uint32>(m_objects.size());
				m_objects.emplace_back(std::move(obj));
			}
			m_numObjects++;
			return PoolHandle<BaseT>(idx, m_objects[idx].m_generation);
		}

		void Destroy(PoolHandle<BaseT> poolHandle)
		{
			if (poolHandle.IsEmpty())
				return;
			pdlAssert(m_numObjects > 0); // double deletion
			const uint32 index = poolHandle.GetIndex();
			pdlAssert(index < m_objects.size());
			pdlAssert(poolHandle.GetGeneration() == m_objects[index].m_generation); // double deletion
			m_objects[index].m_object = ImplT{};
			++m_objects[index].m_generation;
			m_objects[index].m_nextFree = m_freeListHead;
			m_freeListHead = index;
			m_numObjects--;
		}
		
		bool IsValid(PoolHandle<BaseT> _poolHandle) const
		{
			auto index = _poolHandle.GetIndex();
			if(index >= m_objects.size())
				return false;
			return _poolHandle.GetGeneration() == m_objects[index].m_generation;
		}

		const ImplT* Get(PoolHandle<BaseT> _poolHandle) const
		{
			if (_poolHandle.IsEmpty())
				return nullptr;

			const uint32 index = _poolHandle.GetIndex();
			pdlAssert(index < m_objects.size());
			pdlAssert(_poolHandle.GetGeneration() == m_objects[index].m_generation); // accessing deleted object
			return &m_objects[index].m_object;
		}

		ImplT* Get(PoolHandle<BaseT> poolHandle)
		{
			if (poolHandle.IsEmpty())
				return nullptr;

			const uint32 index = poolHandle.GetIndex();
			pdlAssert(index < m_objects.size());
			pdlAssert(poolHandle.GetGeneration() == m_objects[index].m_generation); // accessing deleted object
			return &m_objects[index].m_object;
		}

		PoolHandle<BaseT> GetPoolHandle(uint32 index) const
		{
			pdlAssert(index < static_cast<uint32>(m_objects.size()));
			if (index >= static_cast<uint32>(m_objects.size()))
				return {};

			return PoolHandle<BaseT>(index, m_objects[index].m_generation);
		}

		PoolHandle<BaseT> FindObject(const ImplT* obj) const
		{
			if (!obj)
				return {};

			for (uint32 idx = 0; idx != static_cast<uint32>(m_objects.size()); idx++)
			{
				if (&m_objects[idx].m_object == obj)
				{
					return PoolHandle<BaseT>(idx, m_objects[idx].m_generation);
				}
			}

			return {};
		}

		void Clear()
		{
			m_objects.clear();
			m_freeListHead = kListEndSentinel;
			m_numObjects = 0;
		}

		uint32 GetNumObjects() const
		{
			return m_numObjects;
		}
	};
}
