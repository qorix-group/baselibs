load("//platform/aas/bazel/generators:aragen_tree.bzl", "cc_mw_com_library", "mw_artifacts")
load("//platform/aas/bazel/generators:franca2arxml.bzl", "franca2arxml_converter")

def cc_middleware_com_frontend(
        name,
        srcs,
        target_compatible_with = [],
        **kwargs):
    """
    Marco to generate LoLa-Frontend from Franca-Files.

    This macro reduces duplication in Bazel Rules. It takes Franca as input and generates a cc_library that
    applications can link and build against. It is exclusively used to generate a LoLa (mw::com) frontend.
    """

    franca2arxml_converter(
        name = "{}_arxml".format(name),
        srcs = srcs,
        allow_missing_model_files = True,
        design_input_only = True,
        target_compatible_with = target_compatible_with,
    )

    mw_artifacts(
        name = "{}_mw_artifacts".format(name),
        arxmls = [
            ":{}_arxml".format(name),
            "//platform/aas/meta/datatypes:stdtypes",
        ],
        target_compatible_with = target_compatible_with,
    )

    cc_mw_com_library(
        name = name,
        mw_artifacts = ":{}_mw_artifacts".format(name),
        target_compatible_with = target_compatible_with,
        **kwargs
    )
