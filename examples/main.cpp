#include "quix_logger.hpp"

int main()
{
    quix::logger logger("log1");
    
    logger.add_sink(
        std::make_shared
        <spdlog::sinks::basic_file_sink_mt>(
        "logs/logger.log", true
    ));

    logger.info("Hello, World!");
    
    return 0;
}