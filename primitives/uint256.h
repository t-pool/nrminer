#pragma once

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

#include "nrghash/nrghash.h"

/**
 * @class base_blob
 **/
template<unsigned int BITS>
class base_blob
{
protected:
    enum {
        WIDTH = BITS / 8
    };
    uint8_t data[WIDTH];

public:
    base_blob()
    {
        std::memset(data, 0, sizeof(data));
    }

    explicit base_blob(const std::vector<unsigned char>& vch);

    bool IsNull() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (data[i] != 0)
                return false;
        return true;
    }

    void SetNull()
    {
        std::memset(data, 0, sizeof(data));
    }

    //! @brief friend operators
    //! {
public:
    friend inline bool operator==(const base_blob& a, const base_blob& b)
    {
        return memcmp(a.data, b.data, sizeof(a.data)) == 0;
    }
    friend inline bool operator!=(const base_blob& a, const base_blob& b)
    {
        return memcmp(a.data, b.data, sizeof(a.data)) != 0;
    }
    friend inline bool operator<(const base_blob& a, const base_blob& b)
    {
        return memcmp(a.data, b.data, sizeof(a.data)) < 0;
    }
    //! }

    //! @breif set/get functions
    //! {
public:
    /// @brief returns hexadecimal value
    std::string GetHex() const;
    /// @brief set value
    void SetHex(const char* psz);
    /// @brief set value
    void SetHex(const std::string& str);
    /// @brief convert value to string
    std::string ToString() const;
    //! }

    /// @brief inline functions which are providing access to local data
    /// {
public:
    inline unsigned char* begin()
    {
        return &data[0];
    }

    inline unsigned char* end()
    {
        return &data[WIDTH];
    }

    inline const unsigned char* begin() const
    {
        return &data[0];
    }

    inline const unsigned char* end() const
    {
        return &data[WIDTH];
    }

    inline unsigned int size() const
    {
        return sizeof(data);
    }

    inline unsigned int GetSerializeSize(int /*nType*/, int /*nVersion*/) const
    {
        return sizeof(data);
    }
    /// }
    template<typename Stream>
    void Serialize(Stream& s, int/* nType*/, int/* nVersion*/) const
    {
        s.write((char*)data, sizeof(data));
    }

    template<typename Stream>
    void Unserialize(Stream& s, int/* nType*/, int/* nVersion*/)
    {
        s.read((char*)data, sizeof(data));
    }
};

/**
 * @class uint160
 * @description 160-bit opaque blob.
 * @note This type is called uint160 for historical reasons only. It is an opaque
 * blob of 160 bits and has no integer operations.
 */
class uint160 : public base_blob<160>
{
public:
    uint160()
    {
    }
    uint160(const base_blob<160>& b)
        : base_blob<160>(b)
    {
    }
    explicit uint160(const std::vector<unsigned char>& vch)
        : base_blob<160>(vch)
    {
    }
};

/**
 * @class uint256
 * @description 256-bit opaque blob.
 * @note This type is called uint256 for historical reasons only. It is an
 * opaque blob of 256 bits and has no integer operations. Use arith_uint256 if
 * those are required.
 */
class uint256 : public base_blob<256>
{
public:
    uint256()
    {
    }
    uint256(const base_blob<256>& b)
        : base_blob<256>(b)
    {
    }
    explicit uint256(const std::vector<unsigned char>& vch)
        : base_blob<256>(vch)
    {
    }

    explicit uint256(const nrghash::h256_t & h)
        : base_blob<256>()
    {
        // copy to internal byte order for this type
        for (size_t i = 0; i < 32; i++)
        {
            data[31-i] = h.b[i];
        }
    }

    /**
     * @brief A more secure, salted hash function.
     * @note This hash is not stable between little and big endian.
     */
    uint64_t GetHash(const uint256& salt) const;
};

/* uint256 from const char *.
 * This is a separate function because the constructor uint256(const char*) can result
 * in dangerously catching uint256(0).
 */
inline uint256 uint256S(const char *str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

/* uint256 from std::string.
 * This is a separate function because the constructor uint256(const std::string &str) can result
 * in dangerously catching uint256(0) via std::string(const char*).
 */
inline uint256 uint256S(const std::string& str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}
