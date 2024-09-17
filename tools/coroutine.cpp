#include <comics/coro.h>
#include <comics/database.h>

#include <iostream>
#include <stdexcept>
#include <string>

namespace
{

int usage(const char *program)
{
    std::cerr << "Usage: " << program
              << " <jsondir> (-s <script writer name>|-p <penciler name>|-i <inker name>|-c <colorist name>)\n";
    return 1;
}

std::string issueTitle(simdjson::dom::object issue)
{
    return std::string{issue.at_key("series name").get_string().value()} + " #" +
        std::string{issue.at_key("issue number").get_string().value()};
}

void printSequence(std::ostream &str, simdjson::dom::object sequence)
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

void printMatch(std::ostream &str, comics::SequenceMatch match)
{
    static std::string lastTitle{};

    if (const std::string title{issueTitle(match.issue)}; lastTitle != title)
    {
        str << title << '\n';
        lastTitle = title;
    }
    printSequence(str, match.sequence);
}

void printMatches(std::ostream &str, comics::DatabasePtr db, comics::CreditField field, std::string_view name)
{
    comics::coroutine::MatchGenerator coro{comics::coroutine::matches(db, field, name)};
    bool first{true};
    while (coro.resume())
    {
        if (!first)
        {
            str << '\n';
        }
        printMatch(str, coro.getMatch());
        first = false;
    }
}

} // namespace

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        return usage(argv[0]);
    }
    try
    {
        std::shared_ptr db{::comics::createDatabase(argv[1])};
        const std::string_view option{argv[2]};
        const std::string_view name{argv[3]};
        comics::CreditField field{comics::CreditField::NONE};
        if (option == "-s")
        {
            field = comics::CreditField::SCRIPT;
        }
        else if (option == "-p")
        {
            field = comics::CreditField::PENCIL;
        }
        else if (option == "-i")
        {
            field = comics::CreditField::INK;
        }
        else if (option == "-c")
        {
            field = comics::CreditField::COLOR;
        }
        else
        {
            return usage(argv[0]);
        }
        printMatches(std::cout, db, field, name);
    }
    catch (const std::exception &bang)
    {
        std::cerr << "Unexpected exception: " << bang.what() << '\n';
        return 2;
    }
    catch (...)
    {
        return 3;
    }

    return 0;
}
