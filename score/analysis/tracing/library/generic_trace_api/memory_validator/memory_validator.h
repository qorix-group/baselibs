#ifndef SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_MEMORY_VALIDATOR_H
#define SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_MEMORY_VALIDATOR_H

#if defined __QNX__
#include "score/analysis/tracing/library/generic_trace_api/memory_validator/qnx/qnx_memory_validator.h"
#else
#include "score/analysis/tracing/library/generic_trace_api/memory_validator/linux/linux_memory_validator.h"
#endif

#endif  // SCORE_ANALYSIS_TRACING_LIB_GENERIC_TRACE_API_MEMORY_VALIDATOR_H
