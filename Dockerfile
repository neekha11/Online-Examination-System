
FROM gcc:13

WORKDIR /app

COPY . .

RUN g++ -std=c++11 -Wall -Wextra -pthread -I. \
    server_main.cpp src/server/ApiServer.cpp \
    src/models/*.cpp src/storage/*.cpp \
    src/services/*.cpp src/utils/*.cpp \
    -o server

EXPOSE 8080

CMD ["./server"]
