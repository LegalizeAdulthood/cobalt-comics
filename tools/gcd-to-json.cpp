#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace tool
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length() - suffix.length()) == suffix;
}

inline std::string escaped(std::string text)
{
    // escape all backslashes
    for (std::string::size_type pos = text.find('\\'); pos != std::string::npos; pos = text.find('\\', pos + 2))
    {
        text.insert(pos, 1, '\\');
    }

    // replace "" with \"
    for (std::string::size_type pos = text.find(R"("")"); pos != std::string::npos; pos = text.find(R"("")", pos + 2))
    {
        text[pos] = '\\';
    }

    // escape all TABs
    for (std::string::size_type pos = text.find('\t'); pos != std::string::npos; pos = text.find('\t', pos + 1))
    {
        text.erase(pos, 1);
        text.insert(pos, "\\t");
    }

    // scan for control characters
    std::string::size_type pos = 0;
    while (pos < text.length())
    {
        const unsigned char c = static_cast<unsigned char>(text[pos]);
        if (c < 32)
        {
            std::cerr << "\nControl character (^" << static_cast<char>(c + 64) << ") found in '" << text << "'; dropping\n"
                << "                                 " << std::string(pos, ' ') << "^--\n";
            text.erase(pos, 1);
        }
        else
        {
            ++pos;
        }
    }
    return text;
}

void split(std::vector<std::string> &fields, const std::string &text)
{
    fields.clear();
    std::size_t start{1};
    for (std::size_t split = text.find("\"\t\""); split != std::string::npos; split = text.find("\"\t\"", split + 1))
    {
        fields.emplace_back(text.substr(start, split - start));
        start = split + 3;
    }
    fields.emplace_back(text.substr(start, text.length() - start - 1));
}

void convertIssues( const fs::path &path, bool singleLineRecords )
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert issues at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
    int recordCount{};
    std::string line;
    std::map<std::string, std::string> record;
    int lastRecordId{-1};
    bool firstRecord{true};
    auto printRecord = [&]
    {
        if (record.empty())
            return;
        if (!firstRecord)
            json << ",\n";
        json << "{";
        bool first{true};
        for (const auto &pair : record)
        {
            if (!first)
            {
                json << ",";
            }
            json << (singleLineRecords ? " \"" : "\n    \"") << escaped(pair.first) << "\": ";
            if (pair.second == "True" || pair.second == "False")
            {
                json << (pair.second == "True" ? "true" : "false");
            }
            else
            {
                json << "\"" << escaped(pair.second) << "\"";
            }
            first = false;
        }
        json << (singleLineRecords ? "}" : "\n}");
        firstRecord = false;
    };
    json << "[\n";
    while (tsv)
    {
        if (!std::getline(tsv, line))
        {
            break;
        }
        ++recordCount;
        if (recordCount % 1000 == 0)
        {
            std::cout << recordCount << " records...\r";
        }

        std::vector<std::string> fields;
        split(fields, line);
        if (fields.size() != 3)
        {
            throw std::runtime_error("Expected 3 fields, got " + std::to_string(fields.size()));
        }
        const int recordId{std::stoi(fields[0])};
        if (recordId != lastRecordId)
        {
            if (!record.empty())
            {
                printRecord();
            }
            record.clear();
            record["id"] = fields[0];
            lastRecordId = recordId;
        }
        record[fields[1]] = fields[2];
    }
    printRecord();
    json << "\n]\n";
    std::cout << '\n' << recordCount << " records processed.\n";
}

void convertSequences( const fs::path &path, bool singleLineRecords )
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert sequences at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
    int recordCount{};
    std::string line;
    std::map<std::string, std::string> record;
    int lastRecordId{-1};
    int lastSequenceId{-1};
    bool firstRecord{true};
    auto printRecord = [&]
    {
        if (record.empty())
            return;
        if (!firstRecord)
            json << ",\n";
        json << "{";
        bool first{true};
        for (const auto &pair : record)
        {
            if (!first)
            {
                json << ",";
            }
            json << (singleLineRecords ? " \"" : "\n    \"") << escaped(pair.first) << "\": ";
            if (pair.second == "True" || pair.second == "False")
            {
                json << (pair.second == "True" ? "true" : "false");
            }
            else
            {
                json << "\"" << escaped(pair.second) << "\"";
            }
            first = false;
        }
        json << (singleLineRecords ? "}" : "\n}");
        firstRecord = false;
    };
    json << "[\n";
    while (tsv)
    {
        if (!std::getline(tsv, line))
        {
            break;
        }
        ++recordCount;
        if (recordCount % 1000 == 0)
        {
            std::cout << recordCount << " records...\r";
        }

        std::vector<std::string> fields;
        split(fields, line);
        if (fields.size() != 4)
        {
            throw std::runtime_error("Expected 4 fields, got " + std::to_string(fields.size()));
        }
        const int recordId{std::stoi(fields[0])};
        const int sequenceId{std::stoi(fields[1])};
        if (recordId != lastRecordId || sequenceId != lastSequenceId)
        {
            printRecord();
            record.clear();
            record["issue"] = fields[0];
            lastRecordId = recordId;
            lastSequenceId = sequenceId;
        }
        record[fields[2]] = fields[3];
    }
    printRecord();
    json << "\n]\n";
    std::cout << '\n' << recordCount << " records processed.\n";
}

void gcdToJSON( const std::string &dataDir, bool singleLineRecords )
{
    for (const fs::directory_entry &entry : fs::directory_iterator(dataDir))
    {
        const fs::path &path = entry.path();
        if (!(entry.is_regular_file() && path.extension().string() == ".tsv"))
        {
            continue;
        }

        if (endsWith(path.stem().string(), "issues"))
        {
            convertIssues(path, singleLineRecords);
        }
        else if (endsWith(path.stem().string(), "sequences"))
        {
            convertSequences(path, singleLineRecords);
        }
    }
}

} // namespace tool

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3 || (argc == 3 && argv[1] != std::string{"-s"}))
    {
        std::cerr << "Usage: gcd-to-json [-s] <datadir>\n";
        return 1;
    }
    const bool singleLineRecords{argc == 3};

    const std::string dataDir{argv[singleLineRecords ? 2 : 1]};
    try
    {
        tool::gcdToJSON(dataDir, singleLineRecords);
    }
    catch (const std::exception &bang)
    {
        std::cerr << "\nUnexpected exception: " << bang.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "\nUnexpected exception\n";
        return 2;
    }
    return 0;
}
