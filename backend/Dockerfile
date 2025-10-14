# ===== Stage 1: build Drogon from source (Postgres only) =====
FROM ubuntu:24.04 AS drogon-build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /tmp

RUN apt-get update && apt-get install -y --no-install-recommends \
  ca-certificates git \          
  build-essential cmake pkg-config \
  uuid-dev libjsoncpp-dev libssl-dev zlib1g-dev \
  libpq-dev \
  && update-ca-certificates \
  && rm -rf /var/lib/apt/lists/*

# build drogon
RUN git clone --depth=1 --recurse-submodules --shallow-submodules https://github.com/drogonframework/drogon.git
WORKDIR /tmp/drogon
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_ORM=ON \
      -DBUILD_POSTGRESQL=ON \
      -DBUILD_SQLITE=OFF \
      -DBUILD_MYSQL=OFF \
  && cmake --build build -j \
  && cmake --install build

# ===== Stage 2: build your app =====
FROM ubuntu:24.04 AS app-build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential cmake pkg-config git curl zip unzip \
  uuid-dev libjsoncpp-dev libssl-dev zlib1g-dev ca-certificates \
  libpq-dev \
  && rm -rf /var/lib/apt/lists/*

# vcpkg(providing libraries for aws s3)
WORKDIR /opt
RUN git clone https://github.com/microsoft/vcpkg.git && \
  /opt/vcpkg/bootstrap-vcpkg.sh
# only install the features we need
RUN /opt/vcpkg/vcpkg install "aws-sdk-cpp[s3, identity-management]" --triplet x64-linux

WORKDIR /app
COPY --from=drogon-build /usr/local /usr/local
COPY . .
RUN rm -rf /app/build \
 && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux \
  -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON \
 && cmake --build build -j

# ===== Stage 3: slim runtime =====
FROM ubuntu:24.04 AS runtime
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends \
  libuuid1 libjsoncpp25 libssl3 zlib1g libpq5 \
  && rm -rf /var/lib/apt/lists/*
COPY --from=app-build /app/build/server /app/server
EXPOSE 8080
CMD ["/app/server"]

# # ============================================================
# # Stage 1: build Drogon (static, Postgres only)
# # ============================================================
# FROM ubuntu:24.04 AS drogon-build
# ENV DEBIAN_FRONTEND=noninteractive
# WORKDIR /tmp

# RUN apt-get update && apt-get install -y --no-install-recommends \
#     ca-certificates git \
#     build-essential cmake pkg-config \
#     uuid-dev libjsoncpp-dev libssl-dev zlib1g-dev \
#     libpq-dev \
#  && update-ca-certificates \
#  && rm -rf /var/lib/apt/lists/*

# # Build Drogon as static (installs libdrogon.a, libtrantor.a, and CMake config)
# RUN git clone --depth=1 --recurse-submodules --shallow-submodules https://github.com/drogonframework/drogon.git
# WORKDIR /tmp/drogon
# RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
#       -DBUILD_SHARED_LIBS=OFF \            
#       -DBUILD_EXAMPLES=OFF \
#       -DBUILD_ORM=ON \
#       -DBUILD_POSTGRESQL=ON \
#       -DBUILD_SQLITE=OFF \
#       -DBUILD_MYSQL=OFF \
#  && cmake --build build -j \
#  && cmake --install build

# # ============================================================
# # Stage 2: build your app with vcpkg (AWS SDK via CONFIG mode)
# # ============================================================
# FROM ubuntu:24.04 AS app-build
# ENV DEBIAN_FRONTEND=noninteractive
# WORKDIR /app

# RUN apt-get update && apt-get install -y --no-install-recommends \
#     build-essential cmake pkg-config git curl zip unzip \
#     uuid-dev libjsoncpp-dev libssl-dev zlib1g-dev ca-certificates \
#     libpq-dev \
#  && rm -rf /var/lib/apt/lists/*

# # vcpkg (use static linkage for Linux by default; consistent with static Drogon)
# WORKDIR /opt
# RUN git clone https://github.com/microsoft/vcpkg.git && /opt/vcpkg/bootstrap-vcpkg.sh

# # Keep triplet/linkage explicit so CMake + vcpkg agree
# ENV VCPKG_ROOT=/opt/vcpkg
# ENV VCPKG_DEFAULT_TRIPLET=x64-linux
# # (Optional) explicitly prefer static from vcpkg; comment out if you want dynamic:
# ENV VCPKG_FEATURE_FLAGS=-manifests
# # If you want dynamic from vcpkg instead, you can switch to: x64-linux-dynamic

# # Install only the AWS components you use
# RUN /opt/vcpkg/vcpkg install "aws-sdk-cpp[s3,identity-management]" --triplet x64-linux

# # Bring Drogon (static install under /usr/local) into this stage
# COPY --from=drogon-build /usr/local /usr/local

# # Build your app (fresh configure; CONFIG mode packages preferred)
# WORKDIR /app
# COPY . .

# # Safety: ensure stale cache doesn't leak in
# RUN rm -rf /app/build

# # Optional early fail if AWS config is missing (helps debugging)
# RUN test -f /opt/vcpkg/installed/x64-linux/share/aws-sdk-cpp/aws-sdk-cpp-config.cmake

# RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
#     -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
#     -DVCPKG_TARGET_TRIPLET=x64-linux \
#     -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON \
#     -DBUILD_SHARED_LIBS=OFF \             
#  && cmake --build build -j

# # ============================================================
# # Stage 3: slim runtime (only shared system libs needed)
# # ============================================================
# FROM ubuntu:24.04 AS runtime
# ENV DEBIAN_FRONTEND=noninteractive
# WORKDIR /app

# # Only the runtime .so deps (OpenSSL, libpq, zlib, uuid, etc.)
# RUN apt-get update && apt-get install -y --no-install-recommends \
#     libuuid1 libssl3 zlib1g libpq5 ca-certificates \
#  && rm -rf /var/lib/apt/lists/*

# # Your statically-linked server (no Drogon/AWS .so needed)
# COPY --from=app-build /app/build/server /app/server

# EXPOSE 8080
# CMD ["/app/server"]
