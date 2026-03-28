# Use a basic Ubuntu image with OpenGL dependencies
FROM ubuntu:22.04

# Prevent interactive prompts during install
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and OpenGL/GLUT/GLEW libraries
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    libglew-dev \
    libx11-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the project files
COPY . .

# Build the project
RUN rm -rf build && mkdir -p build && cd build && cmake .. && make

# Command to run (Note: Requires X11 forwarding to see the window)
CMD ["./build/mancurya"]
