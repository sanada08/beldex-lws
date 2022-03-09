#pragma once

#include <cassert>
#include <cstdint>
#include <iosfwd>
#include <utility>

#include "lmdb/util.h"
#include "wire/fwd.h"
#include "crypto/crypto.h"

namespace lws
{
    namespace db
    {
        //! References a block height
        enum class block_id : std::uint64_t{};
        // WIRE_AS_INTEGER(block_id);

        struct block_info
        {
            block_id id; //!< Must be first for LMDB optimizations
            crypto::hash hash;
        };
        static_assert(sizeof(block_info) == 8 + 32, "padding in block_info");
        // WIRE_DECLARE_OBJECT(block_info);
    }
}