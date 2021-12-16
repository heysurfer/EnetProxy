#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include "enet/include/enet.h"
#include "http.h"
#include "server.h"
#include "proton/rtparam.hpp"
#include <fstream>
#include "HTTPRequest.hpp"

server* g_server = new server();

void setgtserver() {
    try {
        std::ofstream unwrite("C:\\Windows\\System32\\drivers\\etc\\hosts");

        if (unwrite.is_open()) {
            unwrite << "";
            unwrite.close();
        }
    } catch (std::exception) {}
    http::Request request{ "http://growtopia1.com/growtopia/server_data.php" };

    const auto response = request.send("POST", "version=1&protocol=128", { "Content-Type: application/x-www-form-urlencoded" });

    rtvar var = rtvar::parse({ response.body.begin(), response.body.end() });

    var.serialize();
    if (var.get("server") == "127.0.0.1") {
        return;
    }
    if (var.find("server")) {
        g_server->m_server = var.get("server");
        g_server->m_port = std::stoi(var.get("port"));
    }
}

int main() {
#ifdef _WIN32
    SetConsoleTitleA("proxy by ama");
#endif
    printf("enet proxy by ama\n");

    std::thread http(http::run, "127.0.0.1", "17191");
    http.detach();
    printf("HTTP server is running.\n");
    setgtserver();
    enet_initialize();
    if (g_server->start()) {
        printf("Server & client proxy is running.\n");
        while (true) {
            g_server->poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }   
    else
        printf("Failed to start server or proxy.\n");
}
