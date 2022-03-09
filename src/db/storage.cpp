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
#include "storage.h"

#include <boost/container/static_vector.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/iterator_range.hpp>
#include <cassert>
#include <chrono>
#include <limits>
#include <string>
#include <utility>

#include "lmdb/database.h"
#include "lmdb/error.h"
#include "lmdb/key_stream.h"
#include "lmdb/table.h"
#include "lmdb/util.h"
#include "lmdb/value_stream.h"

namespace lws
{
    namespace db
    {
        struct storage_internal : lmdb::database
        {
            constexpr const lmdb::basic_table<unsigned, block_info> blocks{
                "blocks_by_id", (MDB_CREATE | MDB_DUPSORT), MONERO_SORT_BY(block_info, id)
            };
            struct tables_
            {
                MDB_dbi blocks;
            }tables;
    
            explicit storage_internal(lmdb::environment env) : lmdb::database(std::move(env)), tables{} 
            {
                lmdb::write_txn txn = this->create_write_txn().value();
                assert(txn != nullptr);
                tables.blocks      = blocks.open(*txn).value();

                MONERO_UNWRAP(this->commit(std::move(txn)));
            }
        };
        storage storage::open(const char *path)
        {
            return {
                std::make_shared<storage_internal>(
                    MONERO_UNWRAP(lmdb::open_environment(path, 20)))};
        }
    } // db
} // lws
