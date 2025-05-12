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
load("//platform/aas/bazel/generators:arxmlgen.bzl", "arxmlgen_converter")

# macro for franca_to_arxml converter
def franca2arxml_converter(
        name,
        srcs,
        app_config_files = None,
        deploy_elem = None,  # buildifier: disable=unused-variable # obsolete, will be removed in near future
        ecu_name = "BMW.Root_Ecu",
        ignore_missing_elements = None,
        generate_services = [],
        available_plugins = [],
        plugin_srcs = [],
        plugin_deps = [],
        plugin_inputs = [],
        plugin_outputs = [],
        plugin_args = [],
        generate_standalone_services = [],
        dext_for_app = True,
        use_francapp = False,
        arxml_1903_version = False,
        # paramconf_only = False,
        # only_interface = False,
        allow_missing_model_files = False,
        design_input_only = False,
        very_verbose = False,
        exec_properties = {},
        **kwargs):
    """
    Generate a ARXML from specified Franca files

    This rule calls the Franca2ARXML Tool. It will basically create one ARXML that contains
    all elements that were specified within the Franca files. This ARXML can the be used
    as input Artifact for other generation steps like the `cc_amsrgen_library` or the `cc_aragen_library` rules.

    Example:
    ```python
    franca2arxml_converter(
        name = "example_franca2_arxml",
        srcs = glob(["franca/**/*"]),
        generate_services = ["paramServerComponent"]
        ecu_name = "Ecu1",
    )
    ```

    Args:
      name: target name of the library
      srcs: List of labels that specify the Franca files used for the generation step
      app_config_files:
      deploy_elem:
      ecu_name: String of the ECU to generate ARXML for
      ignore_missing_elements: If set to true it will not fail when having unresolved references
      generate_services: is a list of services, for which autosar model will be generated, [] means for all
      available_plugins: is a list of plugins to be imported by the generator
      plugin_srcs:
      plugin_deps:
      plugin_inputs:
      plugin_outputs:
      plugin_args:
      generate_standalone_services:
      dext_for_app:
      arxml_1903_version:
      paramconf_only:
      only_interface:
      allow_missing_model_files:
      design_input_only:
      very_verbose:
      exec_properties: See Bazel Documentation about exec_properties
      **kwargs:
    """

    # config transition:
    if generate_services or generate_standalone_services:
        pass

    if ignore_missing_elements:
        pass

    if arxml_1903_version or use_francapp:
        pass

    if available_plugins or plugin_srcs or plugin_deps or plugin_inputs or plugin_outputs or plugin_args:
        pass

    arxmlgen_converter(
        name = name,
        srcs = srcs,
        app_config_files = app_config_files,
        dext_for_app = dext_for_app,
        ecu_name = ecu_name,
        design_input_only = design_input_only,
        allow_missing_model_files = allow_missing_model_files,
        log_level = "DEBUG" if very_verbose else "ERROR",
        exec_properties = exec_properties,
        **kwargs
    )
