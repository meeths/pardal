
#pragma once
#include <cstdint>
#include <utility>
#include <type_traits>
#include <memory>
#include <initializer_list>
#include <new>
#include <functional>
#include <iterator>

// Created on 2023-12-15 by sisco

namespace pdl
{
    template <class Key, class T,
              class Hash = std::hash<Key>,
              class KeyEqual = std::equal_to<Key>,
              class Alloc = std::allocator<std::pair<const Key, T>>>
    class UnorderedMap
    {
    public:
        using key_type        = Key;
        using mapped_type     = T;
        using value_type      = std::pair<const Key, T>;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using hasher          = Hash;
        using key_equal       = KeyEqual;
        using allocator_type  = Alloc;

    private:
        enum Control : uint8_t { CTRL_EMPTY = 0, CTRL_OCCUPIED = 1, CTRL_TOMBSTONE = 2 };

        using AllocTraits = std::allocator_traits<allocator_type>;
        using ValueAlloc  = typename AllocTraits::template rebind_alloc<value_type>;
        using ValueAllocTraits = std::allocator_traits<ValueAlloc>;

        // Storage
        value_type* m_entries = nullptr;   // length == m_capacity
        uint8_t*    m_ctrl    = nullptr;   // length == m_capacity
        size_type   m_capacity = 0;        // power of two, or 0
        size_type   m_size = 0;            // number of OCCUPIED
        size_type   m_tombstones = 0;      // number of TOMBSTONE
        hasher      m_hash{};
        key_equal   m_eq{};
        ValueAlloc  m_alloc{};

        static constexpr float kMaxLoad = 0.80f; // grow when size > cap * load

        static constexpr size_type next_pow2(size_type x)
        {
            if (x <= 1) return 1;
            --x;
            x |= x >> 1;  x |= x >> 2;  x |= x >> 4;  x |= x >> 8;  x |= x >> 16;
            if constexpr (sizeof(size_type) > 4) x |= x >> 32;
            return x + 1;
        }

        static constexpr size_type min_capacity(size_type n)
        {
            size_type need = static_cast<size_type>(float(n) / kMaxLoad) + 1;
            size_type cap = next_pow2(need);
            return cap < 8 ? 8 : cap;
        }

        size_type mask() const noexcept { return m_capacity ? (m_capacity - 1) : 0; }

        size_type index_for_hash(size_type h) const noexcept { return h & mask(); }

        void allocate_buffers(size_type cap)
        {
            if (cap == 0) return;
            m_entries = ValueAllocTraits::allocate(m_alloc, cap);
            m_ctrl = static_cast<uint8_t*>(::operator new[](cap * sizeof(uint8_t), std::nothrow));
            // Initialize all control bytes to EMPTY
            std::memset(m_ctrl, CTRL_EMPTY, cap);
            m_capacity = cap;
        }

        void deallocate_buffers()
        {
            if (!m_capacity) return;
            // Destroy occupied entries
            for (size_type i = 0; i < m_capacity; ++i)
            {
                if (m_ctrl[i] == CTRL_OCCUPIED)
                {
                    ValueAllocTraits::destroy(m_alloc, &m_entries[i]);
                }
            }
            ValueAllocTraits::deallocate(m_alloc, m_entries, m_capacity);
            ::operator delete[](m_ctrl);
            m_entries = nullptr;
            m_ctrl = nullptr;
            m_capacity = 0;
            m_size = 0;
            m_tombstones = 0;
        }

        void maybe_grow_for_insert()
        {
            if (m_capacity == 0)
            {
                rehash(8);
                return;
            }
            const float load = float(m_size + 1) / float(m_capacity);
            if (load > kMaxLoad || (m_tombstones > (m_capacity >> 2)))
            {
                rehash(m_capacity << 1);
            }
        }

        template <class K>
        size_type find_slot(const K& key) const noexcept
        {
            if (m_capacity == 0) return npos();
            size_type h = static_cast<size_type>(m_hash(key));
            size_type idx = index_for_hash(h);
            for (;;)
            {
                uint8_t c = m_ctrl[idx];
                if (c == CTRL_EMPTY) return npos();
                if (c == CTRL_OCCUPIED && m_eq(m_entries[idx].first, key)) return idx;
                idx = (idx + 1) & mask();
            }
        }

        static constexpr size_type npos() noexcept { return static_cast<size_type>(-1); }

        template <class K, class... Vs>
        std::pair<size_type, bool> emplace_internal(K&& key, Vs&&... vs)
        {
            maybe_grow_for_insert();
            size_type h = static_cast<size_type>(m_hash(key));
            size_type idx = index_for_hash(h);
            size_type first_tomb = npos();
            for (;;)
            {
                uint8_t c = m_ctrl[idx];
                if (c == CTRL_EMPTY)
                {
                    size_type place = (first_tomb != npos()) ? first_tomb : idx;
                    if (m_ctrl[place] == CTRL_TOMBSTONE) --m_tombstones;
                    ValueAllocTraits::construct(m_alloc, &m_entries[place],
                                                value_type(std::forward<K>(key), T(std::forward<Vs>(vs)...)));
                    m_ctrl[place] = CTRL_OCCUPIED;
                    ++m_size;
                    return { place, true };
                }
                if (c == CTRL_OCCUPIED && m_eq(m_entries[idx].first, key))
                {
                    return { idx, false };
                }
                if (c == CTRL_TOMBSTONE && first_tomb == npos()) first_tomb = idx;
                idx = (idx + 1) & mask();
            }
        }

        void reinsert_all(value_type* old_entries, uint8_t* old_ctrl, size_type old_cap)
        {
            for (size_type i = 0; i < old_cap; ++i)
            {
                if (old_ctrl[i] == CTRL_OCCUPIED)
                {
                    emplace_internal(old_entries[i].first, old_entries[i].second);
                    // Destroy moved-from old value_type
                    old_entries[i].~value_type();
                }
            }
        }

    public:
        // Iterators that skip empty/tombstone slots
        class iterator
        {
            UnorderedMap* m_map = nullptr;
            size_type m_index = 0;
            void skip() noexcept
            {
                if (!m_map) return;
                while (m_index < m_map->m_capacity && m_map->m_ctrl[m_index] != CTRL_OCCUPIED) ++m_index;
            }
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = UnorderedMap::value_type;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            iterator() = default;
            iterator(UnorderedMap* m, size_type i) : m_map(m), m_index(i) { skip(); }
            reference operator*() const { return m_map->m_entries[m_index]; }
            pointer operator->() const { return &m_map->m_entries[m_index]; }
            iterator& operator++() { ++m_index; skip(); return *this; }
            iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator==(const iterator& a, const iterator& b) { return a.m_map == b.m_map && a.m_index == b.m_index; }
            friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }
        };

        class const_iterator
        {
            const UnorderedMap* m_map = nullptr;
            size_type m_index = 0;
            void skip() noexcept
            {
                if (!m_map) return;
                while (m_index < m_map->m_capacity && m_map->m_ctrl[m_index] != CTRL_OCCUPIED) ++m_index;
            }
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const UnorderedMap::value_type;
            using difference_type = std::ptrdiff_t;
            using pointer = const value_type*;
            using reference = const value_type&;

            const_iterator() = default;
            const_iterator(const UnorderedMap* m, size_type i) : m_map(m), m_index(i) { skip(); }
            reference operator*() const { return m_map->m_entries[m_index]; }
            pointer operator->() const { return &m_map->m_entries[m_index]; }
            const_iterator& operator++() { ++m_index; skip(); return *this; }
            const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.m_map == b.m_map && a.m_index == b.m_index; }
            friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }
        };

        // ctors/dtor
        UnorderedMap() noexcept = default;

        explicit UnorderedMap(size_type bucket_count,
                              const Hash& h = Hash(),
                              const KeyEqual& eq = KeyEqual(),
                              const Alloc& alloc = Alloc())
            : m_hash(h), m_eq(eq), m_alloc(alloc)
        {
            rehash(bucket_count);
        }

        UnorderedMap(std::initializer_list<value_type> init,
                     size_type bucket_count = 0,
                     const Hash& h = Hash(),
                     const KeyEqual& eq = KeyEqual(),
                     const Alloc& alloc = Alloc())
            : m_hash(h), m_eq(eq), m_alloc(alloc)
        {
            if (bucket_count < init.size()) bucket_count = init.size();
            reserve(bucket_count);
            for (const auto& kv : init) insert(kv);
        }

        UnorderedMap(const UnorderedMap& other)
            : m_hash(other.m_hash), m_eq(other.m_eq), m_alloc(AllocTraits::select_on_container_copy_construction(other.m_alloc))
        {
            if (other.m_capacity)
            {
                allocate_buffers(other.m_capacity);
                for (size_type i = 0; i < other.m_capacity; ++i)
                {
                    if (other.m_ctrl[i] == CTRL_OCCUPIED)
                    {
                        ValueAllocTraits::construct(m_alloc, &m_entries[i], other.m_entries[i]);
                        m_ctrl[i] = CTRL_OCCUPIED;
                        ++m_size;
                    }
                    else
                    {
                        m_ctrl[i] = other.m_ctrl[i];
                        if (other.m_ctrl[i] == CTRL_TOMBSTONE) ++m_tombstones;
                    }
                }
            }
        }

        UnorderedMap(UnorderedMap&& other) noexcept
            : m_entries(other.m_entries), m_ctrl(other.m_ctrl), m_capacity(other.m_capacity),
              m_size(other.m_size), m_tombstones(other.m_tombstones), m_hash(other.m_hash),
              m_eq(other.m_eq), m_alloc(std::move(other.m_alloc))
        {
            other.m_entries = nullptr; other.m_ctrl = nullptr; other.m_capacity = 0; other.m_size = 0; other.m_tombstones = 0;
        }

        ~UnorderedMap() { deallocate_buffers(); }

        UnorderedMap& operator=(const UnorderedMap& other)
        {
            if (this == &other) return *this;
            deallocate_buffers();
            m_hash = other.m_hash; m_eq = other.m_eq;
            if constexpr (AllocTraits::propagate_on_container_copy_assignment::value)
            {
                m_alloc = other.m_alloc;
            }
            if (other.m_capacity)
            {
                allocate_buffers(other.m_capacity);
                for (size_type i = 0; i < other.m_capacity; ++i)
                {
                    if (other.m_ctrl[i] == CTRL_OCCUPIED)
                    {
                        ValueAllocTraits::construct(m_alloc, &m_entries[i], other.m_entries[i]);
                        m_ctrl[i] = CTRL_OCCUPIED;
                        ++m_size;
                    }
                    else
                    {
                        m_ctrl[i] = other.m_ctrl[i];
                        if (other.m_ctrl[i] == CTRL_TOMBSTONE) ++m_tombstones;
                    }
                }
            }
            return *this;
        }

        UnorderedMap& operator=(UnorderedMap&& other) noexcept
        {
            if (this == &other) return *this;
            deallocate_buffers();
            m_entries = other.m_entries; m_ctrl = other.m_ctrl; m_capacity = other.m_capacity;
            m_size = other.m_size; m_tombstones = other.m_tombstones;
            m_hash = other.m_hash; m_eq = other.m_eq; m_alloc = std::move(other.m_alloc);
            other.m_entries = nullptr; other.m_ctrl = nullptr; other.m_capacity = 0; other.m_size = 0; other.m_tombstones = 0;
            return *this;
        }

        // capacity
        size_type size() const noexcept { return m_size; }
        bool empty() const noexcept { return m_size == 0; }

        // iterators
        iterator begin() noexcept { return iterator(this, 0); }
        iterator end() noexcept { return iterator(this, m_capacity); }
        const_iterator begin() const noexcept { return const_iterator(this, 0); }
        const_iterator end() const noexcept { return const_iterator(this, m_capacity); }
        const_iterator cbegin() const noexcept { return const_iterator(this, 0); }
        const_iterator cend() const noexcept { return const_iterator(this, m_capacity); }

        // element access
        T& operator[](const Key& key)
        {
            auto idx = find_slot(key);
            if (idx != npos()) return m_entries[idx].second;
            auto res = emplace_internal(key);
            return m_entries[res.first].second;
        }
        T& operator[](Key&& key)
        {
            auto idx = find_slot(key);
            if (idx != npos()) return m_entries[idx].second;
            auto res = emplace_internal(std::move(key));
            return m_entries[res.first].second;
        }
        T& at(const Key& key) { return (*this)[key]; }
        const T& at(const Key& key) const
        {
            auto idx = find_slot(key);
            return idx == npos() ? const_cast<UnorderedMap*>(this)->operator[](key) : m_entries[idx].second;
        }

        // lookup
        size_type count(const Key& key) const noexcept { return find_slot(key) != npos() ? 1u : 0u; }
        bool contains(const Key& key) const noexcept { return find_slot(key) != npos(); }

        iterator find(const Key& key) noexcept
        {
            auto idx = find_slot(key);
            return (idx == npos()) ? end() : iterator(this, idx);
        }
        const_iterator find(const Key& key) const noexcept
        {
            auto idx = find_slot(key);
            return (idx == npos()) ? cend() : const_iterator(this, idx);
        }

        // modifiers
        std::pair<iterator, bool> insert(const value_type& v)
        {
            auto [idx, inserted] = emplace_internal(v.first, v.second);
            return { iterator(this, inserted ? idx : find_slot(v.first)), inserted };
        }
        std::pair<iterator, bool> insert(value_type&& v)
        {
            auto [idx, inserted] = emplace_internal(v.first, std::move(v.second));
            return { iterator(this, inserted ? idx : find_slot(v.first)), inserted };
        }

        template <class K, class... Args>
        std::pair<iterator, bool> emplace(K&& key, Args&&... args)
        {
            auto [idx, inserted] = emplace_internal(std::forward<K>(key), std::forward<Args>(args)...);
            return { iterator(this, idx), inserted };
        }

        size_type erase(const Key& key) noexcept
        {
            if (m_capacity == 0) return 0;
            size_type h = static_cast<size_type>(m_hash(key));
            size_type idx = index_for_hash(h);
            for (;;)
            {
                uint8_t c = m_ctrl[idx];
                if (c == CTRL_EMPTY) return 0;
                if (c == CTRL_OCCUPIED && m_eq(m_entries[idx].first, key))
                {
                    // destroy and mark tombstone
                    ValueAllocTraits::destroy(m_alloc, &m_entries[idx]);
                    m_ctrl[idx] = CTRL_TOMBSTONE;
                    --m_size;
                    ++m_tombstones;
                    return 1;
                }
                idx = (idx + 1) & mask();
            }
        }

        void clear() noexcept { deallocate_buffers(); }

        void swap(UnorderedMap& other) noexcept
        {
            using std::swap;
            swap(m_entries, other.m_entries);
            swap(m_ctrl, other.m_ctrl);
            swap(m_capacity, other.m_capacity);
            swap(m_size, other.m_size);
            swap(m_tombstones, other.m_tombstones);
            swap(m_hash, other.m_hash);
            swap(m_eq, other.m_eq);
            if constexpr (AllocTraits::propagate_on_container_swap::value)
            {
                swap(m_alloc, other.m_alloc);
            }
        }

        void reserve(size_type n)
        {
            size_type desired = min_capacity(n);
            if (desired > m_capacity) rehash(desired);
        }

        void rehash(size_type n)
        {
            size_type new_cap = (n == 0) ? 0 : next_pow2(n);
            if (new_cap < 8 && n != 0) new_cap = 8;

            // Save old
            value_type* old_entries = m_entries;
            uint8_t*  old_ctrl = m_ctrl;
            size_type old_cap = m_capacity;

            // Reset current
            m_entries = nullptr; m_ctrl = nullptr; m_capacity = 0; m_size = 0; m_tombstones = 0;

            if (new_cap == 0)
            {
                // just destroy old
                if (old_cap)
                {
                    for (size_type i = 0; i < old_cap; ++i)
                        if (old_ctrl[i] == CTRL_OCCUPIED) old_entries[i].~value_type();
                    ValueAllocTraits::deallocate(m_alloc, old_entries, old_cap);
                    ::operator delete[](old_ctrl);
                }
                return;
            }

            allocate_buffers(new_cap);
            if (old_cap)
            {
                reinsert_all(old_entries, old_ctrl, old_cap);
                // Free old buffers
                ValueAllocTraits::deallocate(m_alloc, old_entries, old_cap);
                ::operator delete[](old_ctrl);
            }
        }

        // Concatenate contents of another map into this one.
        // Existing keys in this map will be overwritten by values from 'other'.
        void concat(const UnorderedMap& other)
        {
            if (other.m_size == 0) return;
            reserve(m_size + other.m_size);
            for (const auto& kv : other)
            {
                (*this)[kv.first] = kv.second;
            }
        }

        // Move-aware concatenate. Values from 'other' are moved into this map.
        // Existing keys in this map will be overwritten by values from 'other'.
        void concat(UnorderedMap&& other)
        {
            if (other.m_size == 0) return;
            reserve(m_size + other.m_size);
            for (auto& kv : other)
            {
                (*this)[kv.first] = std::move(kv.second);
            }
        }
    };

    /// Given a mapping function, create a reverse map from To to From.
    /// The mapping function func must be bijective.
    /// The reverse map will return defaultValue if the value is not found.
    template<typename From, typename To, typename Func>
    auto ReverseMap(Func func, From min, From max, From defaultValue = From(0))
    {
        static UnorderedMap<To, From> reverseMap = [&]()
        {
            UnorderedMap<To, From> map;
            for (int i = int(min); i <= int(max); i++)
            {
                map[func(From(i))] = From(i);
            }
            return map;
        }();
        return [defaultValue](To value) -> From
        {
            auto it = reverseMap.find(value);
            return it == reverseMap.end() ? defaultValue : it->second;
        };
    }

}

