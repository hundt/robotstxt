FROM golang:1.12 AS build-server
COPY ./robots-api /build
WORKDIR /build
RUN CGO_ENABLED=0 GOBIN=/bin/ go install .
RUN touch /empty

FROM l.gcr.io/google/bazel:0.17.1 AS build-parser
COPY . /src/workspace
WORKDIR /src/workspace
RUN bazel build :check_robots :check_urls
RUN cp bazel-bin/check_urls bazel-bin/check_robots /bin/

FROM scratch
COPY --from=build-server /bin/robots-api /bin/robots-api
COPY --from=build-parser /bin/check_urls /bin/check_robots /bin/
# so that tmp dir exists
COPY --from=build-server /empty /tmp/empty
ENTRYPOINT ["/bin/robots-api"]
