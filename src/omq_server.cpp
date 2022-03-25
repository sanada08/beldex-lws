#include "omq_server.h"

#include "dev_sink.h"
#include "beldex_common.h"
#include "beldex_logger.h"
#include "beldexd_key.h"

#include <chrono>
#include <exception>
#include <nlohmann/json.hpp>
#include <bmq/bt_serialize.h>
#include <bmq/hex.h>

#include <optional>
#include <stdexcept>
#include <variant>

namespace beldex
{
    bmq::bt_value json_to_bt(nlohmann::json j)
    {
        using namespace bmq;
        if (j.is_object())
        {
            bt_dict res;
            for (auto &[k, v] : j.items())
                res[k] = json_to_bt(v);
            return res;
        }
        if (j.is_array())
        {
            bt_list res;
            for (auto &v : j)
                res.push_back(json_to_bt(v));
            return res;
        }
        if (j.is_string())
            return j.get<std::string>();
        if (j.is_boolean())
            return j.get<bool>() ? 1 : 0;
        if (j.is_number_unsigned())
            return j.get<uint64_t>();
        if (j.is_number_integer())
            return j.get<int64_t>();
        BELDEX_LOG(warn, "client request returned json with an unhandled value type, unable to convert to bt");
        throw std::runtime_error{"internal error"};
    }

    nlohmann::json bt_to_json(bmq::bt_dict_consumer d)
    {
        nlohmann::json j;
        while (!d.is_finished())
        {
            std::string key{d.key()};
            if (d.is_string())
                j[key] = d.consume_string();
            else if (d.is_dict())
                j[key] = bt_to_json(d.consume_dict_consumer());
            else if (d.is_list())
                j[key] = bt_to_json(d.consume_list_consumer());
            else if (d.is_negative_integer())
                j[key] = d.consume_integer<int64_t>();
            else if (d.is_integer())
                j[key] = d.consume_integer<uint64_t>();
            else
                assert(!"invalid bt type!");
        }
        return j;
    }

    nlohmann::json bt_to_json(bmq::bt_list_consumer l)
    {
        nlohmann::json j = nlohmann::json::array();
        while (!l.is_finished())
        {
            if (l.is_string())
                j.push_back(l.consume_string());
            else if (l.is_dict())
                j.push_back(bt_to_json(l.consume_dict_consumer()));
            else if (l.is_list())
                j.push_back(bt_to_json(l.consume_list_consumer()));
            else if (l.is_negative_integer())
                j.push_back(l.consume_integer<int64_t>());
            else if (l.is_integer())
                j.push_back(l.consume_integer<uint64_t>());
            else
                assert(!"invalid bt type!");
        }
        return j;
    }

    void bmq_logger(bmq::LogLevel level, const char *file, int line,
                    std::string message)
    {
#define LMQ_LOG_MAP(LMQ_LVL, SS_LVL)                            \
    case bmq::LogLevel::LMQ_LVL:                                \
        BELDEX_LOG(SS_LVL, "[{}:{}]: {}", file, line, message); \
        break;

        switch (level)
        {
            LMQ_LOG_MAP(fatal, critical);
            LMQ_LOG_MAP(error, err);
            LMQ_LOG_MAP(warn, warn);
            LMQ_LOG_MAP(info, info);
            LMQ_LOG_MAP(trace, trace);
            LMQ_LOG_MAP(debug, debug);
        }
#undef LMQ_LOG_MAP
    }

    

} // namespace beldex
