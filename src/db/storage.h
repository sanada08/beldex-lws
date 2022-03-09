// Copyright (c) 2018, The Monero Project
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include <iosfwd>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "common/expect.h"
#include "lmdb/transaction.h"
#include "lmdb/key_stream.h"
#include "lmdb/value_stream.h"

#include "db/data.h"

namespace lws
{
    namespace db
    {
        struct storage_internal;
        //! Wrapper for LMDB on-disk storage of light-weight server data.
        class storage
        {
            std::shared_ptr<storage_internal> db;

            storage(std::shared_ptr<storage_internal> db) noexcept
                : db(std::move(db))
            {
            }

        public:
            /*!
              Open a light_wallet_server LDMB database.

              \param path Directory for LMDB storage
              \param create_queue_max Maximum number of create account requests allowed.

              \throw std::system_error on any LMDB error (all treated as fatal).
              \throw std::bad_alloc If `std::shared_ptr` fails to allocate.

              \return A ready light-wallet server database.
            */
            static storage open(const char* path);
        };

    } // db
} // lws
