FROM ubuntu:18.04

RUN apt update && \
    apt install -y build-essential g++-aarch64-linux-gnu git cmake

RUN mkdir fcs-server
WORKDIR fcs-server
CMD ["/bin/bash"]
