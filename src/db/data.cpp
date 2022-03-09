#include "data.h"

#include <cstring>
#include <memory>

#include "wire.h"

namespace lws
{
    namespace db
    {
        namespace
        {
            template <typename F, typename T>
            void map_output_id(F &format, T &self)
            {
                wire::object(format, WIRE_FIELD(high), WIRE_FIELD(low));
            }
        }
        WIRE_DEFINE_OBJECT(output_id, map_output_id);
    }
}