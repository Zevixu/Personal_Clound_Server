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
