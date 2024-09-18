#pragma once

#include <simdjson.h>

struct ParsedJson
{
    explicit ParsedJson(std::string_view json) :
        m_document(m_parser.parse(json.data(), json.size()))
    {
    }
    simdjson::dom::parser m_parser;
    simdjson::simdjson_result<simdjson::dom::element> m_document;
};
