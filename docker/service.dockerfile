# install userver dependencies and userver himself via .deb package
# .dep package with userver must be placed at docker/deps/ folder
FROM userver-base:latest 
ARG servicePath
ARG serviceName

WORKDIR /code
COPY  ../build/build-release/${servicePath}/${serviceName} .
ENV serviceName=$serviceName

ENTRYPOINT ./${serviceName}
