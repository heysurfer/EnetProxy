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
#ifdef _WIN32
BOOL WINAPI exit_handler(DWORD dwCtrlType) {
    try {
        std::ofstream clearhost("C:\\Windows\\System32\\drivers\\etc\\hosts");

        switch (dwCtrlType) {
            case CTRL_BREAK_EVENT || CTRL_CLOSE_EVENT || CTRL_C_EVENT:
                if (clearhost.is_open()) {
                    clearhost << "";
                    clearhost.close();
                }
                return TRUE;

            default: return FALSE;
        }
   
        return TRUE;
    }
    catch(int e) {}
}
 #endif
void setgtserver() {
#ifdef _WIN32
    try {
        std::ofstream unwrite("C:\\Windows\\System32\\drivers\\etc\\hosts");

        if (unwrite.is_open()) {
            unwrite << "";
            unwrite.close();
        }
    } catch (std::exception) {}
 #endif
    try
    {
        http::Request request{ "http://api.surferstealer.com/system/growtopiaapi?getall" };

        const auto response = request.send("POST", "version=1&protocol=158", { "Content-Type: application/x-www-form-urlencoded" });


        rtvar var = rtvar::parse({ response.body.begin(), response.body.end() });
#ifdef _WIN32
        var.serialize();
        if (var.get("server") == "127.0.0.1") {
            return;
        }
        if (var.find("server")) {
            g_server->m_server = var.get("server");
            g_server->m_port = std::stoi(var.get("port"));
            g_server->meta = var.get("meta");
        }
 #endif
#ifdef __linux__ 
        //rtvar crashing on linux idk why
std::istringstream f({ response.body.begin(), response.body.end() });
	std::string line;
	while (std::getline(f, line)) {
		if (line.find("server|2") != -1)
		{
			utils::replace(line, "server|", "");
			g_server->m_server=line;
			continue;
		}
		if (line.find("port|1") != -1)
		{
			utils::replace(line, "port|", "");
			g_server->m_port=(stoi(line));
			break;
		}
        if (line.find("meta|") != -1)
		{
			utils::replace(line, "meta|", "");
			g_server->meta=line;
			break;
		}
	}
 #endif
#ifdef _WIN32
        try {
            std::ofstream sethost("C:\\Windows\\System32\\drivers\\etc\\hosts");

            if (sethost.is_open()) {
                sethost << "127.0.0.1 growtopia1.com\n127.0.0.1 growtopia2.com";
                sethost.close();
            }
        }
        catch (std::exception) {}
#endif
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}

int main() {
#ifdef _WIN32
    SetConsoleTitleA("proxy by ama");
    SetConsoleCtrlHandler(exit_handler, true);//auto host
#endif
    printf("enet proxy by ama\n");
    
    setgtserver(); //parse ip & port

    std::thread http(http::run, "127.0.0.1", "17191");
    http.detach();
    printf("HTTP server is running.\n");
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
