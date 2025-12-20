# install userver dependencies and userver himself via .deb package
# .dep package with userver must be placed at docker/deps/ folder
FROM ubuntu:24.04 AS builder

COPY docker/deps/* /service_tmp/
COPY docker/* /service_tmp/

RUN apt update \
  && apt install -y $(cat /service_tmp/ubuntu-24.04.md) \
	&& apt install -y python3-pip \
  && apt install -y locales 
	# apt clean all

RUN \
  # Set UTC timezone \
  TZ=Etc/UTC; \
  ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
   \
  # Generate locales \
  && locale-gen en_US.UTF-8 \
  && update-locale LC_ALL="en_US.UTF-8" LANG="en_US.UTF-8" LANGUAGE="en_US.UTF-8" 

RUN /service_tmp/ubuntu-install-mongodb.sh
RUN dpkg -i /service_tmp/ubuntu24.04-libuserver-all-dev_2.13_amd64.deb
