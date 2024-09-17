#pragma once

#include <simdjson.h>

#include <filesystem>
#include <memory>

namespace comics
{

class Database
{
public:
    virtual ~Database() = default;
    virtual simdjson::simdjson_result<simdjson::dom::element> getIssues() const = 0;
    virtual simdjson::simdjson_result<simdjson::dom::element> getSequences() const = 0;
};

using DatabasePtr = std::shared_ptr<Database>;

DatabasePtr createDatabase(const std::filesystem::path &jsonDir);

enum class CreditField
{
    NONE = 0,
    SCRIPT = 1,
    PENCIL = 2,
    INK = 3,
    COLOR = 4,
    LETTER = 5
};

struct SequenceMatch
{
    simdjson::dom::object issue;
    simdjson::dom::object sequence;
};

} // namespace comics
