#include <comics/database.h>

#include "ends_with.h"

namespace comics
{
namespace
{

class JSONDatabase : public Database
{
public:
    JSONDatabase(const std::filesystem::path &jsonDir);
    ~JSONDatabase() override = default;

    simdjson::simdjson_result<simdjson::dom::element> getIssues() const override
    {
        return m_issues;
    }
    simdjson::simdjson_result<simdjson::dom::element> getSequences() const override
    {
        return m_sequences;
    }

private:
    simdjson::dom::parser m_issueParser;
    simdjson::simdjson_result<simdjson::dom::element> m_issues;
    simdjson::dom::parser m_sequenceParser;
    simdjson::simdjson_result<simdjson::dom::element> m_sequences;
};

JSONDatabase::JSONDatabase(const std::filesystem::path &jsonDir)
{
    bool foundIssues{false};
    bool foundSequences{false};
    for (const auto &entry : std::filesystem::directory_iterator(jsonDir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        const std::filesystem::path &path{entry.path()};
        const std::string filename{path.filename().string()};
        if (endsWith(filename, "issues.json"))
        {
            std::cout << "Reading issues...\n";
            m_issues = m_issueParser.load(path.string());
            foundIssues = true;
            std::cout << "done.\n";
            if (!m_issues.is_array())
            {
                throw std::runtime_error("JSON issues file should be an array of objects");
            }
        }
        else if (endsWith(filename, "sequences.json"))
        {
            std::cout << "Reading sequences...\n";
            m_sequences = m_sequenceParser.load(path.string());
            foundSequences = true;
            std::cout << "done.\n";
            if (!m_sequences.is_array())
            {
                throw std::runtime_error("JSON sequences file should be an array of objects");
            }
        }
    }
    if (!(foundIssues && foundSequences))
    {
        if (foundIssues)
        {
            throw std::runtime_error("Couldn't find sequences JSON file in " + jsonDir.string());
        }
        if (foundSequences)
        {
            throw std::runtime_error("Couldn't find issues JSON file in " + jsonDir.string());
        }
        throw std::runtime_error("Couldn't find either issues or sequences JSON file in " + jsonDir.string());
    }
}

} // namespace

DatabasePtr createDatabase(const std::filesystem::path &jsonDir)
{
    return std::make_shared<JSONDatabase>(jsonDir);
}

} // namespace comics
