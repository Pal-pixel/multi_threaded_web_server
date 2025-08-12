# Use official GCC image (Debian-based) for C++ development
FROM gcc:13

# Install extra tools if needed (g++, make, cmake, gdb, netcat for networking tests)
RUN apt-get update && apt-get install -y \
    cmake \
    make \
    gdb \
    netcat-openbsd \
    && rm -rf /var/lib/apt/lists/*
    

# Install MySQL client and C++ connector libraries
RUN apt-get update && apt-get install -y \
    default-mysql-client \
    default-libmysqlclient-dev


# Set working directory inside container
WORKDIR /app

# Copy source files (will mount during dev, so not permanent)
COPY ./src ./src
COPY ./include ./include

# Default command (just to keep container alive in dev mode)
CMD ["bash"]
