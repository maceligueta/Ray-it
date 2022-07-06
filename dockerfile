FROM ubuntu:latest

RUN apt-get update && apt-get install -y build-essential cmake --no-install-recommends

ADD . /ray-it

WORKDIR /ray-it
RUN ./linux_compile_release.sh