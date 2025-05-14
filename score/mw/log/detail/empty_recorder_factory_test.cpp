

#include "score/mw/log/detail/empty_recorder_factory.h"
#include "gtest/gtest.h"

#include <type_traits>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{
template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

TEST(EmptyRecorderFactory, TestCreateEmptyRecorder)
{
    const Configuration config;
    auto recorder = EmptyRecorderFactory{}.CreateLogRecorder(config, nullptr);
    ASSERT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
