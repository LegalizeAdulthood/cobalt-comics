#pragma once

#include <filesystem>
#include <memory>
#include <ostream>

namespace comics
{

class Query
{
public:
    virtual ~Query() = default;

    virtual void printScriptSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printPencilSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printInkSequences(std::ostream &str, const std::string &name) = 0;
    virtual void printColorSequences(std::ostream &str, const std::string &name) = 0;
};

std::shared_ptr<Query> createQuery(const std::filesystem::path &jsonDir);

} // namespace comics
