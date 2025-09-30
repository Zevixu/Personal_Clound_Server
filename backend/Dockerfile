FROM debian:bookworm as build
RUN apt-get update && apt-get install -y g++ cmake git libdrogon-dev libssl-dev libpq-dev
WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j

FROM debian:bookworm
RUN apt-get update && apt-get install -y ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/build/server /app/server
COPY config.json /app/config.json
EXPOSE 8080
CMD ["/app/server"]
