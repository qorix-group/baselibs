#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_H

#include "score/analysis/tracing/library/generic_trace_api/daemon_communicator/daemon_communicator_factory_interface.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class DaemonCommunicatorFactory : public IDaemonCommunicatorFactory
{
  public:
    DaemonCommunicatorFactory() = default;

    /// @brief Method to create DaemonCommunicator instance.
    ///
    /// @return Unique pointer to IDaemonCommunicator.
    // This is intented, default argument is needed here to ensure testability
    // NOLINTBEGIN(google-default-arguments) see comment above
    std::unique_ptr<IDaemonCommunicator> CreateDaemonCommunicator(
        // This is false positive, as the default arguments are the same.
        // coverity[autosar_cpp14_m8_3_1_violation]
        std::unique_ptr<IDaemonCommunicator> daemon_communicator = nullptr) const override;
    // NOLINTEND(google-default-arguments) see comment above
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_DAEMON_COMMUNICATOR_FACTORY_H
