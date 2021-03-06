#!/bin/bash

export HOST_UID=$(id -u)
export HOST_GID=$(id -g)
export WHO=$(whoami)

docker-compose build
docker-compose -f docker-compose.yml up -d
