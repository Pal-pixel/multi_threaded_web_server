#include "thread_pool.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

static int server_fd = -1;
static ThreadPool* pool_ptr = nullptr;

void handle_signal(int) {
    std::cout << "\nShutting down...\n" << std::flush;
    if (pool_ptr) pool_ptr->shutdown();
    if (server_fd >= 0) ::close(server_fd);
    exit(0);
}

std::string load_index_html() {
    std::ifstream file("www/index.html");
    if (!file) {
        return "<html><body><h1>File not found</h1></body></html>";
    }
    return std::string(std::istreambuf_iterator<char>(file), {});
}

int main() {
    const int PORT = 8080;
    const size_t THREADS = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4;

    // install signal handlers to allow Ctrl+C stop inside Docker/terminal
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // 0) create thread pool
    ThreadPool pool(THREADS);
    pool_ptr = &pool;

    // 1) create socket
    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2) set SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        ::close(server_fd);
        return 1;
    }

    // 3) bind to 0.0.0.0:PORT
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // -> 0.0.0.0
    addr.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        ::close(server_fd);
        return 1;
    }

    // 4) listen
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        ::close(server_fd);
        return 1;
    }

    std::cout << "✅ Server listening on http://0.0.0.0:" << PORT << "\n" << std::flush;

    while (true) {
        sockaddr_in client{};
        socklen_t clen = sizeof(client);
        int cfd = ::accept(server_fd, reinterpret_cast<sockaddr*>(&client), &clen);
        if (cfd < 0) {
            // accept interrupted? try again
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        // capture client fd into task and submit
        pool.submit([cfd] {
            // read request (simple single read; fine for small requests)
            char buf[8192];
            ssize_t n = ::read(cfd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                std::cout << "---- request begin ----\n" << buf << "---- request end ----\n" << std::flush;
            } else {
                ::close(cfd);
                return;
            }

            // load index.html
            std::string body = load_index_html();

            // build response
            std::string headers =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Connection: close\r\n"
                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                "\r\n";

            std::string resp = headers + body;
            ssize_t written = ::write(cfd, resp.c_str(), resp.size());
            (void)written; // ignore short writes for this example

            ::close(cfd);
        });
    }

    // unreachable in current form; cleanup would be here
    return 0;
}
