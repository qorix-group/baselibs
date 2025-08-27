///
/// @file register_test_app.cpp
/// @copyright Copyright (C) 2025, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief
///

#include "score/analysis/tracing/library/interface/generic_trace_api.h"
#include "score/mw/log/logging.h"

#include <chrono>
#include <thread>

using namespace score::analysis::tracing;

/* Pass to the app argument with client name. E.g. ./register_test_app id1 */
int main(int argc, char* argv[])
{
    std::cout << "Register test app is running" << std::endl;

    if (argc != 3)
    {
        std::cout << "There should be exactly two params with client names!" << std::endl;
        return -1;
    }

    auto register_client_result1 = GenericTraceAPI::RegisterClient(BindingType::kFirst, argv[1]);
    if (!register_client_result1.has_value())
    {
        std::cout << "Failed to register client with name: " << argv[1] << std::endl;
        std::cout << "Error: " << register_client_result1.error() << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Registered with client_id: " << static_cast<std::uint16_t>(register_client_result1.value())
                  << std::endl;
    }

    auto register_client_result1_same_name = GenericTraceAPI::RegisterClient(BindingType::kFirst, argv[1]);
    if (!register_client_result1_same_name.has_value())
    {
        std::cout << "Failed to reregister client with name: " << argv[1] << std::endl;
        std::cout << "Error: " << register_client_result1_same_name.error() << std::endl;
        return -1;
    }
    if (register_client_result1.value() != register_client_result1_same_name.value())
    {
        std::cout << "Failed to reregister (got different client_id) client with name: " << argv[1] << std::endl;
        std::cout << "Error: " << register_client_result1.error() << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Reregistered succesfull with client_id: "
                  << static_cast<std::uint16_t>(register_client_result1.value()) << std::endl;
    }

    auto register_client_result2 = GenericTraceAPI::RegisterClient(BindingType::kFirst, argv[2]);
    if (!register_client_result2.has_value())
    {
        std::cout << "Failed to register client with name: " << argv[2] << std::endl;
        std::cout << "Error: " << register_client_result2.error() << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Registered with client_id: " << static_cast<std::uint16_t>(register_client_result2.value())
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
