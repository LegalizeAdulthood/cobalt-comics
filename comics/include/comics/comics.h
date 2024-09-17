#pragma once

#include <filesystem>
#include <memory>
#include <ostream>

namespace comics
{

class Database
{
public:
    virtual ~Database() = default;

    virtual void printScriptSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printPencilSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printInkSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printColorSequences(std::ostream &str, const std::string &name) = 0;
};

std::shared_ptr<Database> createDatabase(const std::filesystem::path &jsonDir);

} // namespace comics
