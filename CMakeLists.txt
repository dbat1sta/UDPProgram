cmake_minimum_required(VERSION 3.10)

# Set the project name
project(UDPProgram)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the executable for the server
add_executable(udp_server udp_server.c)

# Add the executable for the client
add_executable(udp_client udp_client.c)

# Add any necessary compiler flags
target_compile_options(udp_server PRIVATE -Wall -Wextra)
target_compile_options(udp_client PRIVATE -Wall -Wextra)
