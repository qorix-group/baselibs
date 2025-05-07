load("@bazel_skylib//lib:paths.bzl", "paths")

def _aragen_gen_impl(ctx):
    run_outputs = ctx.actions.declare_directory(ctx.attr.name)

    outdir = ctx.bin_dir.path
    if ctx.label.workspace_root:
        outdir += "/" + ctx.label.workspace_root
    outdir += "/" + ctx.label.package

    args = ctx.actions.args()
    args.add("-o", outdir + "/" + ctx.attr.name)
    args.add("--frontend")
    args.add("--log-file", ctx.attr.name + "_aragen.log")
    args.add("--release", ctx.attr.release)

    args.add_all(ctx.files.arxmls)

    args.add("--exact-mode")

    if ctx.attr.print_debug:
        args.add("--print-debug")

    args.add("--orig-types-handling")

    ctx.actions.run(
        inputs = ctx.files.arxmls,
        outputs = [run_outputs],
        arguments = [args],
        mnemonic = "aragen",
        progress_message = "Generating " + ctx.label.name,
        executable = ctx.executable.aragen,
    )

    return [DefaultInfo(files = depset(direct = [run_outputs]))]

_aragen_gen = rule(
    implementation = _aragen_gen_impl,
    attrs = {
        "aragen": attr.label(
            cfg = "exec",
            executable = True,
            allow_files = False,
            default = # "@ddad//platform/aas/tools/aragen:aragen",
        ),
        "arxmls": attr.label_list(allow_files = True),
        "print_debug": attr.bool(
            mandatory = False,
            default = False,
        ),
        "release": attr.string(
            mandatory = True,
        ),
    },
)

def _filter_hdrs_impl(ctx):
    # The directory has to be declared with a file extension, otherwise cc_library would reject them as inputs
    includes = ctx.actions.declare_directory("%s/%s/includes.h" % (ctx.attr.name, ctx.attr.application_name))

    ctx.actions.run_shell(
        inputs = [ctx.file.mw_artifacts],
        outputs = [includes],
        mnemonic = "filter",
        progress_message = "FILTERING " + ctx.label.name,
        command = "mkdir -p %s; cp -rL %s/%s/includes/* %s || true" % (includes.path, ctx.file.mw_artifacts.path, ctx.attr.application_name, includes.path),
        use_default_shell_env = True,
    )

    return DefaultInfo(files = depset(direct = [includes]))

_filter_hdrs = rule(
    implementation = _filter_hdrs_impl,
    attrs = {
        "application_name": attr.string(mandatory = True),
        "mw_artifacts": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
    },
)

def mw_artifacts(
        name,
        arxmls,
        aragen = # "@ddad//platform/aas/tools/aragen:aragen",
        release = "20-11",
        **kwargs):
    """
    Creates mw::com artifacts (.h/.cpp/*.json).

    This rule calls the aragen with the specified ARXMLs to generate the LoLa-Binding for ara::com.
    It will generate the bindings for ALL software applications that are specified in the ARXMLs,
    in individual folders. Therefore, this target only needs to be created (and run) once for each
    set of ARXMLs.

    After using the mw_artifacts() rule, the cc_mw_com_library() can be used to create a cpp library
    for the individual software component.

    Args:
      name: Target name
      arxmls: List of labels that specify the ARXMLs used for the generation step
      aragen: Aragen to be used, most probably you do not want to change it
      release: Autosar release version, if not specified default '20-11' is in use
      **kwargs: Additional parameters to be forwarded
    """

    _aragen_gen(
        name = name,
        arxmls = arxmls + ["//ecu/xpad/ara/diag:diagnostic_manager_interfaces_custom"],
        aragen = aragen,
        release = release,
        **kwargs
    )

def cc_mw_com_library(
        name,
        mw_artifacts,
        **kwargs):
    """
    Build LoLa binding as cc_library for one software component

    This rule builds an LoLa binding cpp library for an individual software component.
    It can then be referenced like a regular `cc_library`, meaning that all parameters
    of a `cc_library` (e.g `copts`) can be used.

    The input of this rule is an mw_artifacts() target and an application name. The rule will then extract
    the necessary code for the application from the mw_artifacts() target, build the ROS messages/services
    for it, and then create a cc_library.

    Args:
        name: Target name of the library
        mw_artifacts: Label to the corresponding mw_artifacts() target
        **kwargs: Additional parameters to be forwarded
    """
    hdrs_filter = "{}_hdr_files".format(name)

    deps = [
        # "@ddad@score-baselibs//score/memory/shared:types",
        # "@ddad//platform/aas/mw/com",
    ]

    kwargs.setdefault("tags", [])

    _filter_hdrs(
        name = hdrs_filter,
        mw_artifacts = mw_artifacts,
        application_name = "",  # we don't group by applications
        tags = kwargs["tags"],
    )

    native.cc_library(
        name = name,
        hdrs = [hdrs_filter],
        includes = [
            "{}/includes.h".format(hdrs_filter),
        ],
        deps = deps,
        alwayslink = 1,
        **kwargs
    )

def _filegroup_filter_impl(ctx):
    base_dir = paths.join(
        ctx.bin_dir.path,
        ctx.label.package,
        ctx.attr.name,
    )

    output = []
    cmd = "mkdir -p {};".format(base_dir)

    # Build shell copy command for each file individually
    for file in ctx.attr.files:
        filename = file[file.rfind(("/")) + 1:]
        outfile = "{}/{}".format(ctx.attr.name, filename)
        output.append(ctx.actions.declare_file(outfile))
        cmd += "cp -L {}/{} {};".format(ctx.file.mw_artifacts.path, file, base_dir)

    ctx.actions.run_shell(
        inputs = [ctx.file.mw_artifacts],
        outputs = output,
        mnemonic = "filter",
        progress_message = "FILTER " + ctx.label.name,
        command = cmd,
        use_default_shell_env = True,
    )

    return [DefaultInfo(files = depset(direct = output))]

_filegroup_filter = rule(
    implementation = _filegroup_filter_impl,
    attrs = {
        "files": attr.string_list(
            mandatory = True,
            allow_empty = False,
        ),
        "mw_artifacts": attr.label(allow_single_file = True),
    },
)

def filegroup_mw_artifacts(
        name,
        mw_artifacts,
        artifacts,
        **kwargs):
    """
    Build a filegroup for generated aragen artifacts

    This rule allows to create a filegroup for generated aragen artifacts. The files that are
    supposed to be included in this filegroup, must be listed via the `artifacts` parameter.

    Args:
        name: Target name of the library
        mw_artifacts: Label to the corresponding mw_artifacts() target
        artifacts: List of the artifacts you wish to generate
        **kwargs: Additional parameters to be forwarded
    """
    filter_files = "{}_ros_files".format(name)

    # Only provide needed files in sandbox and check that they are also created
    _filegroup_filter(
        name = filter_files,
        mw_artifacts = mw_artifacts,
        files = artifacts,
        tags = kwargs.get("tags", []),
    )

    native.filegroup(
        name = name,
        srcs = [filter_files],
        **kwargs
    )

######################### MOCK GENERATOR ##############################

def _aragen_gen_mock_impl(ctx):
    run_outputs = ctx.actions.declare_directory(ctx.attr.name + ".cc")

    args = ctx.actions.args()
    args.add("-o", run_outputs.path)
    args.add("--frontend")
    args.add("--log-file", ctx.attr.name + "_aragen.log")
    args.add("--release", ctx.attr.release)

    args.add_all(ctx.files.arxmls)

    if ctx.attr.exact_mode:
        args.add("--exact-mode")

    if ctx.attr.print_debug:
        args.add("--print-debug")

    args.add("--mock-aragen")
    args.add("--no-arxml-validation")

    ctx.actions.run(
        inputs = ctx.files.arxmls,
        outputs = [run_outputs],
        arguments = [args],
        mnemonic = "aragen",
        progress_message = "Generating " + ctx.label.name,
        executable = ctx.executable.aragen,
        execution_requirements = {"no-cache": ""},
    )

    return [DefaultInfo(files = depset(direct = [run_outputs]))]

ara_mock_artifacts = rule(
    implementation = _aragen_gen_mock_impl,
    attrs = {
        "aragen": attr.label(
            cfg = "exec",
            executable = True,
            allow_files = False,
            default = # "@ddad//platform/aas/tools/aragen:aragen",
        ),
        "arxmls": attr.label_list(allow_files = True),
        "exact_mode": attr.bool(
            mandatory = False,
            default = False,
        ),
        "print_debug": attr.bool(
            mandatory = False,
            default = False,
        ),
        "release": attr.string(
            default = "19-03",
        ),
    },
)

def _filter_srcs_mock_impl(ctx):
    # The directories have to be declared with a file extension, otherwise cc_library would reject them as inputs
    src_files = ctx.actions.declare_directory("%s/src.cc" % (ctx.attr.name))

    cmd = ('''
        mkdir -p %s;
        [ ! -d %s/%s ] && true || find %s/%s -name \"*.cc\" -exec sh -c 'mkdir -p "$(dirname `echo {} | sed 's#%s/%s#%s#'`)" && cp -RL {} `echo {} | sed 's#%s/%s#%s#'`' \\;
    ''') % (src_files.path, ctx.file.ara_mock_artifacts.path, "includes", ctx.file.ara_mock_artifacts.path, "includes", ctx.file.ara_mock_artifacts.path, "includes", src_files.path, ctx.file.ara_mock_artifacts.path, "includes", src_files.path)

    ctx.actions.run_shell(
        inputs = [ctx.file.ara_mock_artifacts],
        outputs = [src_files],
        mnemonic = "filter",
        progress_message = "FILTER " + ctx.label.name,
        command = cmd,
        use_default_shell_env = True,
    )

    return DefaultInfo(files = depset([src_files]))

_filter_srcs_mock = rule(
    implementation = _filter_srcs_mock_impl,
    attrs = {
        "ara_mock_artifacts": attr.label(allow_single_file = True),
    },
)

def _filter_hdrs_mock_impl(ctx):
    # The directories have to be declared with a file extension, otherwise cc_library would reject them as inputs
    includes = ctx.actions.declare_directory("%s/includes.h" % (ctx.attr.name))

    cmd = ('''
        mkdir -p %s;
        [ ! -d %s/%s ] && true || find %s/%s -name \"*.h\" -exec sh -c 'mkdir -p "$(dirname `echo {} | sed 's#%s/%s#%s#'`)" && cp -RL {} `echo {} | sed 's#%s/%s#%s#'`' \\;
    ''') % (includes.path, ctx.file.ara_mock_artifacts.path, "includes", ctx.file.ara_mock_artifacts.path, "includes", ctx.file.ara_mock_artifacts.path, "includes", includes.path, ctx.file.ara_mock_artifacts.path, "includes", includes.path)

    ctx.actions.run_shell(
        inputs = [ctx.file.ara_mock_artifacts],
        outputs = [includes],
        mnemonic = "filter",
        progress_message = "FILTER " + ctx.label.name,
        command = cmd,
        use_default_shell_env = True,
    )

    return DefaultInfo(files = depset([includes]))

_filter_hdrs_mock = rule(
    implementation = _filter_hdrs_mock_impl,
    attrs = {
        "ara_mock_artifacts": attr.label(allow_single_file = True),
    },
)

def cc_ara_mock_library(
        name,
        ara_mock_artifacts,
        alwayslink = True,  # Needs to be enabled to remove dependency to link order
        deps = [],
        **kwargs):
    """
    Build ara::com SOME/IP mocked binding as cc_library for SWC

    Args:
        name: Target name of the library
        ara_mock_artifacts: Label to the corresponding ara_mock_artifacts() target
        alwayslink: Wether to use alwayslink in the cc_library or not
        deps: Dependencies to be added to the cc_library
        **kwargs: Additional parameters to be forwarded
    """

    kwargs.setdefault("tags", [])

    hdrs_filter = "{}_hdr_files".format(name)
    srcs_filter = "{}_files".format(name)

    # Only provide needed files in sandbox and check that they are also created
    _filter_hdrs_mock(
        name = hdrs_filter,
        ara_mock_artifacts = ara_mock_artifacts,
        tags = kwargs["tags"],
    )

    _filter_srcs_mock(
        name = srcs_filter,
        ara_mock_artifacts = ara_mock_artifacts,
        tags = kwargs["tags"],
    )

    native.cc_library(
        name = name,
        hdrs = [hdrs_filter],
        srcs = [srcs_filter],
        alwayslink = alwayslink,
        linkstatic = True,
        testonly = True,
        deps = [
            "//platform/aas/mw/utils/mock:mock_registry",
            "@score-baselibs//score/memory/shared:types",
            "@googletest//:gtest",
        ] + deps,
        includes = [
            "{}/includes.h".format(hdrs_filter),
        ],
        **kwargs
    )
