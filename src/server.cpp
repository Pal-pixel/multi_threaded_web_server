#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

int main() {
    const int PORT = 8080;

    // 1) create socket
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2) reuse addr/port for quick restarts
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return 1;
    }

    // 3) bind to 0.0.0.0:8080
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 4) listen
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "✅ Server listening on http://localhost:" << PORT << std::endl;

    // 5) main loop: accept one client at a time (blocking)
    while (true) {
        sockaddr_in client{};
        socklen_t clen = sizeof(client);
        int cfd = ::accept(server_fd, reinterpret_cast<sockaddr*>(&client), &clen);
        if (cfd < 0) {
            perror("accept");
            continue; // try next
        }

        // 6) read request (basic)
        char buf[65536];
        ssize_t n = ::read(cfd, buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read");
            ::close(cfd);
            continue;
        }
        buf[n] = '\0';
        std::cout << "---- request begin ----\n" << buf << "\n---- request end ----\n";

        // 7) build a minimal HTTP response (CRLF line endings)
        std::string body = "<html><body><h1>Hello from single-threaded server 👋</h1></body></html>";
        std::string headers =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n";

        // 8) send response
        std::string resp = headers + body;
        ssize_t sent = 0;
        while (sent < (ssize_t)resp.size()) {
            ssize_t m = ::write(cfd, resp.data() + sent, resp.size() - sent);
            if (m <= 0) break;
            sent += m;
        }

        // 9) close client
        ::close(cfd);
    }

    // (unreachable in this demo)
    ::close(server_fd);
    return 0;
}
