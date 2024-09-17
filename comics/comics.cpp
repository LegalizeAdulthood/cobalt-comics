#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <simdjson.h>

#include "comics/comics.h"

namespace comics
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length() - suffix.length()) == suffix;
}

namespace
{

void printSequence(std::ostream &str, const simdjson::dom::object &sequence)
{
    const auto printField = [&](const std::string_view key)
    {
        const auto it = std::find_if(sequence.begin(), sequence.end(),
            [=](const simdjson::dom::key_value_pair &item) { return item.key == key; });
        if (it == sequence.end())
        {
            // field might not be present
            return;
        }
        const simdjson::simdjson_result<simdjson::dom::element> field = sequence.at_key(key);
        if (key.length() > 18)
        {
            throw std::runtime_error("Field " + std::string{key} + " too long");
        }
        str << std::string(18 - key.length(), ' ') << key << ": ";
        if (field.is_string())
        {
            str << field.get_string().value() << '\n';
        }
        else if (field.is_bool())
        {
            str << (field.get_bool().value() ? "true\n" : "false\n");
        }
        else if (field.is_number())
        {
            str << field.get_int64() << '\n';
        }
        else
        {
            std::ostringstream fieldType;
            fieldType << field.type();
            throw std::runtime_error("Unknown type for field '" + std::string{key} + "', got " + fieldType.str());
        }
    };
    printField("title");
    printField("feature");
    printField("script");
    printField("pencils");
    printField("inks");
    printField("colors");
}

class JSONDatabase : public Database
{
public:
    JSONDatabase(const std::filesystem::path &jsonDir);
    void printScriptSequences(std::ostream &str, const std::string &name) override;
    void printPencilSequences(std::ostream &str, const std::string &name) override;
    void printInkSequences(std::ostream &str, const std::string &name) override;
    void printColorSequences(std::ostream &str, const std::string &name) override;

private:
    void printIssue( std::ostream& str, int id ) const;
    void printMatchingSequences(std::ostream &str, const std::string_view &fieldName, const std::string &name);

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

void JSONDatabase::printScriptSequences(std::ostream &str, const std::string &name)
{
    printMatchingSequences(str, "script", name);
}

void JSONDatabase::printPencilSequences(std::ostream &str, const std::string &name)
{
    printMatchingSequences(str, "pencils", name);
}

void JSONDatabase::printInkSequences(std::ostream &str, const std::string &name)
{
    printMatchingSequences(str, "inks", name);
}

void JSONDatabase::printColorSequences(std::ostream &str, const std::string &name)
{
    printMatchingSequences(str, "colors", name);
}

void JSONDatabase::printIssue(std::ostream &str, int id) const
{
    const auto it = std::find_if(m_issues.get_array().begin(), m_issues.get_array().end(),
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
            return issueId == id;
        });
    if (it == m_issues.get_array().end())
    {
        throw std::runtime_error("Couldn't find issue with id " + std::to_string(id));
    }
    const simdjson::dom::object issue = (*it).get_object().value();
    str << issue.at_key("series name").get_string().value() << " #" << issue.at_key("issue number").get_string().value() << '\n';
}

void JSONDatabase::printMatchingSequences(std::ostream &str, const std::string_view &fieldName, const std::string &name)
{
    std::map<int, std::vector<simdjson::dom::object>> issueSequences;

    for (const simdjson::dom::element record : m_sequences.get_array())
    {
        if (!record.is_object())
        {
            throw std::runtime_error("Sequence array element should be an object");
        }

        for (const simdjson::dom::key_value_pair field : record.get_object())
        {
            if (field.key == fieldName)
            {
                const simdjson::dom::element &value = field.value;
                if (!value.is_string())
                {
                    throw std::runtime_error("Value of script field should be a string");
                }
                if (value.get_string().value().find(name) != std::string::npos)
                {
                    simdjson::dom::object obj = record.get_object().value();
                    const int issue = std::stoi(std::string{obj.at_key("issue").get_string().value()});
                    issueSequences[issue].push_back(obj);
                }
                break;
            }
        }
    }

    int firstIssue{true};
    const auto sequence_number = [](const simdjson::dom::object &seq)
        { return std::stoi(std::string{seq.at_key("sequence_number").get_string().value()}); };
    for (const auto &[issueId, matches] : issueSequences)
    {
        if (!firstIssue)
        {
            str << '\n';
        }
        printIssue(str, issueId);

        std::vector sequences{matches};
        std::sort(sequences.begin(), sequences.end(),
            [&](const simdjson::dom::object &lhs, const simdjson::dom::object &rhs)
            { return sequence_number(lhs) < sequence_number(rhs); });
        bool first{true};
        for (const simdjson::dom::object &seq : sequences)
        {
            if (!first)
            {
                str << '\n';
            }
            printSequence(str, seq);
            first = false;
        }
        firstIssue = false;
    }
}

} // namespace

std::shared_ptr<Database> createDatabase(const std::filesystem::path &jsonDir)
{
    return std::make_shared<JSONDatabase>(jsonDir);
}

} // namespace comics
