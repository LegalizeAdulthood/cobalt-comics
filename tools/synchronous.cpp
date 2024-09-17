#include <iostream>
#include <stdexcept>

#include <comics/comics.h>

namespace
{

int usage(const char *program)
{
    std::cerr << "Usage: " << program
              << " <jsondir> (-s <script writer name>|-p <penciler name>|-i <inker name>|-c <colorist name>)\n";
    return 1;
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
        std::shared_ptr db{comics::createQuery(argv[1])};
        const std::string option{argv[2]};
        const std::string name{argv[3]};
        if (option == "-s")
        {
            db->printScriptSequences(std::cout, name);
        }
        else if (option == "-p")
        {
            db->printPencilSequences(std::cout, name);
        }
        else if (option == "-i")
        {
            db->printInkSequences(std::cout, name);
        }
        else if (option == "-c")
        {
            db->printColorSequences(std::cout, name);
        }
        else
        {
            return usage(argv[0]);
        }
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
