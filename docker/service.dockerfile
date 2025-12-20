ARG serviceName
ARG servicePath
# install userver dependencies and userver himself via .deb package
# .dep package with userver must be placed at docker/deps/ folder
FROM userver-base:latest AS builder

WORKDIR /code

COPY . .

RUN /code/docker/buildService.sh

FROM userver-base:latest
ARG serviceName
ARG servicePath

ENV serviceName=${serviceName}
EXPOSE 8080

COPY --from=builder /build/${servicePath}/${serviceName} /bin/${serviceName}
ENTRYPOINT /bin/$serviceName

