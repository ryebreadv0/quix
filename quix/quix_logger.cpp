#ifndef _QUIX_LOGGER_CPP
#define _QUIX_LOGGER_CPP

#include "quix_logger.hpp"

namespace quix
{

logger::logger(const char* name)
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    m_logger = std::make_shared<spdlog::logger>(name);
    m_logger->set_level(spdlog::level::info);

    spdlog::register_logger(m_logger);
}

logger::logger(const char* name, spdlog::level::level_enum level)
{
    // spdlog::set_pattern("%^[%T] %n: %v%$");
    m_logger = std::make_shared<spdlog::logger>(name);
    m_logger->set_level(level);

    spdlog::register_logger(m_logger);
}

std::shared_ptr<spdlog::logger>& logger::get_logger()
{
    return m_logger;
}


} // namespace quix

#endif // _QUIX_LOGGER_CPP