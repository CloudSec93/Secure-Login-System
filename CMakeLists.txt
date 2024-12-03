

# cmake_minimum_required(VERSION 3.5)
# project(SecureLoginSystem)

# # Set the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find the AWS SDK for C++
# find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# # Add the Argon2 library (manually specify source files)
# set(ARGON2_SRC
#     third_party/phc-winner-argon2/src/argon2.c
#     third_party/phc-winner-argon2/src/core.c
#     third_party/phc-winner-argon2/src/blake2/blake2b.c
#     third_party/phc-winner-argon2/src/thread.c
#     third_party/phc-winner-argon2/src/encoding.c
#     third_party/phc-winner-argon2/src/opt.c
# )

# # Include Argon2 headers
# include_directories(third_party/phc-winner-argon2/include)

# # Add the executable and source files
# add_executable(SecureLoginSystem
#     main.cpp
#     new_user.cpp
#     existing_user.cpp
#     forgot_password.cpp
#     CustomHash.cpp
#     Argon2Hash.cpp
#     otp.cpp
#     utilities.cpp
#     # PasswordEntry.cpp
#     PasswordManager.cpp
#     ${ARGON2_SRC} # Include Argon2 source files in the build
# )

# # Include directories for AWS SDK
# target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# # Link libraries
# target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})
# target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)






# cmake_minimum_required(VERSION 3.5)
# project(SecureLoginSystem)

# # Set the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find the AWS SDK for C++
# find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# # Add the Argon2 library (manually specify source files)
# set(ARGON2_SRC
#     third_party/phc-winner-argon2/src/argon2.c
#     third_party/phc-winner-argon2/src/core.c
#     third_party/phc-winner-argon2/src/blake2/blake2b.c
#     third_party/phc-winner-argon2/src/thread.c
#     third_party/phc-winner-argon2/src/encoding.c
#     third_party/phc-winner-argon2/src/opt.c
# )

# # Include Argon2 headers
# include_directories(third_party/phc-winner-argon2/include)

# # Add the executable and source files
# add_executable(SecureLoginSystem
#     main.cpp
#     new_user.cpp
#     existing_user.cpp
#     forgot_password.cpp
#     CustomHash.cpp
#     Argon2Hash.cpp
#     otp.cpp
#     utilities.cpp
#     ${ARGON2_SRC} # Include Argon2 source files in the build
# )

# # Include directories for AWS SDK
# target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# # Link libraries
# target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})
# target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)



# CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(SecureLoginSystem)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the AWS SDK for C++
find_package(AWSSDK REQUIRED COMPONENTS dynamodb email)

# Add the Argon2 library (manually specify source files)
set(ARGON2_SRC
    third_party/phc-winner-argon2/src/argon2.c
    third_party/phc-winner-argon2/src/core.c
    third_party/phc-winner-argon2/src/blake2/blake2b.c
    third_party/phc-winner-argon2/src/thread.c
    third_party/phc-winner-argon2/src/encoding.c
    third_party/phc-winner-argon2/src/opt.c
)

# Include directories for Argon2 and other headers
include_directories(
    third_party/phc-winner-argon2/include
    include
)

# Add the executable and source files
add_executable(SecureLoginSystem
    main.cpp
    new_user.cpp
    existing_user.cpp
    forgot_password.cpp
    CustomHash.cpp
    Argon2Hash.cpp
    otp.cpp
    utilities.cpp
    PasswordManager.cpp
    Encryption.cpp
    ${ARGON2_SRC} # Include Argon2 source files in the build
)

# Include directories for AWS SDK
target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# Link libraries
target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)
