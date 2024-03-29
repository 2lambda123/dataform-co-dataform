load("@rules_proto//proto:defs.bzl", "proto_library")
load("@io_bazel_rules_go//proto:def.bzl", "go_proto_library")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "files",
    srcs = glob(["**/*.proto"]),
)

proto_library(
    name = "dataform_proto",
    srcs = [
        "core.proto",
        "evaluation.proto",
        "execution.proto",
        "profiles.proto",
    ],
)

proto_library(
    name = "server_proto",
    srcs = ["server.proto"],
    deps = [
        ":dataform_proto",
    ],
)

java_proto_library(
    name = "dataform_java_proto",
    deps = [":dataform_proto"],
)

go_proto_library(
    name = "dataform_go_proto",
    importpath = "github.com/dataform-co/dataform/protos/dataform",
    proto = ":dataform_proto",
)

load("//tools:ts_proto_library.bzl", "ts_proto_library")

ts_proto_library(
    name = "ts",
    deps = [
        ":dataform_proto",
        ":server_proto",
    ],
)

load("//testing:build_test.bzl", "build_test")

build_test(
    name = "build_test",
    deps = [
        ":dataform_go_proto",
        ":dataform_java_proto",
    ],
)

go_proto_library(
    name = "server_go_proto",
    compilers = ["@io_bazel_rules_go//proto:go_grpc"],
    importpath = "github.com/dataform-co/dataform/protos",
    proto = ":server_proto",
    deps = [":dataform_go_proto"],
)
