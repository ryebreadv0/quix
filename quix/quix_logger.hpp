#ifndef _QUIX_LOGGER_HPP
#define _QUIX_LOGGER_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <string_view>

namespace quix
{

class logger {
public:
    explicit logger(const char* name);
    explicit logger(const char* name, spdlog::level::level_enum level);

    ~logger() = default;
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;
    logger(logger&&) = delete;
    logger& operator=(logger&&) = delete;

    static void set_pattern(std::string_view pattern) {
        spdlog::set_pattern(pattern.data());
    }

    std::shared_ptr<spdlog::logger>& get_logger();

    auto& get_sink(uint32_t index) {
        return m_logger->sinks()[index];
    }

    template <typename S>
    void add_sink(std::shared_ptr<S> sink) {
        m_logger->sinks().push_back(sink);
    }

    template <typename... Args>
    void trace(Args&&... args) {
        m_logger->trace(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void debug(Args&&... args) {
        m_logger->debug(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void info(Args&&... args) {
        m_logger->info(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void warn(Args&&... args) {
        m_logger->warn(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void error(Args&&... args) {
        m_logger->error(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void critical(Args&&... args) {
        m_logger->critical(std::forward<Args>(args)...);
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
    
};

    

} // namespace quix

#endif // _QUIX_LOGGER_HPP