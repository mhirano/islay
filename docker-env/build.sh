#!/bin/bash

######################
## SET PROJECT NAME ##
######################
PROJECT_NAME="islay" # must be lowercase
CONTAINER_NAME="bowmore" # must be lowercase
sed -i "s/islay/${PROJECT_NAME}/g" docker-compose.yml
sed -i "s/islay/${PROJECT_NAME}/g" create_and_start.sh
sed -i "s/islay/${PROJECT_NAME}/g" Dockerfile
sed -i "s/islay/${PROJECT_NAME}/g" ../.gitignore
sed -i "s/project(islay)/project(${PROJECT_NAME})/g" ../CMakeLists.txt
sed -i "s/bowmore/${CONTAINER_NAME}/g" docker-compose.yml

# Set DISPLAY to pass environment variable
echo "DISPLAY=${DISPLAY}" > display.env

docker-compose build 
