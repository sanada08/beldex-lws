#program once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <bmq/bmq.h>
#include <nlohmann/json_fwd.hpp>

#include "bmq/bt_serialize.h"

namespace lws
{
    class BeldexmqServer
    {
        bmq::BMQ bmq_;
        bmq::ConnectionID beldexd_conn_;

        // Connects (and blocks until connected) to beldexd.  When this returns an beldexd connection will
        // be available (and beldexd_conn_ will be set to the connection id to reach it).
        void connect_beldexd(const bmq::address& beldexd_rpc);

        public:
            // Initialize oxenmq; return a future that completes once we have connected to and initialized
            // from beldexd.
            void init( bmq::address beldexd_rpc);    
    }
}