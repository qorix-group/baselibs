# Null-Termination Check for string-view like types

`std::string_view` does not give any guarantee w.r.t. its underlying character buffer to be null-terminated.
Same applies for `score::cpp::span` as well as `std::span`! As a result, providing a pointer to the underlying
buffer (e.g. via `string_view.data()`) to another API which expects the underlying character buffer to
be null-terminated (e.g. legacy C-APIs or `std::string`'s constructor), can easily result in undefined
behavior. Furthermore, programs could also easily produce unpredictable results in case the
null-termination occurs at a location which is beyond the view's range (see examples below).

Due to lack of a guaranteed null-terminated `string_view` type in C++'s stdlib, we provide a common
utility here which shall be used by any code requiring a null-termination guarantee in conjunction
with a `string_view`'s underlying buffer. This utility also works for `string_view`-like types
such as `score::cpp::span` and `std::span` as well as similar types from `ara::core`.

**NOTE:** In general, developers are requested to migrate their code (if possible) to make use of
`safecpp`'s types `zstring_view` or `zspan` since their underlying sequences are guaranteed to be
null-terminated.
Once C++'s stdlib or any other custom library (e.g. `amp`) offers null-terminated `span` and/or
`string_view` type(s), the respective types within the `safecpp` namespace could then get
replaced by aliases to these standard ones (if compatible).
