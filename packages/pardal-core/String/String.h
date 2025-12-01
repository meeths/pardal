
#pragma once

// Created on 2023-10-30 by sisco

#include <cstddef>
#include <string_view>
#include <ostream>
#include <algorithm>
#include <Containers/Vector.h>

namespace pdl
{
    class String;

    class StringView
    {
    public:
        using value_type = char;
        using size_type = std::size_t;
        using pointer = const char*;
        using const_pointer = const char*;
        using reference = const char&;
        using const_reference = const char&;
        using const_iterator = const char*;

        static constexpr size_type npos = static_cast<size_type>(-1);

        constexpr StringView() noexcept : _data(nullptr), _size(0) {}
        constexpr StringView(const char* s, size_type count) noexcept : _data(s), _size(count) {}
        constexpr StringView(const char* s) noexcept : _data(s), _size(s ? std::char_traits<char>::length(s) : 0) {}
        constexpr StringView(std::nullptr_t) noexcept : _data(nullptr), _size(0) {}
        constexpr StringView(std::string_view sv) noexcept : _data(sv.data()), _size(sv.size()) {}

        // Defined after String
        StringView(const String& s) noexcept;

        constexpr const char* data() const noexcept { return _data; }
        constexpr size_type size() const noexcept { return _size; }
        constexpr size_type length() const noexcept { return _size; }
        constexpr bool empty() const noexcept { return _size == 0; }

        constexpr const char& operator[](size_type i) const noexcept { return _data[i]; }
        constexpr const_iterator begin() const noexcept { return _data; }
        constexpr const_iterator end() const noexcept { return _data + _size; }

        constexpr StringView substr(size_type pos, size_type count = npos) const noexcept
        {
            if (pos > _size) return StringView();
            const size_type rcount = (count == npos || pos + count > _size) ? (_size - pos) : count;
            return StringView(_data + pos, rcount);
        }

        constexpr bool starts_with(StringView prefix) const noexcept
        {
            if (prefix.size() > _size) return false;
            for (size_type i = 0; i < prefix.size(); ++i)
                if (_data[i] != prefix._data[i]) return false;
            return true;
        }

        constexpr bool ends_with(StringView suffix) const noexcept
        {
            if (suffix.size() > _size) return false;
            const size_type offset = _size - suffix.size();
            for (size_type i = 0; i < suffix.size(); ++i)
                if (_data[offset + i] != suffix._data[i]) return false;
            return true;
        }

        // Interop
        constexpr operator std::string_view() const noexcept { return std::string_view(_data, _size); }

        friend constexpr bool operator==(StringView a, StringView b) noexcept
        {
            if (a._size != b._size) return false;
            if (a._size == 0) return true;
            return std::char_traits<char>::compare(a._data, b._data, a._size) == 0;
        }
        friend constexpr bool operator!=(StringView a, StringView b) noexcept { return !(a == b); }

    private:
        const char* _data;
        size_type _size;
    };

    class String
    {
    public:
        using value_type = char;
        using size_type = std::size_t;
        using pointer = char*;
        using const_pointer = const char*;
        using reference = char&;
        using const_reference = const char&;
        using iterator = char*;
        using const_iterator = const char*;

        static constexpr size_type npos = static_cast<size_type>(-1);

        String() { _buf.resize(1); _buf[0] = '\0'; }
        String(const char* s) { assign_from_cstr(s); }
        String(const char* s, size_type count) { assign_from_range(s, s ? s + count : s); }
        String(std::string s) { assign_from_range(s.data(), s.data() + s.size()); }
        template <class InputIt>
        String(InputIt first, InputIt last) { assign_from_range(first, last); }
        String(StringView sv) { assign_from_range(sv.data(), sv.data() + sv.size()); }

        // Copy/move
        String(const String&) = default;
        String(String&&) noexcept = default;
        String& operator=(const String&) = default;
        String& operator=(String&&) noexcept = default;
        String& operator=(const char* s) { assign_from_cstr(s); return *this; }
        String& operator=(std::string s) { assign_from_range(s.data(), s.data() + s.size()); return *this; }
        String& operator=(StringView sv) { assign_from_range(sv.data(), sv.data() + sv.size()); return *this; }

        // Capacity
        size_type size() const noexcept { return _buf.size() ? (_buf.size() - 1) : 0; }
        size_type length() const noexcept { return size(); }
        bool empty() const noexcept { return size() == 0; }
        size_type capacity() const noexcept { return _buf.capacity() ? (_buf.capacity() - 1) : 0; }
        void reserve(size_type cap) { _buf.reserve(cap + 1); }

        // Access
        char* data() noexcept { return _buf.data(); }
        const char* data() const noexcept { return _buf.data(); }
        const char* c_str() const noexcept { return _buf.data(); }
        reference operator[](size_type i) noexcept { return _buf[i]; }
        const_reference operator[](size_type i) const noexcept { return _buf[i]; }

        iterator begin() noexcept { return _buf.data(); }
        const_iterator begin() const noexcept { return _buf.data(); }
        const_iterator cbegin() const noexcept { return _buf.data(); }
        iterator end() noexcept { return _buf.data() + size(); }
        const_iterator end() const noexcept { return _buf.data() + size(); }
        const_iterator cend() const noexcept { return _buf.data() + size(); }

        // Modifiers
        void clear() noexcept { _buf.resize(1); _buf[0] = '\0'; }
        void push_back(char ch) { 
            const auto old = size();
            _buf.resize(old + 2);
            _buf[old] = ch;
            _buf[old + 1] = '\0';
        }
        String& append(StringView sv) { 
            const auto old = size();
            _buf.resize(old + sv.size() + 1);
            if (sv.size())
                std::copy(sv.begin(), sv.end(), _buf.data() + old);
            _buf[old + sv.size()] = '\0';
            return *this; 
        }
        String& operator+=(StringView sv) { return append(sv); }

        // Find and substring
        size_type find(StringView what, size_type pos = 0) const noexcept { 
            if (what.size() == 0) return pos <= size() ? pos : npos;
            if (what.size() > size()) return npos;
            for (size_type i = pos; i + what.size() <= size(); ++i) {
                size_type j = 0;
                for (; j < what.size(); ++j) {
                    if (_buf[i + j] != what.data()[j]) break;
                }
                if (j == what.size()) return i;
            }
            return npos;
        }
        size_type find(const char* what, size_type pos = 0) const noexcept { return find(StringView(what), pos); }
        size_type find(char ch, size_type pos = 0) const noexcept {
            for (size_type i = pos; i < size(); ++i) if (_buf[i] == ch) return i; return npos;
        }

        size_type find_first_not_of(StringView set, size_type pos = 0) const noexcept { 
            for (size_type i = pos; i < size(); ++i) {
                bool inSet = false;
                for (size_type j = 0; j < set.size(); ++j) if (_buf[i] == set.data()[j]) { inSet = true; break; }
                if (!inSet) return i;
            }
            return npos;
        }
        size_type find_last_not_of(StringView set, size_type pos = npos) const noexcept { 
            if (empty()) return npos;
            size_type i = (pos == npos || pos >= size()) ? (size() - 1) : pos;
            for (;;){
                bool inSet = false;
                for (size_type j = 0; j < set.size(); ++j) if (_buf[i] == set.data()[j]) { inSet = true; break; }
                if (!inSet) return i;
                if (i == 0) break;
                --i;
            }
            return npos;
        }

        String substr(size_type pos = 0, size_type count = npos) const {
            if (pos > size()) return String();
            size_type rcount = (count == npos || pos + count > size()) ? (size() - pos) : count;
            return String(begin() + pos, begin() + pos + rcount);
        }

        // Replace convenience used by utils
        String& replace(size_type pos, size_type count, StringView to)
        {
            if (pos > size()) return *this;
            size_type rcount = (pos + count > size()) ? (size() - pos) : count;
            pdl::Vector<char> newBuf;
            newBuf.reserve(size() - rcount + to.size());
            // before
            newBuf.resize(pos);
            if (pos)
                std::copy(_buf.data(), _buf.data() + pos, newBuf.data());
            // insert 'to'
            size_type oldSize = newBuf.size();
            newBuf.resize(oldSize + to.size());
            if (to.size())
                std::copy(to.begin(), to.end(), newBuf.data() + oldSize);
            // after
            size_type tail = size() - (pos + rcount);
            oldSize = newBuf.size();
            newBuf.resize(oldSize + tail);
            if (tail)
                std::copy(_buf.data() + pos + rcount, _buf.data() + pos + rcount + tail, newBuf.data() + oldSize);

            // assign ensuring null terminator
            assign_from_range(newBuf.data(), newBuf.data() + newBuf.size());
            return *this;
        }

        // Comparisons
        friend bool operator==(const String& a, const String& b) noexcept {
            if (a.size() != b.size()) return false;
            if (a.size() == 0) return true;
            return std::char_traits<char>::compare(a.data(), b.data(), a.size()) == 0;
        }
        friend bool operator!=(const String& a, const String& b) noexcept { return !(a == b); }

        // Interop
        operator std::string_view() const noexcept { return std::string_view(data(), size()); }
        operator std::string() const noexcept { return std::string(data(), size()); }

        // For debugging
        std::string std_str() const noexcept { return std::string(data(), size()); }

    private:
        void assign_from_cstr(const char* s) {
            if (!s) { clear(); return; }
            assign_from_range(s, s + std::char_traits<char>::length(s));
        }
        template <class It>
        void assign_from_range(It first, It last) {
            if (!first && !last) { clear(); return; }
            const size_type len = static_cast<size_type>(last - first);
            _buf.resize(len + 1);
            if (len) std::copy(first, last, _buf.data());
            _buf[len] = '\0';
        }

        pdl::Vector<char> _buf;
    };

    inline StringView::StringView(const String& s) noexcept : _data(s.data()), _size(s.size()) {}

    using WString = std::wstring;
    using WStringView = std::wstring_view;
}

// Stream output interop
inline std::ostream& operator<<(std::ostream& os, const pdl::String& s)
{
    return os << static_cast<std::string_view>(s);
}

// Relational with C strings to avoid ambiguous conversions
inline bool operator==(const pdl::String& a, const char* b) noexcept
{
    return std::string_view(a) == std::string_view(b ? b : "");
}
inline bool operator==(const char* a, const pdl::String& b) noexcept
{
    return b == a;
}
inline bool operator!=(const pdl::String& a, const char* b) noexcept { return !(a == b); }
inline bool operator!=(const char* a, const pdl::String& b) noexcept { return !(a == b); }

