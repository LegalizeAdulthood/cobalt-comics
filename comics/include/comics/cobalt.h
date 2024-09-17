#pragma once

#include <comics/database.h>

#include <boost/cobalt/generator.hpp>

namespace comics
{
namespace cobalt
{

boost::cobalt::generator<SequenceMatch> matches(DatabasePtr database, CreditField creditField, std::string_view name);

} // namespace cobalt
} // namespace comics
