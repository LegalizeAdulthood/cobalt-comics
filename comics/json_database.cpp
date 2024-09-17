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

simdjson::dom::object find_issue(DatabasePtr db, int issue)
{
    simdjson::dom::array issues = db->getIssues().get_array();
    const auto it = std::find_if(issues.begin(), issues.end(),
        [=](const simdjson::dom::element &item)
        {
            if (!item.is_object())
            {
                throw std::runtime_error("Issue array element is not an object");
            }
            const simdjson::dom::object &obj = item.get_object().value();
            if (!obj.at_key("id").is_string())
            {
                std::ostringstream typeName;
                typeName << obj.at_key("id").type();
                throw std::runtime_error("Expected string value for key 'id', got " + typeName.str());
            }
            const int issueId = std::stoi(std::string{obj.at_key("id").get_string().value()});
            return issueId == issue;
        });
    if (it == issues.end())
    {
        throw std::runtime_error("Couldn't find issue with id " + std::to_string(issue));
    }
    return (*it).get_object().value();
}

} // namespace comics
