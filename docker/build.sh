#!/usr/bin/bash

docker rm devel
docker build . -t environment:1.1.1 -t environment:latest
docker run -dit -P --name devel -v ~/project:/root/project environment:latest /bin/bash

