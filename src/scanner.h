#pragma once

#include <atomic>
#include <boost/optional/optional.hpp>
#include <cstdint>
#include <string>

namespace lws
{
    class scanner
    {
        static std::atomic<bool> running;
    
    public:

      static bool is_running() noexcept { return running; }
    };
}