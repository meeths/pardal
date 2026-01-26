#pragma once

// Created on 2026-01-26 by franciscom

namespace pdl
{
	template <typename ObjectType>
	class PoolHandle final
	{
	public:
		PoolHandle() = default;

		explicit PoolHandle(void* ptr)
			: m_index(reinterpret_cast<ptrdiff_t>(ptr) & 0xffffffff)
			, m_generation((reinterpret_cast<ptrdiff_t>(ptr) >> 32) & 0xffffffff)
		{
		}

		bool IsEmpty() const
		{
			return m_generation == 0;
		}

		bool IsValid() const
		{
			return m_generation != 0;
		}

		uint32_t GetIndex() const
		{
			return m_index;
		}

		uint32_t GetGeneration() const
		{
			return m_generation;
		}

		void* GetIndexAsVoid() const
		{
			return reinterpret_cast<void*>(static_cast<ptrdiff_t>(m_index));
		}

		void* GetHandleAsVoid() const
		{
			static_assert(sizeof(void*) >= sizeof(uint64_t));
			return reinterpret_cast<void*>((static_cast<ptrdiff_t>(m_generation) << 32) + static_cast<ptrdiff_t>(m_index));
		}

		bool operator==(const PoolHandle<ObjectType>& other) const
		{
			return m_index == other.m_index && m_generation == other.m_generation;
		}

		bool operator!=(const PoolHandle<ObjectType>& other) const
		{
			return m_index != other.m_index || m_generation != other.m_generation;
		}

		explicit operator bool() const
		{
			return m_generation != 0;
		}

	private:
		PoolHandle(uint32_t index, uint32_t gen) : m_index(index), m_generation(gen)
		{
		}

		template <typename ObjectType_, typename ImplObjectType>
		friend class Pool;

		uint32_t m_index = 0;
		uint32_t m_generation = 0;
	};
}
