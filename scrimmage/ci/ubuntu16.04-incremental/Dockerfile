###############################################################################
# Dockerfile to build SCRIMMAGE (incremental version)
###############################################################################
FROM localhost:5000/scrimmage:latest

MAINTAINER Kevin DeMarco

# Copy repo code into image
RUN mkdir -p /root/scrimmage
COPY ./ /root/scrimmage/

WORKDIR /root/scrimmage

RUN /bin/bash -c "mkdir -p build && \
    cd build && \
    cmake .. -DBUILD_TESTS=ON && \
    source ./setenv.sh && \    
    make"

# Run tests (TODO: this will eventually be in Jenkins workflow when a plugin
# bug is fixed
RUN /bin/bash -c "cd build && \
    source ./setenv.sh && \    
    make test && \
    scrimmage ../missions/straight-no-gui.xml"
