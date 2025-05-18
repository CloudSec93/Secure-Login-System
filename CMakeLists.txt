

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
# include_directories(
#     third_party/phc-winner-argon2/include
#     include
# )


include_directories(
    third_party/phc-winner-argon2/include # Argon2 headers
    .                                   # Include project root (for main.cpp if needed, though headers are subdirs)
    auth                                # Headers in the auth directory
    encryptions                         # Headers in the encryptions directory (e.g., Encryption.h)
    hashing                             # Headers in the hashing directory (e.g., Argon2Hash.h, CustomHash.h)
    other_features                      # Headers in the other_features directory (e.g., otp.h, PasswordManager.h, forgot_password.h)
    utils                               # Headers in the utils directory (e.g., utilities.h)
)


# Add the executable and source files
add_executable(SecureLoginSystem
    main.cpp
    
    auth/new_user.cpp
    auth/existing_user.cpp
    
    other_features/forgot_password.cpp
    
    hashing/CustomHash.cpp
    hashing/Argon2Hash.cpp
    
    other_features/otp.cpp
    
    utils/utilities.cpp
    
    other_features/PasswordManager.cpp
    
    encryptions/Encryption.cpp
    encryptions/ceasar_cipher.cpp      
    encryptions/vigenère_cipher.cpp
    ${ARGON2_SRC} # Include Argon2 source files in the build
)

# Include directories for AWS SDK
target_include_directories(SecureLoginSystem PRIVATE ${AWSSDK_INCLUDE_DIR})

# Link libraries
target_link_libraries(SecureLoginSystem ${AWSSDK_LINK_LIBRARIES})
target_link_libraries(SecureLoginSystem aws-cpp-sdk-email)
