FROM debian:latest
WORKDIR /i-havent-a-clue
RUN apt-get -y update && apt-get -y upgrade && apt-get install -y g++ cmake
COPY . .
# separated into three different commands because i thought it was nicer than concatenating them all with &&
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SERVER=ON -DBUILD_CLIENT=OFF
RUN cmake --build build
RUN ctest --test-dir build
EXPOSE 12345
CMD ["./build/server/server-bin"]
