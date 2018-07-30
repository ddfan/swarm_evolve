###############################################################################
# Dockerfile to build SCRIMMAGE and run its tests
###############################################################################

FROM ubuntu:14.04

MAINTAINER Kevin DeMarco

RUN apt-get update && apt-get install -y git

# Make ssh dir
RUN mkdir /root/.ssh/

# Copy over private key, and set permissions
ADD id_rsa /root/.ssh/id_rsa
RUN chmod 700 /root/.ssh/id_rsa
RUN echo "Host 172.17.0.1\n\tStrictHostKeyChecking no\n" >> /root/.ssh/config

# Create known_hosts
RUN touch /root/.ssh/known_hosts

RUN  echo "    IdentityFile ~/.ssh/id_rsa" >> /etc/ssh/ssh_config

# Add key
RUN ssh-keyscan 172.17.0.1 >> /root/.ssh/known_hosts

ARG USER

RUN cd /root && git clone $USER@172.17.0.1:~/scrimmage/scrimmage

WORKDIR /root/scrimmage
RUN ./setup/install-binaries.sh
RUN mkdir build && cd build && cmake .. && ./setenv.sh
