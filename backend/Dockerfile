# FROM ubuntu:24.04 as build
# RUN apt-get update && apt-get install -y g++ cmake git libdrogon-dev libssl-dev libpq-dev libjsoncpp-dev uuid-dev \
# zlib1g-dev libsqlite3-dev default-libmysqlclient-dev
# WORKDIR /app
# COPY . .
# RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j 

# FROM ubuntu:24.04
# RUN apt-get update && apt-get install -y ca-certificates libjsoncpp25 libuuid1 \
# zlib1g libsqlite3-0 libmysqlclient21 && rm -rf /var/lib/apt/lists/*
# WORKDIR /app
# COPY --from=build /app/build/server /app/server
# COPY config.json /app/config.json
# EXPOSE 8080
# CMD ["/app/server"]

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
  build-essential cmake pkg-config \
  uuid-dev libjsoncpp-dev libssl-dev zlib1g-dev \
  libpq-dev \
  && rm -rf /var/lib/apt/lists/*
COPY --from=drogon-build /usr/local /usr/local
COPY . /app
RUN rm -rf build \
 && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
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

