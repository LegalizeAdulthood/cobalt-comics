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

simdjson::dom::object find_issue(DatabasePtr db, int issue);

enum class CreditField
{
    NONE = 0,
    SCRIPT = 1,
    PENCIL = 2,
    INK = 3,
    COLOR = 4,
    LETTER = 5
};

inline std::string_view to_string(CreditField field)
{
    switch (field)
    {
    case CreditField::NONE:
        return "none";
    case CreditField::SCRIPT:
        return "script";
    case CreditField::PENCIL:
        return "pencils";
    case CreditField::INK:
        return "inks";
    case CreditField::COLOR:
        return "colors";
    case CreditField::LETTER:
        return "letters";
    }
    return "?";
}

struct SequenceMatch
{
    simdjson::dom::object issue;
    simdjson::dom::object sequence;
};

} // namespace comics
