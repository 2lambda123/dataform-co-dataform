package(default_visibility = ["//visibility:public"])

load("@io_bazel_rules_docker//container:image.bzl", "container_image")

# Exists purely to give a clean name to the postgres Docker image.
container_image(
    name = "postgres_image",
    base = "@postgres//image",
)

load("//tools:ts_library.bzl", "ts_library")

ts_library(
    name = "postgres",
    srcs = glob(
        ["*.ts"],
    ),
    data = [
        ":postgres_image",
    ],
    deps = [
        "//common/promises",
        "//testing",
        "@npm//@types/node",
        "@npm//@types/pg",
        "@npm//@types/pg-query-stream",
        "@npm//@types/uuid",
        "@npm//pg",
        "@npm//promise-pool-executor",
        "@npm//uuid",
    ],
)
