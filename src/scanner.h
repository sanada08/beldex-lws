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

      //! \return True if `stop()` has never been called.
      static bool is_running() noexcept { return running; }
      
      //! Stops all scanner instances globally.
      static void stop() noexcept { running = false; }
    };
}