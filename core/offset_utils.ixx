export module offset_utils;

import <cstdint>;
import <numeric>;
import <cassert>;

#pragma warning(push)
#pragma warning(disable : 4312)  // Disable warnings for reinterpret cast from pointer to byte

inline bool
DataCompare(const uint8_t* pData, const uint8_t* bMask, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
    {
        if (*szMask == 'x' && *pData != *bMask)
            return false;
    }
    return (*szMask) == 0;
}

inline uintptr_t
FindPattern(uintptr_t dwAddress, uintptr_t dwLen, const uint8_t* bPattern, const char* szMask)
{
    for (uintptr_t i = 0; i < dwLen; i++)
    {
        if (DataCompare(reinterpret_cast<uint8_t*>(dwAddress + i), bPattern, szMask))
            return dwAddress + i;
    }

    return 0;
}

inline uintptr_t
GetDWordAt(uintptr_t address, uintptr_t numBytes)
{
    if (address)
    {
        address += numBytes;
        return *reinterpret_cast<uintptr_t*>(address);
    }

    return 0;
}

export uintptr_t
GetFunctionAddressAt(uintptr_t address, uintptr_t addressOffset, uintptr_t numBytes)
{
    if (address)
    {
        uintptr_t displacement = *reinterpret_cast<uintptr_t*>(address + addressOffset);
        return address + numBytes + displacement;
    }

    return 0;
}

#pragma warning(pop)

export template <typename... Args>
struct ForeignPointer_StorageBase;

export template <typename T>
struct ForeignPointer_StorageBase<T> {
    operator T*() const noexcept
    {
        return get();
    }

    void set(T* value) noexcept
    {
        if (m_ptr)
        {
            *m_ptr = value;
        }
    }

    T* get() const noexcept
    {
        return m_ptr ? *m_ptr : nullptr;
    }

    T** get_raw() const noexcept
    {
        return m_ptr;
    }

   protected:
    template <typename U>
    U* coerced_get() const noexcept
    {
        return (U*) get();
    }

    T** m_ptr = 0;
};

export template <typename T, typename U, typename... Rest>
struct ForeignPointer_StorageBase<T, U, Rest...> : public ForeignPointer_StorageBase<T, Rest...> {
    operator U*() const noexcept
    {
        return this->coerced_get<U>();
    }

    bool operator==(U* other) const noexcept
    {
        return this->coerced_get<U>() == other;
    }

    bool operator!=(U* other) const noexcept
    {
        return this->coerced_get<U>() != other;
    }

    using ForeignPointer_StorageBase<T, Rest...>::set;

    void set(U* value) noexcept
    {
        if (this->m_ptr)
        {
            *this->m_ptr = reinterpret_cast<T*>(value);
        }
    }
};

export template <typename T, typename... Conversions>
class ForeignPointer : public ForeignPointer_StorageBase<T, Conversions...>
{
   public:
    ForeignPointer() noexcept  = default;
    ~ForeignPointer() noexcept = default;

    ForeignPointer(uintptr_t addr) noexcept
    {
        this->m_ptr = reinterpret_cast<T**>(addr);
    }
    ForeignPointer(const ForeignPointer& other) noexcept
    {
        this->m_ptr = other.m_ptr;
    }
    ForeignPointer(ForeignPointer&& other) noexcept
    {
        this->m_ptr = other.m_ptr;
    }

    ForeignPointer& operator=(const ForeignPointer& other) noexcept
    {
        this->m_ptr = other.m_ptr;
        return *this;
    }

    ForeignPointer& operator=(ForeignPointer&& other) noexcept
    {
        this->m_ptr = other.m_ptr;
        return *this;
    }

    ForeignPointer& operator=(uintptr_t other) noexcept
    {
        set_offset(other);
        return *this;
    }

    ForeignPointer& operator=(nullptr_t) noexcept
    {
        if (this->m_ptr)
        {
            *this->m_ptr = nullptr;
        }

        return *this;
    }

    template <typename U>
    ForeignPointer& operator=(U* other)
    {
        this->set(other);
        return *this;
    }

    bool operator==(nullptr_t) const noexcept
    {
        return this->get() == nullptr;
    }

    bool operator!=(nullptr_t) const noexcept
    {
        return this->get() != nullptr;
    }

    template <typename... T>
    bool operator==(const ForeignPointer<T...>& other)
    {
        return this->get() == other.get();
    }

    template <typename... T>
    bool operator!=(const ForeignPointer<T...>& other)
    {
        return this->get() != other.get();
    }

    T& operator*() const noexcept
    {
        assert(this->m_ptr != nullptr && *this->m_ptr != nullptr);
        return **this->m_ptr;
    }

    T* operator->() const noexcept
    {
        return this->get();
    }

    explicit operator bool() const noexcept
    {
        return is_valid();
    }

    operator void*() const noexcept
    {
        return this->get();
    }

    template <typename U>
    U* get_as() const noexcept
    {
        return (U*) this->get();
    }

    bool is_valid() const noexcept
    {
        return this->m_ptr && *this->m_ptr;
    }

    uintptr_t get_offset() const noexcept
    {
        return reinterpret_cast<uintptr_t>(this->m_ptr);
    }
    void set_offset(uintptr_t offset) noexcept
    {
        this->m_ptr = reinterpret_cast<T**>(offset);
    }
    void set_offset(T** offset) noexcept
    {
        this->m_ptr = offset;
    }

    void reset()
    {
        this->m_ptr = nullptr;
    }

    template <typename U, typename = std::enable_if<std::is_convertible_v<T, U>, void>>
    inline operator ForeignPointer<U>&()
    {
        return reinterpret_cast<ForeignPointer<U>&>(*this);
    }
};
