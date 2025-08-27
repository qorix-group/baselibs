# Detailed Design Generic Trace Library


| Field | Value |
|:-------------------|:--------------:|
| Version | 2.0 |
| Status  | DRAFT |
| Maintainer | JC-30 |


## Table of Contents

- [Goal](#goal)
- [Architectural Assumptions](#architectural-assumptions)
  - [IPC tracing backend](#ipc-tracing-backend)
  - [Generic Trace API as library per process](#generic-trace-api-as-library-per-process)
  - [All Data Preprocessing done by upper layers](#all-data-preprocessing-done-by-upper-layers)
- [Requirements and Safety](#requirements-and-safety)
  - [Requirements](#requirements)
  - [Safety](#safety)
- [External Interfaces](#external-interfaces)
- [Static description of the library](#static-description-of-the-library)
  - [C++ API interface](#c-api-interface)
    - [Registration of trace clients](#registration-of-trace-clients)
    - [(De)Registration of shared-memory objects](#deregistration-of-shared-memory-objects)
    - [Tracing the IPC data forwarded by the users](#tracing-the-ipc-data-forwarded-by-the-users)
      - [Meta-Info concepts](#meta-info-concepts)
        - [Current restriction](#current-restriction)
      - [Data/content representation](#datacontent-representation)
      - [Support for Tracing of data not yet residing in shared memory](#support-for-tracing-of-data-not-yet-residing-in-shared-memory)
      - [Timestamp Acquisition](#timestamp-acquisition)
      - [Loss of trace data](#loss-of-trace-data)
    - [Registering a callback to be get on the completion of a trace job](#registering-a-callback-to-be-get-on-the-completion-of-a-trace-job)
  - [Static class diagram](#static-class-diagram)
  - [Error codes](#error-codes)
  - [Library dependencies](#library-dependencies)
    - [Dependency to BMW `amp` library](#dependency-to-score-amp-library)
    - [Dependency to `lib/memory/shared`](#dependency-to-libmemoryshared)
    - [Dependency to `mw::time`](#dependency-to-mwtime)
    - [Dependency to `lib/os`](#dependency-to-libos)
    - [Dependency to `lib/result`](#dependency-to-libresult)
    - [Dependency to `language/safecpp/scoped_function`](#dependency-to-languagesafecppscoped_function)
    - [Dependency to `lib/concurrency`](#dependency-to-libconcurrency)
- [Dynamic Architecture](#dynamic-architecture)
  - [Library usage sequence](#library-usage-sequence)
  - [Special considerations due to asynchronous behavior](#special-considerations-due-to-asynchronous-behavior)
  - [Library startup sequence](#library-startup-sequence)
  - [Library state machine](#library-state-machine)
  - [Worker thread sequence](#worker-thread-sequence)
    - [Startup Sequence](#startup-sequence)
    - [Background sequence](#background-sequence)
    - [Shutdown sequence](#shutdown-sequence)
  - [Internal communication between trace library and IPC trace backend](#internal-communication-between-trace-library-and-ipc-trace-backend)
    - [Using QNX message passing to communicate with the backend](#using-qnx-message-passing-to-communicate-with-the-backend)
    - [IPC Tracing Backend termination/crash detection](#ipc-tracing-backend-terminationcrash-detection)
    - [Ring buffer](#ring-buffer)
  - [Tracing sequence](#tracing-sequence)
    - [Main sequence](#main-sequence)
    - [Saving the ChunkLists to the TMD area](#saving-the-chunklists-to-the-tmd-area)
- [Log/Trace-Points for ara::com and mw::com](#logtrace-points-for-aracom-and-mwcom)
- [Variability](#variability)
- [Design rationales](#design-rationales)
  - [Mathematics](#mathematics)
  - [Bit-fields](#bit-fields)
  - [Exceptions](#exceptions)
  - [Concurrency](#concurrency)
  - [Assembly language](#assembly-language)
  - [Algorithm](#algorithm)
- [Testing facilities](#testing-facilities)

## Goal

The goal of the `Generic Trace API` is to provide a generic high-performance interface to trace `ara::com` communication[^1].
It serves as the front-end of the [IPC Tracing framework](../../../../docs/features/logging/ipc-log) which uses DMA based communication[^2] to send data over the wire.

A massive amount of data traffic is expected to go through the `ara::com`/middleware layer. Hence, the `Generic Trace API` provides a (almost) zero copy, context switch free mechanism to trace the `ara::com` data being exchanged by the users.
The concept relies on the use of a direct-memory access (DMA) based driver. The data is stored in shared memory and passed around by reference between the different layers until it reaches the network driver which programs the DMA engine to perform the copy to send the data. Saving the CPU from performing the most resource heavy operation.

The `ara::com` messages (more specifically: event/field payload or service-method calls/returns) are be traced implicitly.
This means the user/app developer does not need to explicetly trace the data by calling the `Generic Trace API` but only needs to enable tracing in the chosen `ara::com` framework tracing [configuration file](../../config/schema/comtrace_config_schema.json).

[^1]: Although the library has been designed having in mind the needs of an `ara::com` some design decision have been made to also fit use cases of further upper layers. This use cases have, however, not be properly considered until now and will be left out of this document.

[^2]: The API is only referred to as `Generic Trace API` since the DMA ability depends on DMA capable HW being
present and from the user perspective the underlying 'technology' is irrelevant and transparent.


## Architectural Assumptions

The `Generic Trace API`, which is described here, is built upon the following
assumptions, boundary conditions and requirements:
- Traces are the `ara::com` messages as defined above.
- The traces are provided to the `Generic Trace API` from any so-called upper layer which refers to any used IPC framework (e.g `mw::com/LoLa`, `VectorCopy`, etc). Multiple (differing) upper layers may provided traces to the `Generic Trace API` simultaneously.
- There is one single/central IPC tracing backend (also referred to as "backend" in the following sections), which receives traces from the `Generic Trace API`. This backend outputs the content efficiently to the network.
- The backend is an `ara::com` specific code within a (single) central DMA Log and Trace Daemon process
  called Log and Trace Plugin Manager ([LTPM](../../daemon/design/README.md)). The LTPM is responsible for setting up the configuration of the DMA driver and deploying the different backends, refered to as **plugins**. The `ara::com` specific plugn is called `IPC Trace Plugin`.
- Wherever possible, processing burden shall be offloaded from the IPC tracing backend
  and shifted to the upper layers, so that CPU resources needed
  for DMA tracing are accounted to the users if possible.

These are detailed in upcoming sub-chapters.

### IPC tracing backend

The IPC tracing backend runs as a plugin (called `IPC Trace Plugin`) inside the LTPM which:

- `mmaps` shared-memory objects from various application processes containing
  DMA IPC trace users of `ara::com` (`LoLa` or AUTOSAR stack
  vendor), ...
- manages a dedicated portion of the shared-memory for exchange with the library
  instances.
- uses the network-interfaces configured by the LTPM and outputs "trace-data"
  given to it "efficiently" to the network.
- interacts with the user through the `Generic Trace API`
  library (see next chapter). This component is developed in close cooperation
  with the IPC tracing backend to maximize efficiency in the 'communication' between the
  backend and the library. This communication included - but is not limited to -
  'forwarded' trace-calls from `Generic Trace API` library to the backend and
  asynchronous feedback from the backend that data has been traced.
- will try to do network interface (low-level) optimizations as:
  - combining multiple "upper layer" trace calls into one ether frame (MTU)
  - fragmenting large trace payload from the "upper layer" into multiple (PLP)
    frames.

### Generic Trace API as library per process

All upper layers within application processes (see previous chapter) use the
same `Generic Trace API` described here, which will be implemented as a library
and cares for the communication with the IPC tracing backend. This library
is loaded once per application process, **independent** of the number of upper
layers residing within the concrete application process! I.e.:
If an application process is using an `ara::com` IPC based on `LoLa` and
some `ara::com` IPC based on a stack vendor product, there are already 2
different upper layers within this application process accessing the same
`Generic Trace API` library instance.

All public APIs the `Generic Trace API` library instance provides are
`static`/`class` methods.

### All Data Preprocessing done by upper layers

We want to offload processing from the IPC tracing backend as much
as possible. The main reasons are:

- such a daemon typically runs with low priority in the background. In this case
  the likelihood of data losses would be high! So we have to minimize its
  processing load.
- Applications, which want to trace, shall "pay for it" with their own
  CPU-budget!

This has the following impact: The upper layers, which use the Trace-API have to
hand over their data to be traced/outputted to network in its final network
representation form! That means tasks like the following should be done as much as possible by the
upper layers:

- Composition of protocol headers
- Bundling of data into "frames", which exploit ethernet frames efficiently.
  This includes also logic to abort the bundling in cases the latency for
  outputting would become too large.
- Segmenting of large data.

However for `ara::com`/IPC tracing[^3] this is not straight-forward as the users do not have an
expectation/notion of a concrete network representation of their output! So in
those cases it is completely acceptable, that they are handing over their trace
data "as is" to `Generic Trace API` and that the assembly into a network package
(adding a header, doing truncation or bundling into e.g. eth-frames) happens at
a later stage:

- either within the implementation of the `Generic Trace API` (in this case the
  processing load will still be accountable to the user/upper layer)
- or within the central daemon, in the respective plugin (in the `ara::com` case this would be the `IPC Trace Plugin`)

Currently the second approach has been favored as it simplifies the code on the library side.

[^3]:For `DLT` this is more natural/straight-forward as they are
providing already a specific network protocol specification!


## Requirements and Safety

### Requirements

The library was designed based on the
- platform requirements for IPC tracing concept: [DMA Network Tracing of IPC and Debug Data](broken_link_c/tracker/566329?view_id=-2&subtreeRoot=14532146)
- platform safety requirement for IPC tracing data integrity : [Intra-SoC IPC Tracing Integrity](broken_link_c/issue/25504440)

The generic trace library requirements can be found in the [Generic Trace Library](broken_link_c/issue/32211127) folder in codebeamer.

Of all the requirements, only two are safety relevant (tagged `ASIL B`):
- [IPC Data shall be shared as READ ONLY](broken_link_c/issue/32733341) : see [here](#deregistration-of-shared-memory-objects) for more details on how the handle is generated
- [IPC Data integrity](broken_link_c/issue/33102209) : see [here](#tracing-sequence) for more details on the trace sequence.

### Safety

The `Generic Trace API` is an `FFI` library who interacts with a `QM` software component, the IPC tracing backend.
As such it must guarantee that any information it receives from this `QM` component cannot be trusted to perform any `ASIL B` logic or to avoid spatial or temporal interference within its execution context.
To ensure that freedom of interference is guaranteed within the library an analysis was conducted on the impact of a wrong or incorrect value returned by the backend for all communication channels.
In any case, since the correctness of the returns of the `Generic Trace API` depend on the responses from a `QM` component the user shall not rely on the library returns to perform any `ASIL B` logic/feature.

In the current design, there are only 2 communication channels with the IPC tracing backend :
1. The QNX message passing server described [here](#using-qnx-message-passing-to-communicate-with-the-backend)
2. The trace job processing queue implemented as a ring buffer described [here](#ring-buffer)

The table below shows the expected behavior by the GTL for the different fault scenarios:

| Communication Channel  | Backend times out or does not reply | Backend replies with an error when it should have been a success | Backend replies with a valid incorrect value | Backend replies with an incorrect value |
| -------- | ------- | ------- | ------- | ------- |
| Name_open `daemon_communication` channel offered by IPC tracing backend  | N/A (Backend is not involved) | If the backend fails to create the server unexpectedly, only impact is tracing will not work. No calls to the IPC backend will be made at all during runtime.  | If the IPC tracing backend creates the server with a different name that the one expected by the library only impact is tracing will not work as the library will not be able to find the server. The library will only `name_open` the `daemon_communication` server and this name is unique in the codebase therefore no application can masquerade as the IPC tracing backend. GTL cannot open another server than the one expected. |Not possible, if `name_open` succeeds then the opened channel is valid.  |
| QNX message for PID request  | Done in a separate thread. No impact on user. Thread will be blocked until a response comes. State of the library will be kNotInitialized and there will no calls to the IPC backend.  | Done in a separate thread. Thread is unblocked and returns an error, the library will retry after 300ms. Library stays in kNotInitialized state. only impact is the user cannot trace.  | Library moves to kDaemonInitialized state. PID is used to create `shm_handles`, this means the handle is created for an unknown application. However the handle is `READ_ONLY` as per `ASIL B` requirements spatial FFI is respected. The backend will not be able to open the handle later and will result in no tracing. | Library moves to kDaemonInitialized state. PID is used to create `shm_handles`, this fails with `ESRCH` error code as per [documentation](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_create_handle.html). Regardless of the type of error the library will return the error to the user or at a later point during `Trace()`. |
| QNX message for client registration  |  Done in either separate thread or synchronously from an API call. After 500ms a timeout will unblock the thread and an error will be returned to the user. If the backend tries to reply to a message that timed out, the OS will reject the reply as the rcvid of the reply is no longer valid, so only the second reply will be accepted (see this [section](#using-qnx-message-passing-to-communicate-with-the-backend)) | Client registration fails, tracing is not possible for this client. All calls into the API are not possible as they need this `TraceClientID`.  | The client is registered but the `TraceClientID` returned is for another registered user. From user perspective nothing changes. However the `TraceClientID` is used by the backend to identify the memory locally allocated for the different trace jobs. This means that the backend could mark the wrong memory area for reuse. However, as memory model is static on the backend side, only impact is that garbage data is sent as pools are being rewritten at the same time they are being sent. This local memory is separate from the memory area where IPC is located and as IPC memory is maped as `READ_ONLY` by the backend, the MMU will prevent the backend from writing to the IPC memory as a result of an out of bounds access to local memory.  | This scenario is not possible. As `TraceClientID` is a `uint8` and all values are accepted. |
| QNX message for shm object registration  | Done in either separate thread or synchronously from an API call. Same behavior as for client registration.   | Shm object registration fails and error is returned to the user. Error is either immediately returned to the user or at a later point during `Trace()`. Impact is no tracing for that object is possible.  | Sucessful return from the backend but the shm object was not registered. From `GTL` perspective it is not possible to know this information therefore user will be able to call `Trace()` and jobs will be added to the queue. However they will be rejected at the backend level as it will be unable to access the memory. Only impact is tracing will not work and the user will not be informed via error return.  | This scenario is not possible. As return value `ShmHandle` is a `uint32` and all values are accepted.|
| QNX message for shm object deregistration  | Done in either separate thread or synchronously from an API call. Same behavior as for client registration. | Shm object deregistration fails and error is returned to the user. Error is either immediately returned to the user or at a later point during `Trace()`. No impact on the user, errors are ignored. | Backend replies that the memory has been unregistered but actually it is not and it is kept mmapped inside the bookkeeping structures of backend. There are two usecases: If the typed memory was no longer consumed by any proxy then on re offering the service a new memory area will be allocated by the skeleton. For typed memory, the typed memory daemon always calls `shm_unlink` before creating a new memory area with exclusive creation flags which means a different, independent memory area will be created and remapped by the backend on registration. The only impact here is some memory waste. If the shared memory is still being consumed by some proxy then upon re-offering, the skeleton will not recreate the memory but just open the old one. On re-registration of the same memory, the IPC backend will re-mmap it, assign a different associated virtual address to it and return a different `shm_handle` to the requesting process. Only impact is that there will be two page table entries for the same physical memory. In both cases, there is no functional or safety impact. | This scenario is not possible. Return is of type `ResultBlank`. |
| Trace job processing queue (ring buffer) | The backend is somehow blocked and cannot trace anymore. Acknowledgment callbacks will not be triggered. The users need to prioritze internal `ASIL B` logic over `QM` tracing[^4].    | If backend fails to packetize and then fails to update the processing queue, then there will no tracing as ring buffer will be in an incorrect state. An error will be returned by the `GTL` to the user indicating that the queue is full. | If backend marks a packet as sent even if it was not sent, only impact is one IPC trace will be missing. No impact on the GTL. | If backend marks a packet with an invalid status then ring buffer will be in incorrect state. Tracing will not be possible. `GTL` will return errors to the client on `Trace()`.|


:warning: **Disclaimer**: Currently the IPC tracing backend runs in Engineering Mode therefore the impact of incorrect return values is further reduced as the library will be in the same state as when it fails to connect to the backend.

[^4]:This is already the case for upper level users. `LoLa` will prioritize the IPC communication over tracing. As per its [documentation](../../../../mw/com/design/ipc_tracing/README.md#finding-free-tracing-slots), it will also not call trace anymore on the new messages if there are no free ipc tracing slots left. For Vector Copy IPC `ara::com` all methods are fire and forget, they do not wait for acknowledgment.


## External Interfaces

The library does not use any external interfaces and only exposes a C++ interface, described in the section [below](#c-api-interface), to the platform internal IPC frameworks `ara::com` and `mw::com`.


## Static description of the library

### C++ API interface

#### Registration of trace clients

As stated above, multiple upper layers within the same process may use the `Generic Trace API`.
In order to trigger the appropriate callback when tracing has finished they need to be distinguishable (see [here](#background-sequence)).
Therefore, the `Generic Trace API` library provides the method
`RegisterClient()`, which is the 1st entry-point to be called by any "trace
client".

To register the client needs to pass the following two arguments:
- `app_instance_identifier` which is a string that should reflect which application/process is trying to trace. It will also be later used in the IPC protocol (*TODO: Add link to IPC protocol defintion*) in the app_identifier field. The string has no lentgh limit but the identifier will be truncated to the first 8 bytes.
- `type` of type `BindingType` which indentifies which kind of `ara::com` is being traced by the user. The supported types are provided by the respective enum definition.

The API returns a (process-local) unique id called `TraceClientID` identifying an upper layer and to be used in any subsequent API calls.
If there is a registration attempt with the same arguments the previously registered `TraceClientID` will be returned.

The returned `TraceClientID` will be unique for the process (i.e *local*).
The client will also need to be registered within the IPC tracing backend and due to the [asynchronous behavior](#special-considerations-due-to-asynchronous-behavior) of the library, the registration needs to be delayed.
It will be performed in the background by the worker thread [start sequence](#startup-sequence).
During this registration the locally registered `TraceClientID` will be associated to its *remote* `TraceClientID` provided by the backend.

During the tracing sequence the *local* id will be translated to the *remote* id as can be seen in the [library usage sequence](#library-usage-sequence)

This has some limitations. Consider the following scenario:
In a single ECU, 2 processes (A and B) are communicating through `LoLa` and want to use the tracing infrastructure. They provide the following `app_instance_identifiers`:
-  Process A: `tracker_a`
-  Process B: `tracker_b`

In the description it is mentioned that only the first 8 bytes are taken into consideration so the `app_instance_identifier`s will both be truncated down to `tracker_` ! This means process B will not be able to register as is.

The whole registration sequence is explained in the library usage [sequence](#library-usage-sequence) in the dynamic architecture chapter.

#### (De)Registration of shared-memory objects

The `RegisterShmObject()` function exists because of performance considerations.
As it was mentioned in the introduction the goal is to provide a zero-copy framework to trace data.
A typical user of the generic Trace API is expected to have just a few shared-memory areas where his objects to be
traced are located. This means that the IPC Tracing backend needs to `mmap` those shared-memory areas to be able to access them.
For this reasons two overloaded methods are offered to the user to be able to give the backend read access to its data.

```c++
RegisterShmObject(const TraceClientId trace_client_id, const std::string& shm_object_path)
RegisterShmObject(const TraceClientId trace_client_id, std::int32_t shm_object_fd)
```

The user can either pass a path name or directly a valid descriptor. Under the hood both functions call the same underlying code.
Only difference is for the path variant, `shm_open` will be called with `READ_ONLY` permissions to create a file descriptor.
Please see the [QNX message passing section](#using-qnx-message-passing-to-communicate-with-the-backend) for more details.

The returned object from this method is a local `ShmObjectHandle`. This handle shall be used in the future `Trace()` calls to indicate where the IPC data is.
Another way could have been used to identify where the data is located (e.g. file system path under `/dev/shm`) but
the `ShmObjectHandle` is a more efficient representation.

We say the returned `ShmObjectHandle` is *local* because it is only valid within the process where it is created. Due to the [asynchronous behavior](#special-considerations-due-to-asynchronous-behavior) of the library the memory cannot be mapped (i.e registered) by the IPC tracing backend synchronously during this call.
This registration to the IPC tracing backend will be done at a later point during the [start sequence](#startup-sequence) of the worker thread.
During this registration a *remote* `ShmObjectHandle` will be obtained for each the *local* `ShmObjectHandle`.
This *remote* `ShmObjectHandle` is the key to the IPC tracing backend's memory registry.
The registry, associates this `ShmObjectHandle` to the respective virtual start address of said handle.
With this information the IPC tracing backend can easily access any memory area without needing to call `shm_open` for each `Trace()` call.

For removing the shared-memory area from the IPC tracing backend's address space and registry the inverse operation is provided via `UnregisterShmObject()`. Currently it's agreed that GTL shall not access the resigtered shm object once it receive `UnregisterShmObject()`.
However we shall tolerate the fact that the IPC tracing backend may still access the memory after a request to Unregister and this will lead to tracing garbage data as we cannot block ASIL-B Clients until the backend finishes tracing.

#### Tracing the IPC data forwarded by the users

To actually trace the `ara::com` IPC data the `Trace()` function exists.  It provides two overloads.
One for the zero-copy and another for copy data use case. The latter will synonymously be referred to as "local tracing" in the following.
Details and peculiarities of local tracing are listed [below](#support-for-tracing-of-data-not-yet-residing-in-shared-memory).
Their respective function signature is similar. The type type of `ChunkList` being passed differs between them and only the zero-copy version requires a `TraceContextId`.

```c++
TraceResult GenericTraceAPIImpl::Trace(const TraceClientId trace_client_id,
                                       const MetaInfoVariants::type& meta_info,
                                       LocalDataChunkList& data)

TraceResult GenericTraceAPIImpl::Trace(const TraceClientId trace_client_id,
                                       const MetaInfoVariants::type& meta_info,
                                       ShmDataChunkList& data,
                                       TraceContextId context_id)
```
Therein the following arguments are required:
- local `TraceClientID` (as for every other function)
- the meta-information about the performed trace in the `AraComMetaData` format for `ara::com` which is explained in [below](#meta-info-concepts) paragraph
- Some representation of the data as a `ShmDataChunkList` or `LocalDataChunkList`. More explanations on the [data representation](#datacontent-representation) paragraph.
- `TraceContextId` a unique indentifier for the trace job for the specific client doing the call. This argument is passed only for the zero-copy case and uniqueness needs to be guaranteed by the user of the library. The acknowlegment [callback](#registering-a-callback-to-be-get-on-the-completion-of-a-trace-job) will be triggered with this identifier as the sole argument so the user knows which resources to deallocate.

The full tracing sequence can be seen in the [tracing sequence sub-section](#tracing-sequence).
Before the tracing sequence is performed the translation between the local and remote`TraceClientID` and `ShmHandle`s will be performed by the library see the library usage [sequence](#library-usage-sequence).

##### Meta-Info concepts

Beside the content/payload (user data) of the communication, which shall be
traced, additionally some meta-**information** is needed, that identifies the
concrete occurrence/location of a trace event. In the structural design this
meta-information in the case of `ara::com` is typed by `MetaInfo` class template
instantiated with `AraComProperties`. This approach already shows the extension
point for other upper layers: They should provide their own specific properties,
which are then used to instantiate `MetaInfo` class template accordingly[^1].

In the case of `ara::com` (`AraComProperties`) we have the following properties
to further describe the trace occurrence:

1. `trace_point_id_`: This (type `TracePointIdentification`) is a pair
   consisting of an enumeration value, which defines the `ara::com` API type
   (see  [here](#logtrace-points-for-aracom-and-mwcom)) plus the exact identification of the
   `ara::com` related service instance element, which is affected, represented
   by `ServiceInstanceElement`.
2. `trace_point_data_id_`: An optional identification (see
   [below](#logtrace-points-for-aracom-and-mwcom)) for the traced (user) data. Optional, since not
   all traced `TracePointType`s contain/relate to (user) data. This is kind of a
   unique key generated by the upper layer to support our requirement to trace
   out data only once. Subsequent trace points dealing with the same data only
   reference it via this key ([see here](#logtrace-points-for-aracom-and-mwcom)).

Furthermore The `meta_info` argument is handed by reference. The
`meta_info` might not be located in an area mapped to a shared-memory object!
Therefore, the `Generic Trace API` will copy the relevant portions to a specially reserved area, the TMD area (see [here](#tracing-sequence)). The main reason for this is, that this is trace-specific information. In contrast to the user data it is not already "naturally" held in shared-memory. How this information is technically exchanged
between `Generic Trace API` and the backend is an implementation decision ([see
tracing sequence](#tracing-sequence)).
This means after return from the `Trace()` call the `meta_info` that was passed to the `Generic Trace API` can be safely deleted by the user.

**Note** the size of the `meta_info` is small compared to the user data, making
copy efforts negligible.

###### Current restriction

While the `ara::com` specific meta-info (in the form of `AraComProperties`) is
detailed/good enough to clearly identify any `ara::com` API call on skeleton
side, on the proxy side there is a potential uncertainty, as a user could in
theory create multiple proxy instances for the very same service (skeleton)
instance. This is allowed by the standard ( AUTOSAR `ara::com`), although it is
a rather pathological use case! However, on skeleton side this (creating
multiple skeleton instances for the very same service/instance id) is
impossible. Right now we live with this shortcoming, which could result in two
trace-entries with the very same meta-information from two different proxy
instances, which cannot be mapped to the originating proxy instance within the
code.

##### Data/content representation

The (user) data is passed to the Generic Trace API as raw byte blobs via a `DataChunkList`.
Therein, a chunk is similar to a `span`, is just a tuple of a memory location and a size. The information pointed by that memory location
is refered to as the chunk's data.
The Trace-API uses a list of chunks in the trace call, as
the (user) data to be traced might not be contiguous in memory! E.g. any complex
dynamic data type would consist of a region, where the fixed part has been
allocated and at least a second area (memory resource), where dynamic extent has
been allocated.

In the case of a `ShmDataChunkList` the location is represented via  a `ShmObjectHandle`
in which the data resides and an offset into this shared-memory object where the data starts.
The `ShmObjectHandle` therein identifies the shared-memory area.
The reason is simple: This information will be used by
IPC tracing backend, which maps the same shared-memory object into
its memory as the user/`ara::com` application does via the [`RegisterShmObject`](#deregistration-of-shared-memory-objects) API. If the application, which
uses the Trace-API would define a chunk location via a virtual address from its
point-of-view, this would not be a usable address for the logger process!

In the case of a `LocalDataChunkList` the location is represented by a plain `void*` pointer.
Hence, the data is directly accessible by the library.
Since it is not residing in shared memory the library needs to copy it into shared memory to share it with the backend.
Details are given in the paragraph below.

**Note**: It is obviously the job of the middleware (`ara::com` implementation)
to care for dense/contiguous allocation in memory and therefore restricting the
length of the `ShmDataChunkList` accordingly for performance reasons! Outputting
several (smaller) chunks via DMA, compared to few (bigger) chunks is less
efficient!

##### Support for Tracing of data not yet residing in shared memory

In some cases the data to be traced out (handed over to a
`Trace()`call) is not residing  within a shared-memory region or the memory region is not in an area DMA-accesible area
(also referred to as [typed memory](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.sys_arch/topic/ipc_Typed_memory.html) in QNX).
For this case the `Generic Trace API` provides an overload for `Trace()`, which uses
`LocalDataChunkList` as `data` instead of `ShmDataChunkList`. In this case it is
the responsibility of the `Generic Trace API` library implementation:

- to copy the data behind the chunks of the `LocalDataChunkList` into a shared-memory area accessible by the
  backend.
- transform the `LocalDataChunkList` into a `ShmDataChunkList` pointing to the copied data
- place the `ShmDataChunkList` into the above shared-memory region to share it with the backend as well
- handle the lifetime of this memory and free it, when the data has been traced
  out.

Note, that in this overloaded variant of `Trace()`, **no** callback/context id
mechanism gets applied! I.e. the referenced data chunks within
`LocalDataChunkList` will be copied synchronously within the call to Trace()
into some shared-memory buffers by the `Generic Trace API` library instance.

To avoid performing expensive dynamic allocations during tracing the copy is done on a pre-allocated area using a special lock-less thread-safe circular allocator.
The design of the allocator can be found in [here](../../score/static_reflection_with_serialization/flexible_circular_allocator/design/Readme.md).


##### Timestamp Acquisition

Whatever data is traced via the generic Trace API, there is the common need, to
annotate this trace data with a timestamp, which has to be as close as possible
at the trace point. Therefore, for `ara::com`/`IPC` tracing as there is no need for the upper layer to acquire the
timestamp on its own, the acquisition takes place directly within the call
to the `Trace()` API.

##### Loss of trace data

The upper layers using the `Trace()` API need to reserve (memory)
resources containing the trace data pointed by the `ShmDataChunkList` chunks until the
`TraceDoneCallback` comes in. While waiting on the callback the upper layer might run out of
(memory) resources.
Now, if new trace events happen additional memory would be needed.
As this is not possible, the upper layer will not trigger a `Trace()` call.
This "loss of trace data" shall be memorized by the upper layer and the `Generic Trace API` library shall be informed about this
issue by setting a "data-loss" flag in the `StatusBits` in a subsequent
`Trace()` call. Naturally, the data loss can only be reported from within the
same context as no serialization of library calls is expected. After an upper
layer has set the flag for the next call, it shall be reset and only set again
if a new loss has been detected.

**This is not yet implemented and will be done under : broken_link_j/Ticket-145614**


#### Registering a callback to be get on the completion of a trace job

The `Generic Trace API` implementation obviously needs some time to trace
out/write out the information given as arguments to the `Trace()` call overload
for `ShmDataChunkList`s. Not (only) because the I/O operation itself could/
might take time, but especially because the real implementation of the tracing
takes place in a different process, the IPC tracing backend, which is completely asynchronous to the
caller of the `Trace()` API call.

Since the API semantics is completely asynchronous, the library will notify the user
that its trace call has been completed via a user-defined callback.
This will let the user know that it can free/release all memory ressources associated with said job.
Of course this is only the case if the zero-copy interface is used. Conversely, in local tracing everything is copied and handled by the library.
Therefore the library itself will perform the clean-up for said job.
This is also the case for the metadata since it is also copied to TMD memory area. It is also the library's responsability to mark the corresponding memory as re-usable.

To register a callback the `Generic Trace API` has the `RegisterTraceCallDoneCB()` API.
This method takes two parameters:
- `TraceClientId`: This correspond the client's id that wants to register its own callback.
- `TraceDoneCallBackType`: This is the callback the client wants to register. It is a scoped function of type `safecpp::MoveOnlyScopedFunction<void(TraceContextId)>`.


The library will trigger the callback passing the `TraceContextId` of the completed job so the client can know which job was completed.
This `TraceContextId` is set by the client for each job during the `Trace()` call.

This means there cannot be two `TraceContextId` **active** at the same time with the same value.
("active" means here that it has been given to a `Trace()` call, but no callback has been called for it yet)
It is the responsability if the client to ensure the unicity of this field to make sure no memory area is cleaned/reused before tracing.
If this is not the case, traces on the wire will be incomplete.

The registered callback has also some additional restrictions:
- The callback shall be thread-safe. All callbacks are triggered from **one single thread** running in the background and therefore from a different thread from where the API calls are performed by the client (see this [section](#worker-thread-sequence) for further information).
- The callback should be as efficient as possible. It will be called for each single job completed and if unefficient could really increase the client's CPU usage.


### Static class diagram

The library API has been designed to be used by calling directly the member functions as they have all been implemented as static.
The underlying implementation uses a mechanism similiar to a singleton pattern. In classic singleton pattern one single instance of `GenericTraceAPI` will be created statically and the user will get an instance of the same class type by calling `GetInstance()`.
In the library case the `GetInstance()` method is private and called implicitly for all API calls. Moreover `GetInstance()` does not create a static class of type `GenericTraceAPI` but rather of type `ITraceLibray` an interface class which can be resolved to either a `GenericTraceAPIImpl` where the actual logic is performed or a mock if it is set (see the [testing facilities](#testing-facilities) for more detailed information). With this setup the library supports lazy initialization as the implementation class will only be created/initialized on the first call into the API.

In the following diagram we can see the relationship between the four high level classes:

![API Base diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/generic_trace_api_static_class.uxf?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

- `ITraceLibray`: the virtual interface class
- `GenericTraceAPI`: the class exposed to the client and that shall be used to perform requests
- `GenericTraceAPIImpl`: The class where all the logic is actually performed. It's structure will be further defined below
- `TraceLibraryMock`: mock class to be used in unit testing. It is a friend class of `GenericTraceAPI`, it will inject itself on construction.

The `GenericTraceAPIImpl` class uses six main components to perform the needed logic:
- `ObjectFactory` : This is the factory class it creates 3 of the other needed components (`DaemonCommunicator`, `TraceJobAllocator`, `TraceJobProcessor`).
- `DaemonCommunicator`: This class is used by the library to communicate with the IPC tracing backend. It opens and maintains the connection via QNX message passing to the backend. It is used to send client, memory registration and unregistration requests.
Also any crash or termination for the IPC tracing backend should be detected by this instance, on such detection the `DaemonCommunicator` should invalidate the global state of the `GenericTraceAPIImpl` by invoking a previously registered callback via `SubscribeToDaemonTerminationNotification()`.
- `TraceJobContainer` : This class is used to keep track of which jobs have been sent and are pending. When a job is allocated an element is added to the container, when the job is done (e.g the status in the ring buffer has been updated by the backend) it is removed from the container. The underlying container is a `AtomicRingBuffer<class T, std::uint16_t AtomicRingBufferSize>`, a thread-safe atomic container that is populated like a ring buffer.
- `TraceJobAllocator`: This class allocates all needed ressources and performs the necessary steps for a trace job:
  - It allocates the metadata in the TMD
  - If it is a local trace job, it copies also the IPC data to the TMD area
  - It copies the control information of the `ShmDataChunkList` to the TMD area
  - Fills the ring buffer element with all the necessary information
  - Adds the job to the `TraceJobContainer`
- `TraceJobProcessor`: This class is responsible for triggering the user-defined callbacks and clean the memory associated with a job from the TMD area. It uses the `TraceJobContainer` to know on which elements from the ring buffer it should check the status. The `ProcessJobs` method is triggered periodically by the library's [worker thread](#worker-thread-sequence). `TraceJobContainer` is just a wrapper on top of an atomic ring buffer, you can look at the [ring-buffer section](#ring-buffer) for more details on the algorithm. A global fatal error will be set if processing fails.
On the other hand `CleanPendingJobs()` is trigger within the context of the same thread only one time if the IPC Tracing backend crash/termination is detected.
- `MemoryValidator`: This class is used to check that the registered shared memory is in typed memory. It is not a strict requirement to perform this check but it allows for faster feedback to the user if the memory is in the incorrect area.

To keeps all information about the client the class uses two containers:
- `ClientIdContainer`: Keeps track of association between the local and remote `TraceClientIDs`. Before the IPC tracing backend is available only the local `TraceCliendID` are stored. The corresponding remote `TraceClientID`s will be stored during the [worker thread](#startup-sequence) start sequence.
- `ShmObjectHandleContainer`: Keeps track of association between the local and remote `ShmObjectHandle`. Before the IPC tracing backend is available only the local `TraceCliendID` are stored. The corresponding remote `ShmObjectHandle`s will be stored during the [worker thread](#startup-sequence) start sequence.
Both containers actually store their data using a generic thread safe atomic container called `AtomicContainer<class T, std::size_t AtomicContainerSize>`.
The detailed algorithm on how the container ensures that multiple threads can add elements to it can be found in [here](#special-considerations-due-to-asynchronous-behavior)

![Class diagram GenericTraceAPIImpl](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/generic_trace_impl_class.uxf?ref=ee82d91915f6feb420fc63c4eeec6b477557cc4a)


### Error codes

Function calls of the `Generic Trace API` have the possibility to return an
error if the function call fails. The error code is chosen to be of type
`score::result` as it allows to deduce a meaningful error-code and provide a
(human-readable) string-like message to be logged. It is expected that error
codes are provided to distinguish between library-internal errors,
backend-connectivity issues and contract violations from the caller.
We distinguish between two error categories:
- **Unrecoverable** errors : Means the library is in an unknown state and further calls to any API function will return an error until the next power cycle.
- **Recoverable** errors : Some fault occur during the execution of a given API function due to incorrect parameters or unavailable resources at the moment (e.g : TMD area is full). Caller can try again later to call any API function.

To see the complete list of error codes please look a the implementation in the [error_code.h](./../generic_trace_api/error_code/error_code.h).

Error code returned by the library can be caused by three sources:
- **Global error** - in case an unrecoverable (fatal) error occurred in the Worker thread, subsequent calls to any other function in library return the respective error code, which has information about the source of the problem. This kind of error won't be cleared until the next life cycle of the library. As the error can be set and read from any thread it is implemented as an atomic data type.
- **Pending error** - some library operation like registering shared-memory object or registering client can be executed when library is not yet connected to the daemon (early client registration scenario). Actual register operation will be executed later when the daemon is available and therefore some error could occur in asynchronous manner at this step. In this case error will be stored as client specific pending error and will be returned on next call to Trace() method and error code will be cleared afterwards. Client needs to retry operation that returned such error, to be able to trace data.
- **Operation error** - standard type of error that method returns if during execution will approach some problem.

### Library dependencies

#### Dependency to BMW `amp` library

The `Generic Trace API` was designed for C++14 infrastructure. Therefore some `C++17` functionalities like
`std::variant`, `std::optional` (or even newer `C++23` `std::expected`) were
not used, so the library is relying on its BMW `amp` library replacements
(`score::cpp::variant`, `score::cpp::optional`, `score::cpp::expected`).

#### Dependency to `lib/memory/shared`

The `Generic Trace API` uses the library [`lib/memory/shared`](../../../../lib/memory/shared) to create and manipulate shared-memory.
Specifically it uses the library to allocate the TMD memory area in typed memory as shared.
`lib/memory/shared` offers a simplified way to allocate this kind of memory using the [typed memory daemon](../../../../intc/typedmemd). This component is responsible for allocating typed memory in a specific safe area.

#### Dependency to `mw::time`

The `Generic Trace API` needs to obtain a timestamp to communicate the IPC traces using the PLP protocol as specified in the IPC tracing platform [requirements](broken_link_c/issue/4988910).
It uses the [`mw::time`](../../../../mw/time), specifically the [HWLoggerTime](../../../../mw/time/HWLoggerTime/README.md) interface, to obtain the time.

#### Dependency to `lib/os`

The `Generic Trace API` uses the [OSAL library](../../../../lib/`os/README.md) to call POSIX primitives to ensure portability between Linux and QNX.

#### Dependency to `lib/result`

The `Generic Trace API` uses the [Result library](../../../../lib/result/README.md) to return errors to the user as exceptions cannot be used. For a description on the error codes used by the library refer to this [section](#error-codes).

#### Dependency to `language/safecpp/scoped_function`

The `Generic Trace API` uses the move_only_scoped_function from the [safecpp library](broken_link_g/swh/safe-posix-platform/blob/stable-25/score/language/safecpp/scoped_function/README.md) for the trace job callback type. The scope is created and controlled by the user of the library (see this [line](broken_link_g/swh/safe-posix-platform/blob/stable-25/score/analysis/tracing/score/static_reflection_with_serialization/types.h#L109)).

#### Dependency to `lib/concurrency`

The `Generic Trace API` uses the interruptible `wait_for` provided by the [concurrency library](../../../../lib/concurrency/README.md) inside the library's worker thread.
This function lets a thread sleep until the time has elapsed or the passed `stop_token` is requested to stop. As the thread does not wait for the entirety of the time, this reduces the shutdown time.

## Dynamic Architecture

### Library usage sequence

The sequence diagram below shows the general functionality and asynchronous
behavior of the DMA Trace API. It shows how the client is supposed to call the API and in which order.
Certain calls will wait for the correct state to perform the action as some pre-conditions are needed.
All the library states are defined in the `LibraryState` enum. Currently 4 are supported:
- `kNotInitialized`: Library is not ready. `RegisterClient()` will wait for a state change notification by the worker thread
- `kInitialized`: Library is ready.
- `kDaemonInitialized`: IPC tracing backend is ready.
However, library is not ready but can register clients and (de)register shm objects.
- `kDaemonDisconnected`: IPC tracing backend is disconnected due to crash or normal termination.
- `kGenericError`: An unrecoverable error occured during startup. No calls into the library are possible.

In the `kNotInitialized` only calls to the registration functions is possible. The `Trace()` function will return a recoverable error as the backend is not available yet. This is part of the asynchronous behavior of the library that was introdcued to deal with startup performance constraints. More information can be found in the next [sub-section](#special-considerations-due-to-asynchronous-behavior)

The current behavior is:

![Sequence View](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/trace_library_usage_sequence.puml?ref=ee82d91915f6feb420fc63c4eeec6b477557cc4a)


### Special considerations due to asynchronous behavior

As part of the effort to reduce startup time, it was discovered that the IPC tracing backend and the associated BSP components needed a lot of resources to start. Therefore, the IPC tracing backend be moved to a later point in the start cycle as there are is no need for startup traces.
However, the first design of the library did not support a late startup of the IPC tracing Backend so a few changes were introduced. These design changes have some limitations and should be re-evaluated in future releases.

The main support comes in the form of allowing local only registrations for all register calls while the IPC tracing backend is being started.
This means the all registration calls will return success as long as they comply with the expected format. The actual registration to the IPC tracing backend (via the `DaemonCommunicator`, see [here](#using-qnx-message-passing-to-communicate-with-the-backend)) which will be done within the worker thread [startup sequence](#startup-sequence).
If the registration to the IPC tracing backend fails, clients will not be immediately notified, instead they will encounter the error at a later point, during the `Trace()` call as the remote client will not exist.

To keep track of all the local registrations for both clients and shared memory objects, the library uses the `ClientIdContainer`and the `ShmObjectHandleContainer`.
Both containers actually store their data using an `AtomicContainer` whose design documentation can be found [here](../generic_trace_api/containers/atomic_container/design/README.md).
This container only guarantees that elements are acquired atomically therefore only one part of the data is written only on acquisition of an element.
The other field, the remote ids, will be read and updated by the consumer thread where there is no concurrency.

### Library startup sequence

Upon static initialization the library creates the necessary resources, including all needed heap memory. Rationale: Heap allocation after static initialization is forbidden in an FFI context.

![Library startup sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/library_start_sequence.puml?ref=a6afad65d545d0c12f7f382cdec7d1638c3d7893)

### Library state machine

This section shall show all the states of GTL and their transitions. GTL shall support the re-registration of all clients upon re-connection of the IPC backend during the same lifecycle.

![Library state machine](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/library_state_machine.puml?ref=e331c110d8fcb260bbeceaf8390bd76d8d241e38)

### Worker thread sequence

All initialization takes place in `GenericTraceAPIImpl` constructor. Worker thread starts additional steps after establishing connection to the Daemon.
The worker thread is responsible for triggering the deallocation callbacks both the TMD internal one and the user-defined ones, as well as performing the state transitions.

#### Startup Sequence

The startup sequence initiates when the user first calls the library. A static instance is created and the worker thread is spawned.

![Worker thread startup sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/worker_thread_start_sequence.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

#### Background sequence

To trigger the different callbacks the worker thread uses the `TraceJobProcessor` which queries the  `TraceJobContainer` to check the state of the send jobs periodically.

![Worker thread background sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/worker_thread_main_sequence.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

The `TraceJobContainer` uses also an atomic ring buffer like the one described in the [ring buffer section](#ring-buffer) to locally keep track of the jobs added to the queue.
So when adding elements to the queue the `GetEmptyElement()` function is used and to get elements from the queue the `GetReadyElement()` function is used.

#### Shutdown sequence

Upon static deinitialization of the library, as well as in case the IPC Tracing backend terminates, the worker thread starts its shutdown sequence as described below:

![Worker thread shutdown sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/worker_thread_shutdown.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

### Internal communication between trace library and IPC trace backend

There are two ways the trace library can communicate with the IPC tracing backend :

- Synchronously via QNX message passing: This method is used for all regsitration requests (client and shared memory objects) as this communication needs to handled synchronously. We need a positiv return from the backend to indicate that the registration was immediate and succesfull otherwise race condition can occur. The library needs to be sure that the client and the necessary memory are properly configured in the backend before any tracing request can be served. This registration calls happen only at startup and a few times only, the performance overhead of using QNX message passing can be tolerated. More details on the implementation can be found in the next [sub-section](#using-qnx-message-passing-to-communicate-with-the-backend)

- Asynchronously via the trace job queue (ring buffer): The `Trace()` API will be called every time there is an IPC message to be traced. For many applications this means the API can be called up to once every 20ms which is with very high frecuency. Therefore to avoid performance costs an asynchronous mechanism is used to communicate which information needs to be traced to the IPC tracing backend. For this purpose we use an atomic [ring buffer](#ring-buffer) that is shared among all library instances.

#### Using QNX message passing to communicate with the backend

To communicate with the IPC backend synchronously the library uses [QNX message passing](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.getting_started/topic/s1_msg.html).

To have a uniform sequence to send the requests and receive the responses, the libray and the backend use the following data structures :
- Class [`Request`](broken_link_g/swh/ddad_platform/blob/master/aas/analysis/tracing/score/static_reflection_with_serialization/daemon_communication_request.h) which contains:
  - `message_id_`: an id of type enum `DaemonCommunicatorMessage` which indicates which type of request the class is for
  - `request_`: the request itself which is an `score::cpp::variant` of the different types of request structs that exist:
    - `RegisterClient`: contains the app identifier and the binding type
    - `RegisterSharedMemoryObject`: contains the `shm_handle_t` that the backend needs to open and mmap
    - `UnregisterSharedMemoryObject`: contains the `ShmObjectHandle` that needs to be unregistered
- Class [`Response`](broken_link_g/swh/ddad_platform/blob/master/aas/analysis/tracing/score/static_reflection_with_serialization/daemon_communication_response.h) which contains:
  - `response_`: the response data itself which is an `score::cpp::variant` of the different types of response structs that exist:
    - `DaemonProcessId`: contains a `pid_t` to store the returned PID
    - `RegisterClient`: contains a `TraceClientId`
    - `RegisterSharedMemoryObject`:  contains the `ShmObjectHandle` that the backend created for that shared memory
    - `UnregisterSharedMemoryObject`: contains a `bool` signalling if the unregistration was successful
During each register call a request with the correct parameters and an emtpy response are created and then sent.


On the library side the class responsible to ensure this communication is the `DaemonCommunicator`. It offers 4 services:
- `Connect`: Is called by the factory as the first step. It establishes the connection with the IPC backend:
  - opends the named server created by the backend
  - stores the return channel_id for sending the messages later
  - requests the backend's pid number.
  - stores the returned PID
- `RegisterClient`: Performs all needed logic to register a client:
  - prepares a request with the client's app indentifier and binding type
  - creates an empty response to later the return the answer to the calling function
  - uses `SendMessage()` to send the request to the backend to register a client.
  - returns the received `TraceClientID` or an error
- `RegisterSharedMemoryObject`: Performs all needed logic to register a shared-memory area to the IPC backend:
  - Creates a `READ_ONLY` QNX `shm_handle` based on the file descriptor or path passed by the client for unique use of the LTPM using the previously stored PID(see [`shm_create_handle()`](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_create_handle.html)). This is needed for safety and security reasons as IPC data has an ASIL B integrity level and the IPC backend is only QM. Then the handle is shared with the backend and response is returned.
  - prepares a request with the `shm_handle`
  - creates an empty response to later the return the answer to the calling function
  - uses `SendMessage()` to send the request to the backend to register the memory area
  - returnes the received `ShmObjectHandle` or an error
- `UnregisterSharedMemoryObject`: Performs all needed logic to unregister a shared-memory area from the IPC backend:
  - prepares an unregistration request with the `ShmObjectHandle` that needs to be unregistered
  - creates an empty response to later the return the answer to the calling function
  - uses `SendMessage()` to send the request to the backend to unregister
  - returns blank or an error

All register functions use the `SendMessage()` function which takes as parameters a `Request` and a `Response`. It is in this function that the `MsgSendv()` primitive is called. However, as the library is FFI, we need to guarantee that any call into the library is time-bound. To achieve this we use [`TimerTimeout`](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/t/timertimeout.html).
This function sets a timeout on any kernel blocking state, blocks for a specified time, or sets the timer tolerance for these timeouts, depending on the flags.
In our case we use set a fixed timeout for the states SEND and REPLY and trigger an UNBLOCK signal if the timeout is exceeded.
With this mechanism we can guarantee that the thread will not be blocked forever during the `MsgSendv()` call either waiting for the message to be sent or waiting for a reply. If the `MsgSendv()` times out, it will return with error `ETIMEDOUT`. If the server, the backend, unexpectedly dies during  `MsgSendv()` then it will return `ESRCH` as per the QNX [documentation](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/msgsendv.html).

The full algorithm is:

![MessageSend algorithm](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/send_msg_backend.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

The expected behavior depending on the reply from the IPC tracing backend is:
- If MsgSend() times out before MsgReceive() completes then the library will receive an `ETIMEDOUT` error. An error will be returned by the library to the client. The backend will not receive this message.
- If MsgSend() times out after MsgReceive() but before MsgReply() then on reply the backend will get an `ESRCH` error and reply will not be sent. The library will return an error to the client . On the second MsgSend() the correct reply will be received.
- If MsgReply() comes before MsgSend() times out the reply will be delivered to the library. This is the normal use case.

See [ticket](broken_link_j/Ticket-187609) for more details.

#### IPC Tracing Backend termination/crash detection

Upon construction, the `DaemonCommunicator` spawns a dedicated thread named
`DaemonCrashDetectorThread`. This thread establishes a QNX pulse mechanism to
monitor the termination of any processes connected via message passing to the
GTL instance.

According to the [QNX
Documentation](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/p/_pulse.html#:~:text=_PULSE_CODE_COIDDEATH),
a _PULSE_CODE_COIDDEATH pulse is sent when a connected process terminates. By
tracking the connection ID obtained during initialization (see
[here](#using-qnx-message-passing-to-communicate-with-the-backend)), the thread
can reliably detect when the backend daemon crashes or becomes unavailable.

Upon detecting this condition, the thread triggers a previously registered
callback from `GenericTraceImpl`, which updates the atomic `state_` to
`kDaemonDisconnected`. This change prompts the `WorkerThread` to begin its
cleanup routine and proceed with termination, as illustrated in the following
sequence diagram.

![IpcTracing backend termination
detection](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/daemon_termination_detection.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

#### Ring buffer

The library uses a circular queue (ring buffer) to communicate the trace requests to the IPC Tracing backend
It has the following characteristics:
- It resides in shared-memory so it can be accessed by the backend and all library instances (clients)
- It is thread-safe and atomic as it needs to gurantee concurrent access between the backend and the clients without inter-process mutexes
- It is created by the IPC Tracing backend under a statically defined pathname `/dev_shmem`. The library opens this path during worker thread startup sequence, during the creation of the `TraceJobAllocator`.


Currently the number of elements of the ring buffer is statically defined by the IPC Tracing backend on creation.
Each ring buffer element ([`ShmRingBufferElement`](../../shm_ring_buffer/shm_ring_buffer_element.h)) contains the following information:
- `GlobalTraceContextId ` : This contains both the `ClientID` and the `ContextID` identifier. The combination of both uniquely indentifies each trace request.
- `TraceJobStatus` : Atomic status of the current trace request. It has 5 possible values but only 4 are used:
  - `kEmpty`: Element is empty and can be used to allocate a trace request. When the backend completly serves a request it will set the status to `kEmpty` to mark the element as reusable.
  - `kAllocated`: Element is marked allocated when element is acquired by a client but not yet ready to be traced.
  - `kReady`: Element is ready to be traced
  - `kInvalid`: If there was a problem between acquiring an empty element and setting the element to `kReady` (for example if ) the element will be marked as invalid and will be ignored by the backend during processing. The element will be immediately set to kEmpty.
- `SharedMemoryLocation`: The start location of the `ChunkList` containing the data to trace. It consists of the shared memory handle of the TMD memory area returned by the IPC tracing backend upon registration for the tracing library instance and an offset in that memory area. This information is enough for the backend to be able to retrieve the `ChunkList`.

The library only performs these operations on the ring buffer :
- `Open()`: Opens the memory path `/dev_shmem` and reinterprets the underlying data as a [`ShmDataSegment`](../../shm_ring_buffer/shm_data_segment.h#L32) so the library can read and add jobs to the queue. The class `ShmDataSegment` is the representation of the ring buffer. It contains:
  - `ShmRingBufferState`: Atomic 32-bit integer representing the current state of the ring buffer, contains :
    - `std::uint8_t empty : 1`: Empty flag. Set to true if ring buffer is empty. It lets us differentiate between empty and full ring buffer (as in both cases `start` == `end`).
    - `std::uint16_t start : 15`: Start index of the ring buffer. Increases with acquisition of an empty element.
    - `std::uint16_t end : 16`: End index - 15 bits in use but 16-bit width to fill full 4 bytes in structure. Increases with acquisistion of a ready element.
  - `ShmVector`: Vector of `ShmRingBufferElement`, this is where the actual elements are stored.
  Finally after this reinterpretation is complete the internal state of the local ring buffer handler class is set to initialized.
- `Create(std::size_t size)`: Create the `ShmRingBuffer` in shared memory
- `GetEmptyElement()`: Acquires a reference to an empty element from the ring buffer. This is used during the [tracing sequence](#main-sequence). The reference will be updated with the correct data.
- `GetReadyElement()`: Acquires a reference to the first ready element in the ring buffer which is ready to be processed or an error code if the ring buffer is empty or a ready element couldn't be acquired. If an obtained element is marked as invalid (due to an unrecoverable issue during trace job allocation) then this function will mark the element is empty, update the state and indexes and retry to get the next ready element.
- `GetUseCount()`: Method to atomically fetch the `ShmDataSegment` state and compute statistics based on it
- `IsValidState()`: Helper function to validate the state of the `ShmDataSegment`
- `IsBufferEmpty()`: Helper function to check if the `ShmRingBuffer` is empty. Used for testing purposes
- `CreateNewState()`: Helper function to create a new state based on the current state and index
- `TryFetchElement()`: Helper function used for reducing cyclomatic complexity
- `TryUpdateStateAtomically()`: Helper function to attempt a `ShmDataSegment` state update

The algorithm for `GetEmptyElement()` is:
![get empty element algorithm](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/ring_buffer_get_empty.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

The algorithm for `GetReadyElement()` is:
![get ready element algorithm](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/ring_buffer_get_ready.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)


### Tracing sequence

In this section we will explain how the details of the `Trace()`operation for both the local and shared memory data usecase.
Specifcially we will focus on the sequence followed by the `TraceJobAllocator` to add a job to the trace job queue.
Therefore we assume the transalation between local and remote `TraceClientID` and `ShmHandle`'s has already been performed.

The `TMD` or `TraceMetaData` memory area is owned and created by the
library instance to be reused during the runtime to allocate all needed control data.
Currently it is allocated in typed memory as to support the copy IPC tracing use case the IPC data needs to be copied to typed memory.
To simplify implementation, all data will be copied to this area. The allocation of data will be done during then during the tracing sequence and the deallocation will be done in the worker thread [main sequence](#background-sequence)

To perform any memory allocation in the TMD area the lockless flexible allocator is used. For more information on its allocation and deallocation algorithm please refer to its [documentation](../../score/static_reflection_with_serialization/flexible_circular_allocator/design/Readme.md).

The size of the TMD could roughly be calculated as per the
following formula:

$$ TMD = \frac{{\left(B/8 \times P_{\text{Plugin}}/1000\right)}}{{\text{{N}}}}
\times \left(\frac{{P_{\text{Plugin}} +
P_{\text{WorkerThread}}}}{{P_{\text{Plugin}}}}+ 2\right) $$

where:

- \( TMD \) is TMD big chunk size in Bytes,
- \( B \) is Network Bandwidth in Mbps,
- \( P \) is  periodicity in ms,
- \( N \) is number of library instances,

#### Main sequence

For both copy and zero-copy cases the main steps are almost identical:
- Allocating in TMD the memory area for the IPC meta information. Then copying the data passed by the client to said location
- Allocating in TMD the memory area for the timestamp. Then copying the data passed by the client to said location.
- Save the ChunkList to the TMD area.
It is during the last step that we will see the key difference between the two jobs. As for the copy IPC case the IPC data also needs to be saved to TMD area. See the next [subsection](#saving-the-chunklists-to-the-tmd-area) for the complete save algorithm.


The different steps for the zero-copy case are:

![zero copy trace sequence](./assets/shm_chunk_list_copy.svg)

The different steps for the copy case are:

![copy trace sequence](./assets/local_chunk_list_copy.svg)

The full sequence diagrams can be found below.

For data residing in shared memory (zero-copy tracing):

![Shm data trace sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/shm_data_tracing_sequence.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

and for data in local memory(copy tracing):

![Local data trace sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/local_data_tracing_sequence.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)


#### Saving the ChunkLists to the TMD area

The diagrams above show a call to `SaveToSharedMemory()` which performs the copy on the chunk list to the TMD area.
A particularity of this operation is that althought the client passes a `ShmDataChunkList` the copied data in the TMD will be of type `ShmChunkVector`.
The difference between the underlying type of list that is used. For `ShmDataChunkList` the actual data is a `std::list<SharedMemoryChunk>`.
However for the `ShmChunkVector` a custom list type is used `shared::List<SharedMemoryChunk>`. Further documentation on this custom type can be found [here](../../score/static_reflection_with_serialization/shared_list/doc/SharedList.md). We need this custom list as the nodes of the list need to allocated in the TMD area and not in the heap. The same is true for the `LocalDataChunkList`

The detailed algorithm for the zero-copy case (`ShmDataChunkList`) is:

![Shm save to shm memory sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/shm_save_to_shm_mem.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)

The detailed algorithm for the copy case (`LocalDataChunkList`) is:

![Local save to shm memory sequence](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/analysis/tracing/library/design/assets/local_save_to_shm_mem.puml?ref=01b927eb7f89e2f82a3e0f6f9c9479bbfd3cbf73)


## Log/Trace-Points for ara::com and mw::com

The detailed design on the interactions between the upper layer libraries and the generic trace library can be found:
- For `ara::com` see the `IPCTraceAdapter` design [documentation](../../../../ara/IPCTraceAdapter/detailed_design/README.md)
- For `mw::com` see the tracing framwork design [documentation](../../../../mw/com/design/ipc_tracing/README.md)

## Variability

The `Generic Trace API` has a single point of variability in terms of build configuration. It offers both a QNX and Linux implementation. Most of the code is completely portable from QNX to Linux except the `DaemonCommunicator` and the `MemoryValidator` which are QNX only. For these classes in particular a stub implementation is provided for the Linux build and selected using the Bazel select mechanism.
The library does not use configuration files.

## Design rationales

### Mathematics
The library does not use floating point or fixed point arithmetics.

### Bit-fields
Library uses bit-fields for the `ShmRingBufferState` to have a compact 32 bit structure that fully describes the ring buffer state that can be atomically updated. More details in the ring buffer [section](#ring-buffer)

### Exceptions
The library does not throw exceptions.
Any exception thrown by the standard library will end in `std::terminate` as per exception handling [safety requirement](broken_link_c/issue/81989).

### Concurrency
The library can be used in a multiple producer, single consumer scenario.
The library supports that multiple clients within one process (i.e. multiple threads) call its APIs.
API calls are synchronous themselves and will return either an error or the expected value.  However, processing of services requested can be delayed to a later point (see [asynchronous behavior section](#special-considerations-due-to-asynchronous-behavior)).

As explained in the sections above, one thread (called the `worker thread`) will be spawned to consume the elements that were added to the processing queues.
This `worker_thread` is an `score::cpp::jthread` that takes as an argument a `score::cpp::stop_token`. The `worker_thread` will be requested to stop through this `score::cpp::stop_token` in the destructor of the `GenericTraceAPIImpl` class. This setup ensures the detached thread is joined during shutdown/destruction and all acquired resources (specially in terms of memory) can be freed.

### Assembly language
The library does not use assembly language in its source code.

### Algorithm
The library uses some non-trivial algorithms for data management:
- Flexible circular allocator for memory allocations in the TMD: See [design](../../score/static_reflection_with_serialization/flexible_circular_allocator/design/Readme.md)
- Allocation of the `ChunkList` within the TMD memory area with custom double linked list: See [design](../../score/static_reflection_with_serialization/shared_list/doc/SharedList.md)
- Atomic container to store elements atomically across multiple threads: See its  [design](../generic_trace_api/containers/atomic_container/design/README.md) for details.
- Atomic shared ring buffer for interprocess communication with a multiple producer, single consumer use case: see [ring buffer section](#ring-buffer)


## Testing facilities

To be able to easily mock the generic trace library in unit tests a mock and a way to inject said mock into the static c++ API are provided.
The idea is to be able to replace which underlying class will be returned by the `GetInstance()` method when it is called by the API functions.
Instead of returning a class of type `GenericTraceAPIImpl`, a class of type  `TraceLibraryMock`, a gtest mock, will be returned.

To achieve this the top level class `GenerTraceAPI` has a private member variable `mock_`of type `ITraceLibrary*` which is set through the private function `InjectMock(ITraceLibrary* mock)`.
The class `TraceLibraryMock` is declared as friend to `GenerTraceAPI` which let's it call the private method `InjectMock(ITraceLibrary* mock)` on contruction to inject itself. The `GetInstance()` will then return a `TraceLibraryMock` as the `mock_` member variable is not a null pointer.

With this setup, the only thing the user needs to mock the library is to create the class `TraceLibraryMock` before any call into the library is made:

```c++
TEST(RegisterClientSuccess)
{
  generic_trace_api_mock = std::make_unique<score::analysis::tracing::TraceLibraryMock>();

  GenericTraceAPI::RegisterClient(app, binding);

  EXPECT_CALL(*generic_trace_api_mock.get(),
              RegisterClient(app, binding))
}
```
