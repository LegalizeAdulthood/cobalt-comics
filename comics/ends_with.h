#include <string>

namespace comics
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length() - suffix.length()) == suffix;
}

} // namespace comics
