#include "queue.hpp"
#include "thread_pool.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

int main() {
    const int PORT = 8080;

    // 0) Create task queue and thread pool
    TaskQueue<ThreadPool::Task> taskQueue;
    ThreadPool pool(std::thread::hardware_concurrency(), taskQueue);

    // 1) Create socket
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2) Reuse port
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // 3) Bind to 0.0.0.0:PORT
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 4) Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "✅ Server listening on http://localhost:" << PORT << std::endl;

    while (true) {
        // 5) Accept one client
        sockaddr_in client{};
        socklen_t clen = sizeof(client);
        int cfd = ::accept(server_fd, reinterpret_cast<sockaddr*>(&client), &clen);
        if (cfd < 0) {
            perror("accept");
            continue;
        }

        // 6) Instead of handling here, push task into thread pool queue
        taskQueue.push([cfd] {
            // 6a) Read request
            char buf[4096];
            ssize_t n = ::read(cfd, buf, sizeof(buf) - 1);
            if (n <= 0) {
                ::close(cfd);
                return;
            }
            buf[n] = '\0';
            std::cout << "---- request begin ----\n" << buf << "---- request end ----\n";

            // 7) Load index.html from disk
            std::ifstream file("www/index.html");
            std::string body;
            if (file) {
                body.assign((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
            } else {
                body = "<html><body><h1>File not found</h1></body></html>";
            }

            // 8) Build HTTP response
            std::string headers =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Connection: close\r\n"
                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                "\r\n";

            std::string resp = headers + body;

            // 9) Send response
            ::write(cfd, resp.c_str(), resp.size());

            // 10) Close client
            ::close(cfd);
        });
    }

    ::close(server_fd);
    return 0;
}
