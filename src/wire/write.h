#pragma once

#include <array>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <type_traits>

#include "byte_slice.h" // monero/contrib/epee/include
#include "span.h" 

namespace wire
{
    virtual void start_object(std::size_t) = 0
} // namespace wire

namespace wire_write
{
    
}
