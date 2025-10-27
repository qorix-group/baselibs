
# Custom Recorder Support

This document describes the custom recorder extension that allows external integrators to route logs to non-S-CORE backends, providing an alternative to the default S-CORE provided recorders.

The custom recorder feature enables developers to implement their own logging backends by providing a standardized interface for custom log handling. This extension point allows for flexible log routing while maintaining compatibility with the existing logging framework.

## Implementation

A user who wants to use the custom recorder, has to provide implementation of "CustomRecorder" and "CustomRecorderFactory" classes. And make changes accordingly in `score/mw/log/detail/recorder_config.h`

### custom recorder classes

- `CustomRecorder` - The main recorder implementation
- `CustomRecorderFactory` - Factory class for creating recorder instance

### feature configuration change

Update `score/mw/log/detail/recorder_config.h` to include your custom implementation:

```c++
#if defined(KCUSTOM_LOGGING)
#include "score/mw/log/custom_recorder_example/custom_recorder.h"
#include "score/mw/log/custom_recorder_example/custom_recorder_factory.h"
#endif

#if defined(KCUSTOM_LOGGING)
using CustomRecorderType = CustomRecorder;
using CustomRecorderFactoryType = CustomRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using CustomRecorderType = EmptyRecorder;
using CustomRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif
```

## Build with Custom Recorder Implementation

The label flag `custom_recorder` is defined in "score/mw/log/flags/BUILD". To link with custom implementation, the user should inject the library with build label.

```bash
bazel build --config=<platform_config> --//score/mw/log/flags:KCustom_Logging=True --//score/mw/log/flags:custom_recorder=//score/mw/log/custom_recorder_example <build_target>
```

Therein,

- `--//score/mw/log/flags:KCustom_Logging=True` - feature flag, enable custom logging feature, False by default and recorder will fallback to `EmptyRecorder`
- `--//score/mw/log/flags:custom_recorder=<impl_target>` - label flag, path to your implementation target

## Configuration

To route the log messages to custom recorder, the application specific configuration file `logging.json` with "logMode" = "kCustom" should provided. For example:

```json
{
    "appId": "TEST",
    "appDesc": "test custom recorder",
    "logMode": "kCustom",
    "logLevel": "kVerbose",
}
```

## Example

A reference implementation is available in "score/mw/log/custom_recorder_example".
