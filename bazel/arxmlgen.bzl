# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************
NAMESPACE_ALIAS = [
    "ara.diag.service_interfaces.DiagnosticConversation=ara.diag.service_interfaces.diagnostic_conversation",
    "ara.diag.DiagnosticEvent=ara.diag.service_interfaces.diagnostic_event",
    "ara.diag.DiagnosticMonitor=ara.diag.service_interfaces.diagnostic_monitor",
    "ara.diag.service_interfaces.DataElementBoolean=ara.diag.service_interfaces.data_element_boolean",
    "ara.diag.GenericUDSService=ara.diag.service_interfaces.generic_uds_service",
    "ara.diag.service_interfaces.SecurityAccess=ara.diag.service_interfaces.security_access",
    "ara.diag.service_interfaces.ResetMediator=ara.diag.service_interfaces.reset_mediator",
    "ara.diag.service_interfaces.VINInformation=ara.diag.service_interfaces.vin_information",
    "ara.diag.service_interfaces.ServiceValidation=ara.diag.service_interfaces.service_validation",
    "ara.diag.service_interfaces.DM_IPC=ara.diag.service_interfaces.dm_ipc",
    "ara.diag.service_interfaces.EnableCondition=ara.diag.service_interfaces.enable_condition",
    "ara.diag.DTCInformation=ara.diag.service_interfaces.dtc_information",
    "ara.diag.DiagnosticEventMemory=ara.diag.service_interfaces.diagnostic_event_memory",
    "ara.diag.service_interfaces.DiagnosticServer=ara.diag.service_interfaces.diagnostic_server",
    "ara.diag.service_interfaces.OperationCycle=ara.diag.service_interfaces.operation_cycle",
    "ara.diag.service_interfaces.StorageCondition=ara.diag.service_interfaces.storage_condition",
    "ara.diag.service_interfaces.DataElement_boolean=ara.diag.service_interfaces.data_element_boolean",
    "ara.diag.service_interfaces.DataElement_vector=ara.diag.service_interfaces.data_element_boolean",
    "ara.diag.service_interfaces.DataElement_sint8=ara.diag.service_interfaces.data_element_sint8",
    "ara.diag.service_interfaces.DataElement_sint16=ara.diag.service_interfaces.data_element_sint16",
    "ara.diag.service_interfaces.DataElement_sint32=ara.diag.service_interfaces.data_element_sint32",
    "ara.diag.service_interfaces.DataElement_sint64=ara.diag.service_interfaces.data_element_sint64",
    "ara.diag.service_interfaces.DataElement_uint8=ara.diag.service_interfaces.data_element_uint8",
    "ara.diag.service_interfaces.DataElement_uint16=ara.diag.service_interfaces.data_element_uint16",
    "ara.diag.service_interfaces.DataElement_uint32=ara.diag.service_interfaces.data_element_uint32",
    "ara.diag.service_interfaces.DataElement_uint64=ara.diag.service_interfaces.data_element_uint64",
    "ara.diag.service_interfaces.DataElement_vector=ara.diag.service_interfaces.data_element_vector",
    "ara.diag.service_interfaces.ClearCondition=ara.diag.service_interfaces.clear_condition",
    "ara.diag.service_interfaces.SuppressCondition=ara.diag.service_interfaces.suppress_condition",
    "ara.nm.channel.NmChannel=ara.nm.channel",
    "ara.nm.^channel.NmChannel=ara.nm.channel",
    "ara.nm.NetworkManagement=ara.nm",
    "ara.nm.NetworkState=ara.nm",
    "amsr.nm.PncState=amsr.nm",
]

OLDSTYLE_NAMESPACE_EVENT_TYPES = []

NEWSTYLE_NAMESPACE_EVENT_TYPES = [
    "cwheartbeat.counter.fcwdata.fcwHeartbeatEvtType",
    "bmw.ipnext.cvdn.cv_minion.v1_0.FcwData.fcwHeartbeat",
    "bmw.ipnext.cvdn.cv_minion.FcwData.fcwHeartbeat",
    "y24cw25.Eth4.BMW.DASS.L3StatusInternalAsilD.oddEvents",
    "y24cw25.Eth4.BMW.DASS.L3StatusInternalAsilB.oddEvents",
    "y24cw25.Eth4.BMW.DASS.L3StatusInternalQm.oddEvents",
    "BMW.interface1.Service1.event1",
    "BMW.interface1.Service2.event1",
    "y24cw33.Eth4.BMW.DASS.DisplayDriverAssistanceFunctions.quickInfoAssistedDriving",
    "y24cw33.Eth4.BMW.DASS.DrivingStrategyTargetValues.requestInteriorLightEmergencyStop",
    "y24cw33.Eth4.BMW.INFOTAINMENT.PERSONALIZATION.AccountControllerBasic.accountsUpdated",
    "y24cw33.Eth4.BMW.INFOTAINMENT.PERSONALIZATION.AccountControllerBasic.sessionsUpdated",
    "y24cw33.Eth4.BMW.BODY.DigitalKey.requestFunctionStatus",
    "y24cw33.Eth4.BMW.BODY.DigitalKey.returnRemainingPinInputRetries",
    "y24cw33.Eth4.BMW.BODY.DigitalKey.updateBackendData",
    "y24cw33.Eth4.BMW.INFRASTRUCTURE.VisionAccess2.warningAutomaticDoorsDriverDoor",
    "y24cw33.Eth4.BMW.INFRASTRUCTURE.VisionAccess2.warningAutomaticDoorsDriverDoorRear",
    "y24cw33.Eth4.BMW.INFRASTRUCTURE.VisionAccess2.warningAutomaticDoorsPassengerDoor",
    "y24cw33.Eth4.BMW.INFRASTRUCTURE.VisionAccess2.warningAutomaticDoorsPassengerDoorRear",
    "y24cw43.Eth4.*",
    "y25cw09.Eth4.*",
]

def _arxmlgen_impl(ctx):
    args = ctx.actions.args()
    json_config = dict()

    json_config["namespace_alias"] = ctx.attr.namespace_alias

    json_config["old-style-namespace-event-types"] = ctx.attr.old_style_namespace_event_types

    json_config["new-style-namespace-event-types"] = ctx.attr.new_style_namespace_event_types

    args.add("--old-style-event-type-namespace")  # Default case is to generate event types in old style

    args.add("--ecu_name", ctx.attr.ecu_name)

    if ctx.attr.design_input_only:
        args.add("--design_input_only")
    if ctx.attr.dext_for_app:
        args.add("--dext_for_app")
    if ctx.attr.allow_missing_model_files:
        args.add("--allow_missing_model_files")

    #define src files (franca model files)
    json_config["model"] = sorted([src.path for src in ctx.files.srcs])

    json_config_file = ctx.actions.declare_file(ctx.attr.name + "_arxmlgen_config.json")
    ctx.actions.write(
        output = json_config_file,
        content = json.encode_indent(json_config, indent = "    ") + "\n",
    )
    args.add("--json_config", json_config_file.path)

    args.add("--output_path", ctx.outputs.main_arxml_file)
    args.add("--dext_out_path", ctx.outputs.dext_out_arxml_file)
    args.add("--dext_app_path", ctx.outputs.dext_app_arxml_file)
    args.add("--paramconf_path", ctx.outputs.std_diag_arxml_file)

    for app_config in ctx.files.app_config_files:
        args.add("--app_config_files")
        args.add(app_config)

    args.add("--log_level", ctx.attr.log_level)

    # To use Franca files from external sources it is necessary to tell arxmlgen
    # which are additional prefixes to be added to the import paths for the includes. In this use case,
    # arxmlgen adds the prefix provided to the import path, if a file cannot be found.
    # This use case is especially needed for BNE upgrades. In this case, arxmlgen will try to load
    # files also from the extended path, if a file could not be found.
    substitution_paths = []
    valid_prefixes = [
        {"depth": 1, "prefix": "external/"},  # external dependencies
        {"depth": 3, "prefix": "bazel-out/"},  # generated files (e.g. bazel-out/x86_64-fastbuild/bin/)
    ]
    for src in ctx.files.srcs:
        for prefix in valid_prefixes:
            if src.path[:len(prefix["prefix"])] == prefix["prefix"]:
                folders = src.path.split("/")

                # Only the first <depth> folders are relevant and will be used as prefix by arxmlgen
                relevant_path = "/".join(folders[:prefix["depth"]])
                if relevant_path not in substitution_paths:
                    substitution_paths.append(relevant_path)

    args.add_all(substitution_paths, before_each = "-m")

    executable = ctx.executable._arxmlgen

    ctx.actions.run(
        inputs = ctx.files.srcs + ctx.files.deps + ctx.files.app_config_files + [json_config_file],
        outputs = [ctx.outputs.main_arxml_file, ctx.outputs.dext_out_arxml_file, ctx.outputs.dext_app_arxml_file, ctx.outputs.std_diag_arxml_file],
        arguments = [args],
        mnemonic = "arxmlgen",
        progress_message = "Transform Franca IDL model into adaptive ARXML {}".format(ctx.label),
        executable = executable,
    )

arxmlgen = rule(
    implementation = _arxmlgen_impl,
    attrs = {
        "allow_missing_model_files": attr.bool(default = False),
        "app_config_files": attr.label_list(allow_files = True),
        "deps": attr.label_list(allow_files = True),
        "design_input_only": attr.bool(default = False),
        "dext_app_arxml_file": attr.output(),
        "dext_for_app": attr.bool(default = True),
        "dext_out_arxml_file": attr.output(),
        "ecu_name": attr.string(default = "BMW.Root_Ecu"),
        "log_level": attr.string(default = "DEBUG"),
        "main_arxml_file": attr.output(),
        "namespace_alias": attr.string_list(
            default = NAMESPACE_ALIAS,
        ),
        "new_style_namespace_event_types": attr.string_list(default = NEWSTYLE_NAMESPACE_EVENT_TYPES),
        "old_style_namespace_event_types": attr.string_list(default = OLDSTYLE_NAMESPACE_EVENT_TYPES),
        "srcs": attr.label_list(allow_files = True),
        "std_diag_arxml_file": attr.output(),
        "_arxmlgen": attr.label(
            default = Label("//platform/aas/tools/arxmlgen/code:arxmlgen"),
            cfg = "exec",
            executable = True,
            allow_files = True,
        ),
    },
)

# macro for franca_to_arxml converter
def arxmlgen_converter(
        name,
        srcs,
        deps = [],
        app_config_files = None,
        dext_for_app = True,
        ecu_name = "BMW.Root_Ecu",
        design_input_only = False,
        allow_missing_model_files = False,
        log_level = "ERROR",
        exec_properties = {},
        **kwargs):
    """
    Generate a ARXML from specified Franca files

    This rule calls the arxmlgen Tool. It will basically create one ARXML that contains
    all elements that are specified within the Franca files. This ARXML can be used
    as input Artifact for other generation steps like the `cc_amsrgen_library` or the `cc_aragen_library` rules.

    Example:
    ```python
    arxmlgen_converter(
        name = "example_arxml",
        srcs = glob(["franca/**/*"]),
        deps = [
            "//platform/aas/tools/franca/deployment_specifications",
        ],
        log_level = "DEBUG",
    )
    ```

    Args:
        name: target name of the library
        srcs: List of labels that specify the Franca files used for the generation step
        deps: List of labels that specify the Franca model files dependencies
        dext_for_app: default value is True
        ecu_name: default value is 'BMW.Root_Ecu'
        design_input_only: default value is False
        allow_missing_model_files: allow import file is missing
        log_level: value within {DEBUG, INFO, WARNING, ERROR, CRITICAL}, default value is ERROR
        exec_properties: See Bazel Documentation about exec_properties
        **kwargs:
    """

    arxml_file_pattern = "{}_franca2arxml/gen/{}.arxml"
    main_arxml_file = arxml_file_pattern.format(name, name)
    dext_out_arxml_file = arxml_file_pattern.format(name, "dext_out")
    dext_app_arxml_file = arxml_file_pattern.format(name, "dext_app" if dext_for_app else "dext_bolo")
    std_diag_arxml_file = arxml_file_pattern.format(name, "StdDiag_paramconf")

    franca_grammar_lark_files = "//platform/aas/tools/franca_parser_lark/code/franca_grammar:lark_files"
    if franca_grammar_lark_files not in deps:
        deps = deps + [franca_grammar_lark_files]

    #start generator
    arxmlgen(
        name = name,
        srcs = srcs,
        deps = deps,
        app_config_files = app_config_files,
        main_arxml_file = main_arxml_file,
        dext_out_arxml_file = dext_out_arxml_file,
        dext_app_arxml_file = dext_app_arxml_file,
        std_diag_arxml_file = std_diag_arxml_file,
        dext_for_app = dext_for_app,
        ecu_name = ecu_name,
        design_input_only = design_input_only,
        allow_missing_model_files = allow_missing_model_files,
        log_level = log_level,
        exec_properties = exec_properties,
        **kwargs
    )
