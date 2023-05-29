#ifndef _QUIX_COMMAND_LIST_CPP
#define _QUIX_COMMAND_LIST_CPP

#include "quix_command_list.hpp"

#include "quix_device.hpp"

namespace quix {

command_pool::command_pool(std::shared_ptr<device> s_device, VkCommandPool pool)
    : m_device(s_device)
    , pool(pool)
{
}

command_pool::~command_pool()
{
    m_device->return_command_pool(pool);
}


} // namespace quix

#endif // _QUIX_COMMAND_LIST_CPP