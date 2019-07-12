package(default_visibility = ["//visibility:public"])

licenses(["notice"])

exports_files(["LICENSE"])

cc_library(
    name = "robots",
    srcs = [
        "robots.cc",
        "rewriter.cc",
    ],
    hdrs = [
        "robots.h",
    ],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:fixed_array",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "robots_test",
    srcs = ["robots_test.cc"],
    deps = [
        ":robots",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "check_urls",
    srcs = ["check_urls.cc"],
    linkstatic = 1,
    features = [ "fully_static_link" ],
    deps = [
        ":robots",
    ],
)

cc_binary(
    name = "check_robots",
    srcs = ["check_robots.cc"],
    linkstatic = 1,
    features = [ "fully_static_link" ],
    deps = [
        ":robots",
    ],
)
