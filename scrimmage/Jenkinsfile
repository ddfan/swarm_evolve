#!/usr/bin/env groovy

node('docker-host') {
  checkout scm

  def app_env

  stage "Build"
  gitlabCommitStatus("build") {
    app_env = docker.build("localhost:5000/scrimmage:${env.BUILD_NUMBER}", "-f ci/ubuntu16.04/Dockerfile .")
  }

  // TODO: Currently there is a bug with using
  // docker.image.inside, hopefully, it gets fixed soon
  // Debug: app_env = docker.image("localhost:5000/scrimmage:latest")

  stage "Push Image"
  gitlabCommitStatus("push image") {
    docker.withRegistry('http://localhost:5000') {
      app_env.push('latest')
    }
  }
}

//
//pipeline {
//
//  agent { node { label 'docker-host' } }
//  // Setup gitlab connection
//  options {
//    gitLabConnection('atas-gita-gitlab')
//  }
//  triggers {
//    gitlab(triggerOnPush: true, triggerOnMergeRequest: true, branchFilterType: 'All')
//  }
//
//  stages {
//    stage('Checkout') {
//      steps {
//        //updateGitlabCommitStatus name: 'jenkins', state: 'pending'
//        checkout scm
//      }
//    }
//    stage('Pull Image') {
//      steps {
//        def slave
//        slave = docker.image('jenkins_slave')
//        slave.pull()
//      }
//    }
//  }
//}

//  node('docker-host') {
//    def slave = docker.image('jenkins_slave')
//    slave.pull()
//
//    docker.withRegistry('http://localhost:5000') {
//      slave.inside {
//        stage("Building") {
//            sh 'touch build.txt'
//        }
//        stage("Testing") {
//            sh 'touch test.txt'
//        }
//      }
//      slave.push('mypushed')
//    }
//  }
//}

//pipeline {
//
//    agent { node { label 'docker-ubuntu-16.04-slave' } }
//    // Setup gitlab connection
//    options {
//        gitLabConnection('atas-gita-gitlab')
//    }
//    triggers {
//        gitlab(triggerOnPush: true, triggerOnMergeRequest: true, branchFilterType: 'All')
//    }
//
//    stages {
//        stage('Checkout') {
//            steps {
//                //updateGitlabCommitStatus name: 'jenkins', state: 'pending'
//                checkout scm
//            }
//        }
//        stage('Install Packages') {
//            steps {
//                echo 'Installing required packages...'
//                sh 'sudo ./setup/install-binaries.sh'
//            }
//        }
//
//        stage('CMake') {
//            steps {
//                echo 'Configuring with CMake...'
//                sh '''
//                   mkdir -p build && cd build && cmake ..
//                '''
//            }
//        }
//
//        stage('Build') {
//            steps {
//                echo 'Building...'
//                sh '''#!/bin/bash -l
//                   cd build && source setenv.sh && make
//                '''
//            }
//        }
//
//        stage('Package') {
//            steps {
//                echo 'Packaging...'
//                sh '''#!/bin/bash -l
//                   cd build && source setenv.sh && make package
//                '''
//            }
//        }
//        stage('Install') {
//            steps {
//                echo 'Installing...'
//                sh '''#!/bin/bash -l
//                   cd build && source setenv.sh && sudo make install
//                '''
//            }
//        }
//
//        //stage('Unit Tests') {
//        //    steps {
//        //        echo 'Running unit tests...'
//        //        sh '''#!/bin/bash -l
//        //           cd build && source setenv.sh && make test
//        //        '''
//        //    }
//        //}
//    }
//
//    // Notify gitlab of build failures and successes
//    post {
//        failure {
//            updateGitlabCommitStatus name: 'build', state: 'failed'
//        success {
//            updateGitlabCommitStatus name: 'build', state: 'success'
//        }
//    }
//}

// Workflow notes:
// https://go.cloudbees.com/docs/cloudbees-documentation/cje-user-guide/index.html#docker-workflow-sect-inside
// http://www.stuartellis.name/articles/jenkins-pipeline/

// Example workflow with volumes
// https://antonfisher.com/posts/2017/01/16/run-jenkins-in-docker-container-with-persistent-configuration/
//node('master') {
//    docker.withServer('unix:///var/run/docker.sock') {
//        stage('Git clone') {
//            git [email protected]:antonfisher/node-mocha-extjs.git'
//        }
//        stage('Build') {
//            docker
//                .image('jenkins-agent-ubuntu')
//                .inside('--volumes-from jenkins-master') {
//                    sh "bash ./build.sh;"
//                }
//        }
//        stage('Copy build results') {
//            docker
//                .image('jenkins-agent-ubuntu')
//                .inside('--volumes-from jenkins-master') {
//                    sh """
//                        sshpass -plol scp \
//                            "${WORKSPACE}/build/*.tar.gz" \
//                            "[email protected]:/buils";
//                    """
//                }
//        }
//        stage('UI unit tests') {
//            docker
//                .image('jenkins-agent-ubuntu')
//                .inside('--volumes-from jenkins-master') {
//                    sh """
//                        cd ./tests;
//                        bash ./run.sh;
//                    """
//                }
//        }
//    }
//}
