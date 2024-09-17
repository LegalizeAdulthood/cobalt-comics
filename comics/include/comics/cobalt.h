#pragma once

#include <comics/database.h>

#include <boost/cobalt/generator.hpp>

#include <optional>

namespace comics
{
namespace cobalt
{

using SearchResult = std::optional<SequenceMatch>;

boost::cobalt::generator<SearchResult> matches(DatabasePtr database, CreditField creditField, std::string_view name);

} // namespace cobalt
} // namespace comics
