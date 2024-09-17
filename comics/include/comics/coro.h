#pragma once

#include <comics/database.h>

#include <simdjson.h>

#include <coroutine>
#include <filesystem>
#include <memory>
#include <string_view>

namespace comics
{
namespace coroutine
{

class MatchGenerator
{
public:
    struct Promise
    {
        MatchGenerator get_return_object()
        {
            return MatchGenerator{Handle::from_promise(*this)};
        }
        std::suspend_always initial_suspend()
        {
            return {};
        }
        std::suspend_always yield_value(const SequenceMatch &value)
        {
            m_data = value;
            return {};
        }
        void return_void()
        {
        }
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }
        void unhandled_exception()
        {
            std::terminate();
        }

        mutable SequenceMatch m_data;
    };
    using promise_type = Promise;
    using Handle = std::coroutine_handle<promise_type>;

    ~MatchGenerator()
    {
        if (m_handle)
        {
            m_handle.destroy();
        }
    }
    MatchGenerator(const MatchGenerator &) = delete;
    MatchGenerator &operator=(const MatchGenerator &) = delete;

    bool resume()
    {
        if (!m_handle || m_handle.done())
        {
            return false;
        }
        m_handle.resume();
        return !m_handle.done();
    }

    SequenceMatch getMatch() const
    {
        const SequenceMatch match{m_handle.promise().m_data};
        m_handle.promise().m_data = SequenceMatch{};
        return match;
    }

private:
    MatchGenerator(Handle handle) :
        m_handle(handle)
    {
    }

    Handle m_handle;
};

MatchGenerator matches(DatabasePtr database, CreditField creditField, std::string_view name);

} // namespace coroutine
} // namespace comics
