#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <bmq/bmq.h>
#include <nlohmann/json_fwd.hpp>

#include "bmq/bt_serialize.h"
#include "mn_record.h"

namespace beldex {


void bmq_logger(bmq::LogLevel level, const char* file, int line,
        std::string message);

bmq::bt_value json_to_bt(nlohmann::json j);

nlohmann::json bt_to_json(bmq::bt_dict_consumer d);
nlohmann::json bt_to_json(bmq::bt_list_consumer l);


} // namespace beldex
