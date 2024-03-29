load("//tools:ts_library.bzl", "ts_library")
load("//:version.bzl", "DF_VERSION")
load("//packages:index.bzl", "pkg_bundle", "pkg_bundle_dts", "pkg_json", "pkg_npm_tar")
load("//tools/common:copy.bzl", "copy_file")

package(default_visibility = ["//visibility:public"])

ts_library(
    name = "core",
    srcs = glob(["*.ts"]),
    deps = [
        "//core",
    ],
)

externals = [
    "protobufjs",
    "tarjan-graph",
    "semver",
    "moo",
    "long",
]

pkg_json(
    name = "json",
    package_name = "@dataform/core",
    description = "Dataform core API.",
    external_deps = externals,
    layers = [
        "//:package.json",
        "//packages/@dataform:package.layer.json",
    ],
    main = "bundle.js",
    types = "bundle.d.ts",
    version = DF_VERSION,
)

pkg_bundle(
    name = "bundle",
    entry_point = "index.ts",
    externals = externals,
    deps = [
        ":core",
    ],
)

pkg_bundle_dts(
    name = "bundle.d",
    entry_point = "index.ts",
    externals = externals,
    deps = [
        ":core",
    ],
)

copy_file(
    name = "core_proto",
    src = "//protos:core.proto",
    out = "core.proto",
)

pkg_npm_tar(
    name = "package",
    deps = [
        ":bundle",
        ":bundle.d",
        ":package.json",
        ":core.proto",
    ],
)
