#pragma once
#ifndef _MIRROR_ERROR_HANDLER_
#define _MIRROR_ERROR_HANDLER_

#include <iostream>
#include <stdexcept>
#include <functional>

namespace mirror
{

void __runtime_error_handler(std::exception e)
{
    std::cout << e.what() << std::endl;
}

static std::function<void(std::exception)> runtime_error_handler = __runtime_error_handler;

} // namespace mirror
#endif