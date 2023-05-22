#ifndef _QUIX_LOGGER_HPP
#define _QUIX_LOGGER_HPP

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

    inline void set_pattern(const char* pattern) {
        m_logger->set_pattern(pattern);
    }

    inline void set_pattern() {
        m_logger->set_pattern("%^[%T] [%n:%l] %v%$");
    }

    inline void set_level(spdlog::level::level_enum level) {
        m_logger->set_level(level);
    }

    std::shared_ptr<spdlog::logger>& get_logger();

    auto& get_sink(uint32_t index) const {
        return m_logger->sinks()[index];
    }

    template <typename S>
    void add_sink(std::shared_ptr<S> sink) {
        m_logger->sinks().push_back(sink);
    }


    template <typename... Args>
    inline void trace(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->trace(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void debug(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->debug(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void info(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->info(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void warn(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->warn(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void error(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->error(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    inline void critical(spdlog::format_string_t<Args...> fmt, Args &&... args) const {
        m_logger->critical(fmt, std::forward<Args>(args)...);
    }



private:
    std::shared_ptr<spdlog::logger> m_logger;
    
};

    

} // namespace quix

#endif // _QUIX_LOGGER_HPP