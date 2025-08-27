# Generic DMA Trace Library

## Table of Contents

- [Detailed implementation design](#detailed-implementation-design)
- [Security Concept for the Generic Trace Library](#security-concept-for-the-generic-trace-library)
  - [Acquired Abilities](#acquired-abilities)
  - [Channel connection](#channel-connection)
  - [Pathname attach](#pathname-attach)
  - [Deployment User and Group IDs of the user application](#deployment-user-and-group-ids-of-the-user-application)
- [Suported Log/Trace points](#suported-logtrace-points)
  - [Provider/Skeleton Side Trace-Points](#providerskeleton-side-trace-points)
  - [Consumer/Proxy Side Trace-Points](#consumerproxy-side-trace-points)
  - [Format of content in Trace-Points](#format-of-content-in-trace-points)
    - [Content in chunk lists](#content-in-chunk-lists)
    - [SubscribeInfo struct](#subscribeinfo-struct)
  - [Configuration Flexibility](#configuration-flexibility)



## Detailed implementation design

The detailed design of the generic trace library is in [./design/README.md](./design/README.md)


## Security Concept for the Generic Trace Library

This section shall provide all the security integration notes needed for the
safe integration of the Generic Trace Library.

### Acquired Abilities

The following abilities are needed for the core functionality of the library

| Ability | Rationale |
|:-------------------|:--------------:|
| xthread_threadctl  | Once the Library APIs are utilized, a `worker_thread` is spawned to handle the daemon communication and the request cleaning up |
| pathspace  | To create the TMD shared memory and to attach the message passing server    |
| map_fixed  | To create and `mmap()` the TMD shared memory area  |

### Channel connection

The following channel connections are needed for the core functionality of the
Generic Trace Library

| Connection | Rationale |
|:-------------------|:--------------:|
| devb_loopback_t | For secure communication (read/write) with the mounted overlay in QEMU/QVP|
| datarouter_t| To use the DLT `mw::log` framework |
| LtpmDaemon_t | For the message passing communication with the LtpmDaemon |
| typed_memory_daemon_t | To acquire/create the TMD shared-memory objects in the typed memory area |
| qtsafefsd_t| For trusted filesystem connection, so used to attach the the message passing server, and creating the shared-memory objects   |

### Pathname attach

| pathname | Rationale |
|:-------------------|:--------------:|
| /mw_com/message_passing/logging.* | To use the DLT `mw::log` framework |

### Deployment User and Group IDs of the user application

The Library client, or the user app should be deployed with the following specs
:

| ID | Value | Rationale |
|:----|:--------------:|:-----------:|
| Supplementary GID (SID) | 3020 | To read `/dev/typedshm/cmd` needed for creating the TMD shared-memory objects in typed memory area |


## Suported Log/Trace points

The following tables provide an overview of the relevant/to be supported
trace-points based on the `ara::com` API of adaptive [Autosar 20.11 Standard](https://www.autosar.org/fileadmin/standards/R20-11/AP/AUTOSAR_SWS_CommunicationManagement.pdf). 
If the interface has to be adapted to 21-11 or later, then some of the trace-points may change (some may be added). 
> :speech_balloon: **Note:** The descriptions under `trace point content` are provided in an informal manner and may not reflect exact implementation details.

> ⚠️ **Warning:** : This list is intended for reference purposes only. Its completeness and accuracy cannot be guaranteed.


### Provider/Skeleton Side Trace-Points
| Trace-Point-Type <br/> ID | Name in comtrace config | ID Hex (Dec) | Description | Content (metainfo, chunk lists) |
|-|-|-|-|-|
| \<Event\>::Send(Sample&) <br> SKEL_EVENT_SND | events.<br/>trace_send | 0x00 (0) | Entry point for the copy-based event sending API. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of Sample (raw-memory, `Kaitai` enabled)                        |
| \<Event\>::Send(AllocateePtr\<Sample\>) <br/> SKEL_EVENT_SND_A| events.<br/>trace_send_allocate | 0x01 (1) | Entry point for the zero-copy event sending API entry point. Used after allocating memory with \<Event\>::Allocate() and populating it. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of Sample (raw-memory, `Kaitai` enabled) |
| \<Event\>::Subscription happened <br/> SKEL_EVENT_SUB | events.<br/>trace_subscribe_received | 0x02 (2) | This is no official API entry-point. Logical trace-point, triggered when an event subscription from a consumer is either accepted or rejected. | This is not supported in LoLa (see [here](broken_link_g/swh/safe-posix-platform/blob/main/platform/aas/mw/com/impl/tracing/configuration/tracing_filter_config_parser.cpp#L46)) |
| \<Event\>::Unsubscription happened <br/> SKEL_EVENT_UNSUB | events.<br/>trace_unsubscribe_received | 0x0A (10) | This is no official API entry-point. Logical trace-point triggered when an event-subscription from a consumer has been processed and either accepted or rejected. | This is not supported in LoLa (see [here](broken_link_g/swh/safe-posix-platform/blob/main/platform/aas/mw/com/impl/tracing/configuration/tracing_filter_config_parser.cpp#L46)) |
| \<Field\>::Update(Field&) <br/> SKEL_FIELD_UPD | fields.notifier.<br/>trace_update | 0x03 (3) | Entry point for updating field data using copy-based API. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of Field (raw-memory, `Kaitai` enabled) |
| \<Field\>::Update(AllocateePtr\<Sample\>) <br/> SKEL_FIELD_UPD_A | fields.notifier.<br/>trace_update | 0x0B (11) | Entry point for updating field data using zero-copy API. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of Field (raw-memory, `Kaitai` enabled) |
| \<Field\>::GetHandler <br/> SKEL_FIELD_GET_CALL | fields.getter.<br/>trace_request_received | 0x0C (12) | Entry point of the provider-side field get method, regardless of user-defined handlers. | |
| \<Field\>::GetHandler::Result <br/> SKEL_FIELD_GET_CALL_RESULT | fields.getter.<br/>trace_response_send | 0x0D (13) | Provider-side result emission for the field get method. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of field, which `Get()` returned (raw-memory, `Kaitai` enabled) |
| \<Field\>::SetHandler <br/> SKEL_FIELD_SET_CALL | fields.setter.<br/>trace_request_received | 0x04 (4) | Entry point of the provider-side field set method. | - metainfo: data identification id (`trace_point_data_id_`) |
| \<Field\>::SetHandler::FutureReady <br/> SKEL_FIELD_SET_CALL_RESULT | fields.setter.<br/>trace_response_send | 0x05 (5) | Provider-side emission of the field set method result after async fulfillment. | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: new value of field (raw-memory, `Kaitai` enabled) |
| \<Field\>::Subscription happened <br/> SKEL_FIELD_SUB | fields.notifier.<br/>trace_subscribe_received | 0x06 (6) | Logical trace-point triggered when a field subscription is processed (accepted or rejected). Not an official API. |  This is not supported in LoLa (see [here](broken_link_g/swh/safe-posix-platform/blob/main/platform/aas/mw/com/impl/tracing/configuration/tracing_filter_config_parser.cpp#L46)) |
| \<Field\>::Unsubscription happened <br/> SKEL_FIELD_UNSUB | fields.notifier.<br/>trace_unsubscribe_received | 0x0E (14) | Logical trace-point triggered when a field unsubscription is handled. Not an official API. | This is not supported in LoLa (see [here](broken_link_g/swh/safe-posix-platform/blob/main/platform/aas/mw/com/impl/tracing/configuration/tracing_filter_config_parser.cpp#L46)) |
| ara::core::Future\<MethodOutput\> \<Method\>(\<args\>) <br/> SKEL_METHOD_CALL | methods.<br/>trace_request_received | 0x07 (7) | Entry point of the provider-side service method implementation. | - metainfo: data identification id (`trace_point_data_id_`)  |
| ara::core::Future\<MethodOutput\>::is_ready() <br/> SKEL_METHOD_CALL_RESULT_OK | methods.<br/>trace_response_send | 0x08 (8) | Result trace-point for successful async service method completion (no application error). | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of \<MethodOutput\> (raw-memory, `Kaitai` enabled) |
| ara::core::Future\<MethodOutput\>::is_ready() <br/> SKEL_METHOD_CALL_RESULT_ERROR | methods.<br/>trace_response_send | 0x09 (9) | Result trace-point for failed async service method completion (application error occurred). | - metainfo: data identification id (`trace_point_data_id_`)<br/>- chunk list: value of \<ErrorCode\> (raw-memory, `Kaitai` enabled) |

### Consumer/Proxy Side Trace-Points
| Trace-Point-Type <br/> ID | Name in comtrace config | ID Hex (Dec) | Description | Content (metainfo, chunk lists) |
|-|-|-|-|-|
| \<Event\>::Subscribe(maxSampleCount) <br/> PROXY_EVENT_SUB                 | events.<br/>trace_subscribe_send                                 | (0x80) 128      | Event Subscribe API entry point                                                                                                                                                                                                                                                                                                                                        | - chunk list: value of `maxSampleCount` (unsigned int 16 bit, ECU/target specific endianess)                                        |
| \<Event\>::Unsubscribe() <br/> PROXY_EVENT_UNSUB                             | events.<br/>trace_unsubscribe_send                              | (0x81) 129      | Event Unsubscribe API entry point                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Event\>::Subscription State Change <br/> PROXY_EVENT_SUBSTATE_CHANGE                 | events.<br/>trace_subscription_state_changed          | (0x99) 153      | Event Subscription state changed. There is no directly assigned API call! This trace-point shall fire, when the proxy-side detects a subscription state change. Depending on implementations, it could be the case, such a state-change is only detected in the context of an API call (e.g. GetSubscriptionState()) or in case a state-change-handler was registered. | - chunk list: value new subscription state (8 bit - according to `SWS_CM_00310`[^1])                       |
| \<Event\>::SetReceiveHandler(EventReceiveHandler) <br/> PROXY_EVENT_SET_RECHDL    | events.<br/>trace_receive_handler_registered              | (0x82) 130      | Event ReceiveHandler registration API entry point                                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Event\>::EventReceiveHandler_called <br/> PROXY_EVENT_RECHDL                | events.<br/>trace_receive_handler_callback                     | (0x83) 131      | Call to registered Event ReceiveHandler happened                                                                                                                                                                                                                                                                                                                       |                                                                                                                                     |
| \<Event\>::UnsetReceiveHandler <br/> PROXY_EVENT_UNSET_RECHDL                       | events.<br/>trace_receive_handler_deregistered           | (0x84) 132      | Event ReceiveHandler de-registration API entry point                                                                                                                                                                                                                                                                                                                   |                                                                                                                                     |
| \<Event\>::SetSubscriptionStateChangeHandler <br/> PROXY_EVENT_SET_CHGHDL         | events.<br/>trace_subscription_state_change_handler_registered           | (0x85) 133      | Event SubscriptionStateChangeHandler registration API entry point                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Event\>::SubscriptionStateChangeHandler_called <br/> PROXY_EVENT_CHGHDL     | events.<br/>trace_subscription_state_change_handler_callback                 | (0x86) 134      | Call to registered Event SubscriptionStateChangeHandler happened                                                                                                                                                                                                                                                                                                       | - chunk list: value new subscription state (8 bit - according to [SWS_CM_00310](#autosar-subscription-state))                       |
| \<Event\>::UnsetSubscriptionStateChangeHandler <br/> PROXY_EVENT_UNSET_CHGHDL       | events.<br/>trace_subscription_state_change_handler_deregistered        | (0x9A) 154      | Event SubscriptionStateChangeHandler registration API entry point                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Event\>::GetNewSamples(maxSamples, F) <br/> PROXY_EVENT_GET_SAMPLES              | events.<br/>trace_get_new_samples                        | (0x87) 135      | Event GetNewSamples API entry point                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                     |
| \<Event\>::GetNewSamples::F(SamplePtr) <br/> PROXY_EVENT_SAMPLE_CB               | events.<br/>trace_get_new_samples_callback                 | (0x88) 136      | Call to given GetNewSamples callback F happened                                                                                                                                                                                                                                                                                                                        | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |
| \<Field\>::Subscribe(maxSampleCount) <br/> PROXY_FIELD_SUB                 | fields.notifier.<br/>trace_subscribe_send                              | (0x89) 137      | Field Subscribe API entry point                                                                                                                                                                                                                                                                                                                                        | - chunk list: value of `maxSampleCount` (unsigned int 16 bit, ECU/target specific endianess)                                        |
| \<Field\>::Unsubscribe() <br/> PROXY_FIELD_UNSUB                             | fields.notifier.<br/>trace_unsubscribe_send                            | (0x8A) 138      | Field Unsubscribe API entry point                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Field\>::Subscription State Change <br/> PROXY_FIELD_SUBSTATE_CHANGE                 | fields.notifier.<br/>trace_subscription_state_changed        | (0x9B) 155      | Field Subscription state changed. There is no directly assigned API call! This trace-point shall fire, when the proxy-side detects a subscription state change. Depending on implementations, it could be the case, such a state-change is only detected in the context of an API call (e.g. GetSubscriptionState()) or in case a state-change-handler was registered. | - chunk list: value new subscription state (8 bit - according to [SWS_CM_00310](#autosar-subscription-state))                       |
| \<Field\>::SetReceiveHandler(EventReceiveHandler) <br/> PROXY_FIELD_SET_RECHDL     | fields.notifier.<br/>trace_receive_handler_registered           | (0x8B) 139      | Field ReceiveHandler registration API entry point                                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Field\>::EventReceiveHandler_called <br/> PROXY_FIELD_RECHDL                | fields.notifier.<br/>trace_receive_handler_callback                | (0x8C) 140      | Call to registered Field ReceiveHandler happened                                                                                                                                                                                                                                                                                                                       |                                                                                                                                     |
| \<Field\>::UnsetReceiveHandler() <br/> PROXY_FIELD_UNSET_RECHDL                     | fields.notifier.<br/>trace_receive_handler_deregistered       | (0x8D) 141      | Field ReceiveHandler de-registration API entry point                                                                                                                                                                                                                                                                                                                   |                                                                                                                                     |
| \<Field\>::SetSubscriptionStateChangeHandler() <br/> PROXY_FIELD_SET_CHGHDL       | fields.notifier.<br/>trace_subscription_state_change_handler_registered          | (0x8E) 142      | Field SubscriptionStateChangeHandler registration API entry point                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Field\>::SubscriptionStateChangeHandler_called <br/> PROXY_FIELD_CHGHDL     | fields.notifier.<br/>trace_subscription_state_change_handler_callback              | (0x8F) 143      | Call to registered Field SubscriptionStateChangeHandler happened                                                                                                                                                                                                                                                                                                       | - chunk list: value new subscription state (8 bit - according to [SWS_CM_00310](#autosar-subscription-state))                       |
| \<Field\>::UnsetSubscriptionStateChangeHandler() <br/> PROXY_FIELD_UNSET_CHGHDL     | fields.notifier.<br/>trace_subscription_state_change_handler_deregistered        | (0x9C) 156      | Field SubscriptionStateChangeHandler registration API entry point                                                                                                                                                                                                                                                                                                      |                                                                                                                                     |
| \<Field\>::GetNewSamples(maxSamples, F) <br/> PROXY_FIELD_GET_SAMPLES              | fields.notifier.<br/>trace_get_new_samples                       | (0x90) 144      | Field GetNewSamples API entry point                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                     |
| \<Field\>::GetNewSamples::F(SamplePtr) <br/> PROXY_FIELD_SAMPLE_CB               | fields.notifier.<br/>trace_get_new_samples_callback                | (0x91) 145      | Call to given GetNewSamples callback F happened                                                                                                                                                                                                                                                                                                                        | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |
| \<Field\>::Set(fieldValue) <br/> PROXY_FIELD_SET                           | fields.setter.<br/>trace_request_send                                | (0x92) 146      | Field Set API entry point                                                                                                                                                                                                                                                                                                                                              | - metainfo: data identification id (`trace_point_data_id_`)<br/>chunk list: value of Field (raw-memory, `Kaitai` enabled)           |
| ara::core::Future\<FieldType\>::Set::is_ready() <br/> PROXY_FIELD_SET_RESULT      | fields.setter.<br/>trace_response_received                      | (0x93) 147      | Consumer side async promise fulfillment finalizing the field set call. The result of the returned future gets available/future is ready.                                                                                                                                                                                                                               | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |
| \<Field\>::Get() <br/> PROXY_FIELD_GET                                     | fields.getter.<br/>trace_request_send                                 | (0x94) 148      | Field Get API entry point                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                     |
| ara::core::Future\<FieldType\>::Get::is_ready() <br/> PROXY_FIELD_GET_RESULT      | fields.getter.<br/>trace_response_received                     | (0x95) 149      | Consumer side async promise fulfillment finalizing the field get call. The result of the returned future gets available/future is ready.                                                                                                                                                                                                                               | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |
| ara::core::Future\<MethodOutput\> \<Method\>(\<args\>) <br/> PROXY_METHOD_CALL | methods.<br/>trace_request_send                         | (0x96) 150      |                                                                                                                                                                                                                                                                                                                                                                        | - metainfo: data identification id (`trace_point_data_id_`)<br/>chunk list: value of \<MethodInput\> (raw-memory, `Kaitai` enabled) |
| ara::core::Future\<MethodOutput\>::is_ready() <br/> PROXY_METHOD_CALL_RESULT_OK         | methods.<br/>trace_response_received           | (0x97) 151      |                                                                                                                                                                                                                                                                                                                                                                        | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |
| ara::core::Future\<MethodOutput\>::is_ready() <br/> PROXY_METHOD_CALL_RESULT_ERROR        | methods.<br/>trace_response_received      | (0x98) 152      |                                                                                                                                                                                                                                                                                                                                                                        | - metainfo: data identification id (`trace_point_data_id_`)                                                                         |


### Format of content in Trace-Points
The content to be traced per trace-point type is given in the corresponding
column in the tables above. Trace-points can have the following variations:
- no specific content: This means only the mandatory properties in the meta-info
are set. No optional properties in meta-info and no chunk-list (local or shared)
is given
- specific content only in meta-info
- specific content only in chunk-list
- specific content in meta-info and chunk-list.
  
#### Content in chunk lists
The content in chunk lists is in binary format. If the content reflects "user
data", i.e. event-, field-, service-method payloads, it is the binary in memory
representation of the underlying (C++) data-type. To enable
interpretation/processing in the backend, for those types `Katai` descriptions
of the binary representation are expected to be created/exported at build time.
If the content reflects "control-info" data - that is data of `ara::com` control
APIs like (`Subscribe` or `SubscriptionStateChanges`), then the exact binary
representation is given here! No need for a structural `Kaitai` documentation.

#### SubscribeInfo struct
The complex type for `SubscribeInfo`, which is used in some subscription related
trace-point types shall conform to a struct without any padding/alignment in the
form of:
```
struct __attribute__((packed)) SubscribeInfo
{
  std::uint16_t max_sample_count;
  std::uint8_t subscription_result;
}
```
unsigned integer values shall be in ECU/target specific endianess
representation. `subscription_result` shall be the representation of a boolean
(0 = False/Failure, 1 = True/Success).

### Configuration Flexibility

The concrete trace-points are configurable via a `comtrace_config` file. 
The schema for this file is located [here](../config/schema/comtrace_config_schema.json). 
For more information on how the `ara::com` configuration works, please refer to the IPC framework documentation. 
For `mw::com` the design documentation is located in the [ipc_tracing](broken_link_g/swh/safe-posix-platform/blob/main/platform/aas/mw/com/design/ipc_tracing/README.md#trace-filter-config) folder:  

[^1]: SubscriptionState is described in AUTOSAR Req. `SWS_CM_00310` in SWS Communication Management 22-11
