FROM alpine:latest

# Install build dependencies (GCC, Make, libc-dev, POSIX threads)
RUN apk add --no-cache build-base

# Set working directory inside the container
WORKDIR /app

# Copy all project files into the container
COPY . .

# Compile the C daemon
RUN make clean && make

# Expose port 9000 for web UI and JSON telemetry
EXPOSE 9000

# Run the control daemon when container starts
CMD ["./bbb_gen_ctrl"]
