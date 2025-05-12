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
def cc_unit_test(name, **kwargs):
    """
    Macro in order to declare a C++ unit test

    Args:
      name: Target name, to be forwarded to cc_test
      **kwargs: Additional parameters to be forwarded to cc_test. size and timeout cannot be provided and if tags is
        provided, it should not contain the tag unit. The unit tag is added automatically.

    Wrapper to create a cc_test that provides the unit tag and it forces the size and the timeout of the test
    This makes sure that all unit tests are small and short. If for whatever reason you need that, do not think about
    changing this macro to provide an override. Instead, use directly cc_test.

    There are two parts that are not provided intentionally:
    - toolchain features for compiler warnings still need to be provided. The reason why it is not provided is
    to prevent that from the outside it looks like for some target we provide and some not. We can only do that
    if we have this option for the other cc_library targets.
    - dependencies for gtest. The reason is that this is not called cc_gtest_unit_test but generic, that is why for the
    moment we keep it like that. If you want to have gtest dependencies consider using cc_gtest_unit_test.
    """
    kwargs.setdefault("tags", [])
    if "unit" in kwargs["tags"]:
        fail("'unit' tag already provided, please refrain from adding it manually.")

    kwargs["tags"].append("unit")
    native.cc_test(
        name = name,
        size = "small",
        timeout = "short",
        **kwargs
    )

def cc_gtest_unit_test(name, **kwargs):
    """
    Macro in order to declare a C++ unit test

    Args:
      name: Target name, to be forwarded to cc_unit_test and transitively to cc_test

      **kwargs: Additional parameters to be forwarded to cc_unit_test and transitively to cc_test. size and timeout
      cannot be provided and if tags is provided, it should not contain the tag unit.
      The following dependencies are already added to deps:
      "@score-baselibs//score/language/safecpp/coverage_termination_handler", and "@googletest//:gtest_main".
      The following toolchain features are already added to features: "aborts_upon_exception".

    Wrapper to create a cc_unit_test that provides the most common dependencies to be used in a unit test of an ASIL B
    application. If for whatever reason you need to have some other common dependency or you cannot follow the restrictions
    of cc_unit_test, do not think about changing this macro to provide an override. Instead, use directly cc_test.

    Like with cc_unit_test, toolchain features for compiler warnings still need to be provided. The reason why it is not
    provided is to prevent that from the outside it looks like for some target we provide and some not. We can only do that
    if we have this option for the other cc_library targets.
    """

    # Avoid mutating input arguments
    deps = [
        "@score-baselibs//score/language/safecpp/coverage_termination_handler",
        "@googletest//:gtest_main",
    ]
    deps += kwargs.pop("deps", [])

    features = [
        "aborts_upon_exception",
    ]
    features += kwargs.pop("features", [])

    cc_unit_test(
        name = name,
        features = features,
        deps = deps,
        **kwargs
    )
