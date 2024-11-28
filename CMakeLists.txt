# # # cmake_minimum_required(VERSION 3.15)
# # # project(SecureLoginSystem)

# # # # Set C++ version
# # # set(CMAKE_CXX_STANDARD 17)
# # # set(CMAKE_CXX_STANDARD_REQUIRED True)

# # # # Find AWS SDK components
# # # find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# # # # Add the executable
# # # add_executable(SecureLoginSystem main.cpp)

# # # # Link AWS SDK libraries
# # # target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})





# # # cmake_minimum_required(VERSION 3.14)

# # # # Set the project name
# # # project(SecureLoginSystem)

# # # # Set the C++ standard
# # # set(CMAKE_CXX_STANDARD 17)

# # # # Add executables
# # # add_executable(list_tables main.cpp)   # For listing DynamoDB tables
# # # add_executable(new_user new_user.cpp) # For new user creation

# # # # Find AWS SDK
# # # find_package(AWSSDK REQUIRED COMPONENTS dynamodb)

# # # # Link AWS SDK libraries
# # # target_link_libraries(list_tables ${AWSSDK_LINK_LIBRARIES})
# # # target_link_libraries(new_user ${AWSSDK_LINK_LIBRARIES})




# # cmake_minimum_required(VERSION 3.5)
# # project(SecureLoginSystem)

# # # Find the AWS SDK for C++
# # find_package(AWSSDK REQUIRED COMPONENTS dynamodb)

# # # Add the executable and include both source files
# # add_executable(new_user new_user.cpp CustomHash.cpp)

# # # Link the AWS SDK libraries
# # target_link_libraries(new_user ${AWSSDK_LINK_LIBRARIES})

# # # Optionally include directories if needed
# # target_include_directories(new_user PRIVATE ${AWSSDK_INCLUDE_DIR})




# # # CMakeLists.txt
# # cmake_minimum_required(VERSION 3.5)
# # project(SecureLoginSystem)

# # # Set the C++ standard
# # set(CMAKE_CXX_STANDARD 11)
# # set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # # Find the AWS SDK for C++
# # find_package(AWSSDK REQUIRED COMPONENTS dynamodb)

# # # Add the executable and source files
# # add_executable(SecureLoginSystem main.cpp new_user.cpp CustomHash.cpp)

# # # Include directories (if necessary)
# # target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# # # Link libraries
# # target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})



# # CMakeLists.txt
# cmake_minimum_required(VERSION 3.5)
# project(SecureLoginSystem)

# # Set the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find the AWS SDK for C++
# find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# # Add the executable and source files
# add_executable(SecureLoginSystem main.cpp new_user.cpp CustomHash.cpp otp.cpp)

# # Include directories (if necessary)
# target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# # Link libraries
# target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})

# # Include the AWS SDK email component
# target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)


# CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(SecureLoginSystem)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the AWS SDK for C++
find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# Add the executable and source files
add_executable(SecureLoginSystem main.cpp new_user.cpp existing_user.cpp CustomHash.cpp otp.cpp)

# Include directories (if necessary)
target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# Link libraries
target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})

# Include the AWS SDK email component
target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)


