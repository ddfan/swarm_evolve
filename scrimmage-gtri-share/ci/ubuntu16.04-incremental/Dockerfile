###############################################################################
# Dockerfile to build SCRIMMAGE GTRI SHARE(incremental version)
###############################################################################
FROM localhost:5000/scrimmage-gtri-share:latest

MAINTAINER Kevin DeMarco

# Copy repo code into image
RUN mkdir -p /root/scrimmage-gtri-share
COPY ./ /root/scrimmage-gtri-share/

WORKDIR /root/scrimmage-gtri-share

RUN /bin/bash -c "mkdir -p build && \
    cd build && \
    cmake .. && \
    source ./setenv.sh && \    
    make"
