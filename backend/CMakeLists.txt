cmake_minimum_required(VERSION 3.20)
project(clouddrive CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)
# only ask for aws services that we use
find_package(AWSSDK CONFIG REQUIRED COMPONENTS s3 identity-management) #should we add core??

add_executable(server
  src/main.cpp
  src/controllers/HealthCtrl.cc
  src/controllers/UploadCtrl.cc
  src/controllers/FilesCtrl.cc
  src/plugins/RepoPlugin.cc
  src/plugins/S3ClientPlugin.cc
)

target_include_directories(server PRIVATE src)
target_link_libraries(server 
  PRIVATE 
    Drogon::Drogon 
    OpenSSL::SSL 
    pq 
    # AWS::aws-cpp-sdk-s3 
    # AWS::aws-cpp-sdk-identity-management
    ${AWSSDK_LINK_LIBRARIES}
)
