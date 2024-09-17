#include "comics/cobalt.h"

#include <boost/cobalt/generator.hpp>

#include <map>

namespace comics
{
namespace cobalt
{
boost::cobalt::generator<SequenceMatch> matches(DatabasePtr database, CreditField creditField, std::string_view name)
{
    if (!database)
    {
        co_return SequenceMatch{};
    }

    std::map<int, simdjson::dom::object> issues;
    std::string_view fieldName{to_string(creditField)};

    for (const simdjson::dom::element record : database->getSequences().get_array())
    {
        if (!record.is_object())
        {
            throw std::runtime_error("Sequence array element should be an object");
        }

        const simdjson::dom::object sequence{record.get_object()};
        for (const simdjson::dom::key_value_pair field : sequence)
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
                    if (auto it = issues.find(issue); it == issues.end())
                    {
                        issues[issue] = find_issue(database, issue);
                    }
                    co_yield SequenceMatch{issues[issue], sequence};
                }
                break;
            }
        }
    }
}

} // namespace cobalt
} // namespace comics
