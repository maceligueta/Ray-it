FROM ubuntu:latest

RUN apt-get update && apt-get install -y build-essential cmake --no-install-recommends
RUN apt install -y gdb dos2unix
ADD . /ray-it
WORKDIR /ray-it

RUN dos2unix ./cases/*

RUN ./linux_compile_release.sh
RUN ./linux_compile_debug.sh
RUN ./linux_compile_release_float.sh
RUN ./linux_compile_debug_float.sh

# docker build -t ray-it:latest .