#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include "enet/include/enet.h"
#include "httplib.h"
#include "server.h"
#include "proton/rtparam.hpp"
#include <fstream>
#include "HTTPRequest.hpp"
#pragma comment( lib, "Advapi32.lib" )
#pragma comment( lib, "User32.lib" )
#pragma comment( lib, "winmm.lib" )

std::string decodeBase64(const std::string& base64Text)
{
    const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t DECODED_ALPHBET[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0 };

    if (base64Text.empty())
        return "";

    assert((base64Text.size() & 3) == 0 && "The base64 text to be decoded must have a length devisible by 4!");

    uint32_t numPadding = (*std::prev(base64Text.end(), 1) == '=') + (*std::prev(base64Text.end(), 2) == '=');

    std::string decoded((base64Text.size() * 3 >> 2) - numPadding, '.');

    union
    {
        uint32_t temp;
        char tempBytes[4];
    };
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(base64Text.data());

    std::string::iterator currDecoding = decoded.begin();

    for (uint32_t i = 0, lim = (base64Text.size() >> 2) - (numPadding != 0); i < lim; ++i, bytes += 4)
    {
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12 | DECODED_ALPHBET[bytes[2]] << 6 | DECODED_ALPHBET[bytes[3]];
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        (*currDecoding++) = tempBytes[0];
    }

    switch (numPadding)
    {
    case 2:
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12;
        (*currDecoding++) = tempBytes[2];
        break;

    case 1:
        temp = DECODED_ALPHBET[bytes[0]] << 18 | DECODED_ALPHBET[bytes[1]] << 12 | DECODED_ALPHBET[bytes[2]] << 6;
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        break;
    }

    return decoded;
}


void startHTTPS()
{
    using namespace httplib;
    std::string temp = getenv("TEMP");
    std::string CertPem = decodeBase64("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUR6akNDQXJhZ0F3SUJBZ0lKQUtjT0JBRlcrT0JWTUEwR0NTcUdTSWIzRFFFQkN3VUFNSE14Q3pBSkJnTlYKQkFZVEFsUlNNUXN3Q1FZRFZRUUlFd0pZUkRFU01CQUdBMVVFQnhNSlNHVjVVM1Z5Wm1WeU1SSXdFQVlEVlFRSwpFd2xJWlhsVGRYSm1aWEl4RWpBUUJnTlZCQXNUQ1VobGVWTjFjbVpsY2pFYk1Ca0dBMVVFQXhNU2QzZDNMbWR5CmIzZDBiM0JwWVRFdVkyOXRNQjRYRFRJeU1EWXhOekUwTURNd00xb1hEVEkwTURZeE5qRTBNRE13TTFvd2N6RUwKTUFrR0ExVUVCaE1DVkZJeEN6QUpCZ05WQkFnVEFsaEVNUkl3RUFZRFZRUUhFd2xJWlhsVGRYSm1aWEl4RWpBUQpCZ05WQkFvVENVaGxlVk4xY21abGNqRVNNQkFHQTFVRUN4TUpTR1Y1VTNWeVptVnlNUnN3R1FZRFZRUURFeEozCmQzY3VaM0p2ZDNSdmNHbGhNUzVqYjIwd2dnRWlNQTBHQ1NxR1NJYjNEUUVCQVFVQUE0SUJEd0F3Z2dFS0FvSUIKQVFEYWI5YTFSUDV1ZW5iaitNV3B0UHRWMHVhRlU1Vjc5WFJoYUl5akhyd2hhTUUxM242bHVvdWd0djNJTFhqSwp1UTJlM3ZoR0R2RTROVVBlU0JhRkw3ZFVVcnViWGZ0ZEFCQjJvdVV0Tis0SnBZNE11QnlJTWNHcjQxQVFVakVhCjl6SlJDQlFJSGhpOGxQUS9MMS8zTXFwY1ZxQmpUNTFPRC9qYUI2UU1iSGVzaEN1cjlIVGo5RXE4ckhZOHRFTDIKTUJMY29JeWpXM1VkTGJIVEI5NUk2aVUyTmlha0F0VG9weXpmaXV4bEJjRE9yM2l0SDBuek9qcjY0RWoraUpidgo5MG5zRXRRYTNFYkVrMEtMY1RNYS9xdFZGY1BpeHpJUklVSVJZY1lXeGErN3RkcUUrOThRUERWYmoxbDY5NDIwCkd2YUFqRUdJMlYzUXp2MFBxU2dZbnMwTkFnTUJBQUdqWlRCak1BNEdBMVVkRHdFQi93UUVBd0lEaURBVEJnTlYKSFNVRUREQUtCZ2dyQmdFRkJRY0RBVEE4QmdOVkhSRUVOVEF6Z2hKM2QzY3VaM0p2ZDNSdmNHbGhNUzVqYjIyQwpFbmQzZHk1bmNtOTNkRzl3YVdFeUxtTnZiWUlKTVRJM0xqQXVNQzR4TUEwR0NTcUdTSWIzRFFFQkN3VUFBNElCCkFRQkJnTDZkZExJUVZpRElGTzIzNEZCWE8vZjRKOFNkVEN0YXh2OEpnM2dvVGtWbXJPamphcXB4K2FvTitRdEkKMXJqVllQTUNOVVNoYjcrZFEzTmVsTjNvRFRmS2tuNWtoNkpSdlhOOFVxeGI5eXpsMmNFbnhSNmpKajB4OWdGdgo5N21lZWVoN1ErTko4MkY3b28wYW5RVWk1MWVYWHdHUDlBS1RDK3NiUStqWWVLSlpYWDdkOTJ3dVM3MnhUY0V4Ci8yT0RyT01aYWszUkJlUUtDaHR2eUIwNGphWEFGdzdURENMbTRwRktSaUhMdUhNY1lVQ2IrcVNDMnJPSUh6VGMKS0dLZzBjUGgxVUhkdThoUVpPWTVicmR5V2RBTFpzK3cxekZ0UFhrMWZ5Y2xJdHV6MkZlcVdvM2FYN0pIRG1zeAppM3VreEF1Q3l5bVA2eU5qQm9PVmVhYVUKLS0tLS1FTkQgQ0VSVElGSUNBVEUtLS0tLQ==");
    std::string KeyPem = decodeBase64("LS0tLS1CRUdJTiBSU0EgUFJJVkFURSBLRVktLS0tLQpNSUlFb2dJQkFBS0NBUUVBMm0vV3RVVCtibnAyNC9qRnFiVDdWZExtaFZPVmUvVjBZV2lNb3g2OElXakJOZDUrCnBicUxvTGI5eUMxNHlya05udDc0Umc3eE9EVkQza2dXaFMrM1ZGSzdtMTM3WFFBUWRxTGxMVGZ1Q2FXT0RMZ2MKaURIQnErTlFFRkl4R3ZjeVVRZ1VDQjRZdkpUMFB5OWY5ektxWEZhZ1kwK2RUZy80Mmdla0RHeDNySVFycS9SMAo0L1JLdkt4MlBMUkM5akFTM0tDTW8xdDFIUzJ4MHdmZVNPb2xOalltcEFMVTZLY3MzNHJzWlFYQXpxOTRyUjlKCjh6bzYrdUJJL29pVzcvZEo3QkxVR3R4R3hKTkNpM0V6R3Y2clZSWEQ0c2N5RVNGQ0VXSEdGc1d2dTdYYWhQdmYKRUR3MVc0OVpldmVOdEJyMmdJeEJpTmxkME03OUQ2a29HSjdORFFJREFRQUJBb0lCQUI4bllrVDZNUnVLcGRnLwp5OWszY2IwODFobmY0T3NNQit3NG9BNUh2T2M2N0l1RlR5VE41VW9ucnl4VXAreXAxZko1dElreGFsL3M0T0FjCkFmSSs2dlBBMVBjRXdXdnpMV1h1TjZkcVdhM1FpZUR3aFVrN1ozYmZkYlRPTkNpM1p0cTl2eldsTFR3QU5wR28KYlJSeGluQ2Uva01Md05DNFlIS2dNbHAvUWRZOXhBTUxoOHRjeDN6N0FsUjIyeEpUY3VaeVpNZXQvanZ6bXhTQQpHb3FtYm94SW5tUnZNSjdaRHc4R25zTm5LZ1VBOG1xWnViZ2FOTnNDZHE4c3RyRE5QdER2TmRTRXlrMmJnWjFOCmZuR0VKMmY5MW03cWR6RXFLZGY1YnBwRWEya2d0Y0oyRHRlWlVxd0NUbmgwNndCUUtZcWV1Y0RhLy9RdDhJVngKYmtCYXo0RUNnWUVBOTJRczFwVDdpYkU0MmxqdElyMzQxMjgwWm1pOHE0aEkxaE5WT3haWm4yRThWZjBwK2p1RAordU9VQlMwL3IxN2M2azJuQ3JYYTVYdkFDTVNjSFBLVFJDY1VBNWN3MFdEcTRmS2FmVm8yNnhSU0FHSG9Gd1BCClNoUXk4enFrMnIvTVc5ZjdPcU4rck5QZnF1cnB6TnBBZVpaL3UvWXM0VllJQm52Z1UwL0xnK0VDZ1lFQTRnbTYKNHh4Nm9tQlFtUlZLaFdNNmtUMEUzSGhISlkyNCt6QVlWR2hiTVNlVExBU3AvYlNEaGVaeUtEWEd2Y0tzYTM0UApDZ2RXUlFDNEx0OHp4YnR3NTlRNC9uckhabENTdS85Z3owQmQ1WTFXRDYxdmNKKzZwdFZNT1J2NFhGS2w1elovClkwYnBrUW5yUkJ3M25pUzZvNTdBTGcwR1dtQkVvVEpyZU9ic2JxMENnWUJ3UExXUEFQYUJ5TEtYZFVMWXdVRjEKVkJGODZNVzRPTk42dERpMTN2VDROeUF0anZjTmZSVHFyWGRKUmZjZnREVWI0L0VHRGUxcXNkTTA1eVpBaDlsQwpVVXhtT0tEQVRXMGk1M01wcmRVK24vQjRGZk03QmN3YXRNRk0wbTFhaFN2TSsxY1Npbng5SW43V1IwK2RUZU4wCmhsQWJVWnVZKy9RV0pQdG9NTXFQWVFLQmdDc1dxYjZUZGprdjNRMWhocVFveDBoYWRtdkVyZU5Wd2RaNFU1cjcKamE2d0daa0JocG9yYUFzRlkrdVFYTU5kc2RxSDNEd1FLL3paWjBMZ0g1Rm82dHYyazZySEl1MjVIRStrSGdORQpCT0kyY0JwcStGeGl4b1Q1RWgrczJrcFhJdk1SYTNVMFZsL2tvU21KcTN5RkNlTVk1dytnUWY3R2JTN0JXc1ZnClY5KzlBb0dBSWQ3Z2Q4UTJmcEl1TWIzRkZkMFhOWUtmK0RaY0dWUXFEcFRtek4ydjY1STg1V3RKZGV3cHhqQnIKWFhHSDBZY3Z1SWRMRFNhZWlOc1VoY3lENmdLbGxNbE5nNnhXU2RpcUUzblI1Z3ErLys4dnNETXh3UmdwTnIrTApxeXRaZGsrRVZ1SUJSUFRzMmpjL2ZJY3VXSlVRanUraEZqV2VhQi9pVWhoRll4Y0tyN1E9Ci0tLS0tRU5EIFJTQSBQUklWQVRFIEtFWS0tLS0t");
    std::ofstream CertPemAppend;
    CertPemAppend.open(temp+ ("//cert.pem"), std::ios_base::trunc);
    CertPemAppend << CertPem;
    CertPemAppend.close();
    std::ofstream KeyPemAppend;
    KeyPemAppend.open(temp+ ("//key.pem"), std::ios_base::trunc);
    KeyPemAppend << KeyPem;
    KeyPemAppend.close();
    SSLServer svr(std::string{ temp + ("//cert.pem") }.c_str(), std::string{ temp + ("//key.pem") }.c_str());  
    svr.Post(("/growtopia/server_data.php"), [](const Request& req, Response& res) {
        res.set_content(("server|127.0.0.1\nport|17191\ntype|1\n#maint|Under mainteance.\nbeta_server|127.0.0.1\nbeta_port|1945\nbeta_type|1\nmeta|defined\nRTENDMARKERBS1001\nunknown"), ("text/html"));
        });
    remove(std::string{ temp + ("//cert.pem") }.c_str());
    remove(std::string{ temp + ("//key.pem") }.c_str());
    svr.listen("127.0.0.1", 443);
}

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
    try
    {
       using namespace httplib;
        Headers Header;
        //Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
        //Header.insert(std::make_pair("Host", "www.growtopia1.com"));
        Client cli("https://api.surferstealer.com");
        cli.set_default_headers(Header);
        cli.enable_server_certificate_verification(false);
        cli.set_connection_timeout(2, 0);
        auto res = cli.Get("/system/growtopiaapi?CanAccessBeta=1");
        if (res.error() == Error::Success)
        {
            rtvar var = rtvar::parse({ res->body });
            g_server->m_port = (var.find("port") ? var.get_int("port") : g_server->m_port);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
	
   std::ofstream sethost("C:\\Windows\\System32\\drivers\\etc\\hosts");

    if (sethost.is_open()) {
	  sethost << "127.0.0.1 www.growtopia1.com\n127.0.0.1 www.growtopia2.com";
	sethost.close();
    }
}

int main() {
#ifdef _WIN32
    SetConsoleTitleA("proxy by ama");
    SetConsoleCtrlHandler(exit_handler, true);//auto host
#endif
    printf("enet proxy by ama\n");
    setgtserver(); //parse ip & port
    std::thread httpS(startHTTPS);
    httpS.detach();
    printf("HTTPS server is running.\n");
    enet_initialize();
    g_server->unlockThread();
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
