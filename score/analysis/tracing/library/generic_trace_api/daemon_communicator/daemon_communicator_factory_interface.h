#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_INTERFACE_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_INTERFACE_H

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/i_daemon_communicator.h"
#include "score/result/result.h"
namespace score
{
namespace analysis
{
namespace tracing
{

class IDaemonCommunicatorFactory
{
  public:
    /// @brief IDaemonCommunicatorFactory default constructor.
    IDaemonCommunicatorFactory() noexcept = default;

    /// @brief IDaemonCommunicatorFactory deleted copy constructor.
    IDaemonCommunicatorFactory(const IDaemonCommunicatorFactory&) noexcept = delete;

    /// @brief IDaemonCommunicatorFactory deleted move constructor.
    IDaemonCommunicatorFactory(IDaemonCommunicatorFactory&&) noexcept = delete;

    /// @brief IDaemonCommunicatorFactory default destructor.
    virtual ~IDaemonCommunicatorFactory() = default;

    /// @brief IDaemonCommunicatorFactory deleted copy assignement operator.
    IDaemonCommunicatorFactory& operator=(const IDaemonCommunicatorFactory&) = delete;

    /// @brief IPlugin deleted move assignement operator.
    IDaemonCommunicatorFactory& operator=(IDaemonCommunicatorFactory&&) = delete;

    /// @brief Method to create DaemonCommunicator instance.
    ///
    /// @return Unique pointer to IDaemonCommunicator.
    // This is intented, default argument is needed here to ensure testability
    // NOLINTBEGIN(google-default-arguments) see comment above
    virtual std::unique_ptr<IDaemonCommunicator> CreateDaemonCommunicator(
        std::unique_ptr<IDaemonCommunicator> daemon_communicator = nullptr) const = 0;
    // NOLINTEND(google-default-arguments) see comment above
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_INTERFACE_H
