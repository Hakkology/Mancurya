#!/bin/bash

# Allow local connections to X11 (needed for GUI in Docker)
xhost +local:docker

# Build the docker image
# Note: Using sudo if you haven't added yourself to the docker group
echo "Building Mancurya Masterpiece Docker Image..."
sudo docker build -t mancurya .

# Run the docker container with GUI support
echo "Launching Mancurya 3D Naval Simulation..."
sudo docker run -it --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    --device /dev/dri:/dev/dri \
    mancurya
