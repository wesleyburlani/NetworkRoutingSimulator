# Get the GCC preinstalled image from Docker Hub
FROM gcc:4.9

# Copy the current folder which contains C source code to the Docker image under /usr/src
COPY . /app

# Specify the working directory
WORKDIR /app

# Use GCC to compile the Test.cpp source file
RUN gcc program.c -o bin/program -pthread -Wall