#!/bin/bash

echo "DISPLAY=${DISPLAY}" > display.env

docker-compose build 
