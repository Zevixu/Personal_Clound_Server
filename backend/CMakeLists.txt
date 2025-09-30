cmake_minimum_required(VERSION 3.20)
project(clouddrive CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

add_executable(server
  src/main.cpp
  src/controllers/HealthCtrl.cc
  src/controllers/UploadCtrl.cc
  src/controllers/FilesCtrl.cc
  src/services/S3Client.cc
  src/db/Repo.cc
)

target_include_directories(server PRIVATE src)
target_link_libraries(server PRIVATE Drogon::Drogon OpenSSL::SSL pq)
