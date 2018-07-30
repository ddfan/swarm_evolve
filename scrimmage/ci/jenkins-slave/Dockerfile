###############################################################################
# Dockerfile to build jenkins slave
###############################################################################

FROM ubuntu:16.04

MAINTAINER Kevin DeMarco

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y git \
    wget \
    openssh-server \
    openjdk-9-jre-headless \
    sudo\
    lsb-release

#############################
# Setup ssh service
#############################
RUN mkdir /var/run/sshd
RUN echo 'root:screencast' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
#############################

# gen dummy keys
RUN /usr/bin/ssh-keygen -A

# Create Jenkins User
RUN useradd jenkins -m -s /bin/bash

# Add public key for Jenkins login
RUN mkdir /home/jenkins/.ssh
COPY /ssh/authorized_keys /home/jenkins/.ssh/authorized_keys
RUN chown -R jenkins /home/jenkins
RUN chgrp -R jenkins /home/jenkins
RUN chmod 600 /home/jenkins/.ssh/authorized_keys
RUN chmod 700 /home/jenkins/.ssh

RUN ssh-keyscan atas-gita.gtri.gatech.edu >> /root/.ssh/known_hosts
RUN ssh-keyscan 130.207.222.68 >> /root/.ssh/known_hosts

# Add the jenkins user to sudoers, don't require password with sudo
RUN echo "jenkins  ALL=(ALL)  NOPASSWD:ALL" >> etc/sudoers

# Expose SSH port and run SSHD
EXPOSE 22
CMD ["/usr/sbin/sshd","-D"]
