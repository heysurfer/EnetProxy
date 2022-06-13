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
std::string getMyIP()
{
    //https://stackoverflow.com/questions/122208/how-can-i-get-the-ip-address-of-a-local-computer
    std::string ip = "";
    char szBuffer[1024];

#ifdef WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    if (::WSAStartup(wVersionRequested, &wsaData) != 0)
        return "";
#endif


    if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return "";
    }

    struct hostent* host = gethostbyname(szBuffer);
    if (host == NULL)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return "";
    }

    //Obtain the computer's IP
    ip += std::to_string(((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1);
    ip += "." + std::to_string(((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2);
    ip += "." + std::to_string(((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3);
    ip += "." + std::to_string(((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4);

#ifdef WIN32
    WSACleanup();
#endif
    if(ip.empty())
	ip="127.0.0.5";
    return ip;
}

void startHTTPS()
{
    std::string temp = getenv("TEMP");
    std::string CertPem = decodeBase64("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUdrakNDQkhxZ0F3SUJBZ0lKQU9WMzlIMXJLVWJHTUEwR0NTcUdTSWIzRFFFQkJRVUFNSUdNTVFzd0NRWUQKVlFRR0V3SkJWVEVQTUEwR0ExVUVDQk1HUVZORVJrRlRNUkF3RGdZRFZRUUhFd2RCVTBSVFJFRlRNUTR3REFZRApWUVFLRXdWQlUwUkVRVEVOTUFzR0ExVUVDeE1FVTBGRVFURVNNQkFHQTFVRUF4TUpTR1Y1VTNWeVptVnlNU2N3CkpRWUpLb1pJaHZjTkFRa0JGaGhJWlhsVGRYSm1aWEpBY0hKdmRHOXViV0ZwYkM1amIyMHdIaGNOTWpJd05qRXgKTWpJeE9ERXpXaGNOTWpNd05qRXhNakl4T0RFeldqQ0JqREVMTUFrR0ExVUVCaE1DUVZVeER6QU5CZ05WQkFnVApCa0ZUUkVaQlV6RVFNQTRHQTFVRUJ4TUhRVk5FVTBSQlV6RU9NQXdHQTFVRUNoTUZRVk5FUkVFeERUQUxCZ05WCkJBc1RCRk5CUkVFeEVqQVFCZ05WQkFNVENVaGxlVk4xY21abGNqRW5NQ1VHQ1NxR1NJYjNEUUVKQVJZWVNHVjUKVTNWeVptVnlRSEJ5YjNSdmJtMWhhV3d1WTI5dE1JSUNJakFOQmdrcWhraUc5dzBCQVFFRkFBT0NBZzhBTUlJQwpDZ0tDQWdFQXZMOWk3QzMveXR3azVuZHh2U0tyaG4zNFJhNi80Qmg1bW05bDlnZGI4Y1RNd2Q2dkNvbTh5RkZxCjFEUDhrVUZXekFSOHlLdWh4bWxZQjQzV3AwTDNaSEFFRngzSHVpVUtDQXNFRHNEWlVldTQvYTg4eGxmYWNTYkYKRWs3c3RiRE5YdXZCY0I3TlltVVZOSHNxcEwzWldqc0sxT2U2TGN4aDl2RG5SdHdnSldFNmdhL3dxL2JuWDgrOQpTeXdHcytzWU9ZekxoeWxZR084ZUprQS9INFo4UVBEajJNUFNjQmFrbWFXWFVVRXpnd2IybDNzMlJNazdxbk5uCkRDVzVwT3JYVFI2dDVrY1lId29jaGdPdlZxZ1dmWExhV1RWd1ZXRWxrOUFtMHdHd2xPS3A0ai9QOWZvRW1kQmUKMGtzZHcrZlJuZDcvUk94bUlkbUVtV2RuWUZuKzMvRW92bG43R0tFTGZURFlHSE5vVEFaV2pjQk8ycEROeGVLdwpyZEt6di8yNmM0S29GOHRiZ1VwVUJSeCtjcVhaL1JPUjZ1TmtUT0VOMitvajBsWXhhU2RhU3NodkRsWVBRd2dQCmpGS1Vxc2FRWElZam9ETE5ZVkozWHNZVmZUVEZMR0I1elNYblNUNktBRHAvZjNrNFRxU092TFBzd1lSZk5FSXoKak04OCtRRWF3N0pjcWp6UmU0OWZnaEY0U1FrVzZRMDFNQnM0UGNWcGpNbmlnQktiUEEyOU9mOEk5NFlaZkZjNwpkYlVWckJFcXVhZ0h0bXpoUnpuY1YzM1lzb2I3VDk5VFRtUUVJSDFDU2FvK2ozMXFSZjNqSXNsNFBIZWxVclVDCnVndmoveGJPc0RBbisyWnk4cHNWRFdWN3pSUlc1dC96dFhDbncvb2dWbitXTS9TYmJOOENBd0VBQWFPQjlEQ0IKOFRBZEJnTlZIUTRFRmdRVXdhcGtaRUcxRDZkUW0rdmRIckZPZGw0NlUrb3dnY0VHQTFVZEl3U0J1VENCdG9BVQp3YXBrWkVHMUQ2ZFFtK3ZkSHJGT2RsNDZVK3FoZ1pLa2dZOHdnWXd4Q3pBSkJnTlZCQVlUQWtGVk1ROHdEUVlEClZRUUlFd1pCVTBSR1FWTXhFREFPQmdOVkJBY1RCMEZUUkZORVFWTXhEakFNQmdOVkJBb1RCVUZUUkVSQk1RMHcKQ3dZRFZRUUxFd1JUUVVSQk1SSXdFQVlEVlFRREV3bElaWGxUZFhKbVpYSXhKekFsQmdrcWhraUc5dzBCQ1FFVwpHRWhsZVZOMWNtWmxja0J3Y205MGIyNXRZV2xzTG1OdmJZSUpBT1YzOUgxcktVYkdNQXdHQTFVZEV3UUZNQU1CCkFmOHdEUVlKS29aSWh2Y05BUUVGQlFBRGdnSUJBRXUwc0l1Tm9USlJaMFpsOUVuMWUwbG9vZGlWRjlLOXB0TlIKbzdBcm5FaXk4SzNYeW5LMm92V1VDbGZ4cUlTVU8yNnliUFVEUER5SWs0cG56dElIUDN3YXpVWHhVRDMrbzAvQQpaaVNsWUUvVm9kRENxOFF1SE5yZmh3RnM4SHJuWlNWWkhGREtTaUdZa1FLaDUycTg4V3ZUNWlReUVuejlZSlgxCmZMRG1iNld6Z3JocFkzV2RUUDVYZnFwYjM2NnFXVGFacXNUNlRuUXh5SExzaGM3WHZGK3IrcE9DTS9IUWZ3WWUKYWZjSjR5ak9IY1kzZU5ZWEk5WDBIZDg3R3JsK01RVUN5MjVUdjB0ZGx5RFkrSWt5clBBdlp4MDRvY00yZTFmeApMYVMzakh3Y1doWm1uTzJpQXBsTy84eE10WVFsQ2NaaVRYQ2wxemRDTXBscnMrSVpqcXQveXlVWEJTdjFaNUZwCjY3ZjU4eWtLWkNxSmZNRjNGakJLUE5jQTBDN1hDYnU1V3ZvNTZWTlZYTGNBZ3IzWmNqSDVjT0FXdkJtN1RNcGkKS2xmQ3d5S3RUVG9sdHo3dDlIY0hiZXBScHB2SURKNlJYdFFKNlZROWJhbEtXdUhUTXpTbW4ydVdQdkd1RHdLdApUWTRweFhaMGp0NjQ4V1IwSDcvMFlWVFM2TXVZeVBaSEVpUTRSVjN6U1ZzV1ExejdLaTR2elAwN0pxU1pHYldsCmxWRW9KY2FMVGh5L3haNDlyTnVtVkJZN0tZZ2ZpbURuMXpOMnBqUjlYYkVsZmlqTzMvQ281SmQzNDE1VFJDRjkKazNCWE1lNC9mbk5qeTlSMlN2ajVwNkZEZGRjc3pkS1JOMjJ3WGNtaXpETEN6eWoxcnd5RTY5S1NJaDJxWURlaApRWHlROUxZRQotLS0tLUVORCBDRVJUSUZJQ0FURS0tLS0t");
    std::string KeyPem = decodeBase64("LS0tLS1CRUdJTiBSU0EgUFJJVkFURSBLRVktLS0tLQpNSUlKS0FJQkFBS0NBZ0VBdkw5aTdDMy95dHdrNW5keHZTS3JobjM0UmE2LzRCaDVtbTlsOWdkYjhjVE13ZDZ2CkNvbTh5RkZxMURQOGtVRld6QVI4eUt1aHhtbFlCNDNXcDBMM1pIQUVGeDNIdWlVS0NBc0VEc0RaVWV1NC9hODgKeGxmYWNTYkZFazdzdGJETlh1dkJjQjdOWW1VVk5Ic3FwTDNaV2pzSzFPZTZMY3hoOXZEblJ0d2dKV0U2Z2EvdwpxL2JuWDgrOVN5d0dzK3NZT1l6TGh5bFlHTzhlSmtBL0g0WjhRUERqMk1QU2NCYWttYVdYVVVFemd3YjJsM3MyClJNazdxbk5uRENXNXBPclhUUjZ0NWtjWUh3b2NoZ092VnFnV2ZYTGFXVFZ3VldFbGs5QW0wd0d3bE9LcDRqL1AKOWZvRW1kQmUwa3NkdytmUm5kNy9ST3htSWRtRW1XZG5ZRm4rMy9Fb3ZsbjdHS0VMZlREWUdITm9UQVpXamNCTwoycEROeGVLd3JkS3p2LzI2YzRLb0Y4dGJnVXBVQlJ4K2NxWFovUk9SNnVOa1RPRU4yK29qMGxZeGFTZGFTc2h2CkRsWVBRd2dQakZLVXFzYVFYSVlqb0RMTllWSjNYc1lWZlRURkxHQjV6U1huU1Q2S0FEcC9mM2s0VHFTT3ZMUHMKd1lSZk5FSXpqTTg4K1FFYXc3SmNxanpSZTQ5ZmdoRjRTUWtXNlEwMU1CczRQY1Zwak1uaWdCS2JQQTI5T2Y4SQo5NFlaZkZjN2RiVVZyQkVxdWFnSHRtemhSem5jVjMzWXNvYjdUOTlUVG1RRUlIMUNTYW8rajMxcVJmM2pJc2w0ClBIZWxVclVDdWd2ai94Yk9zREFuKzJaeThwc1ZEV1Y3elJSVzV0L3p0WENudy9vZ1ZuK1dNL1NiYk44Q0F3RUEKQVFLQ0FnQmw2d0pEZmVNdGF3b1IyRlYyUjQ1UEpSNDJvbEhCYy9YVnltbElIRmt0aDVMNDdJR2dNeGZaSXYySQpjRG5sUlY3VTgyZGVCQzlxcjk5MDFNWWZzeTFhOHBHQzJmWExNY3prNHUzaCtZaGZqK1ZvTm9PZXBqbXg5N2xhClZYdkQ4Q0ExTFNCYXBvZElwa1I0L2pqY2xCY1ZmZVF2YXFGQnNhY0ViTGkxcm84OUgrOHpzQWxKRWgyRnd1cjMKK0dIMkxPLzd3MmJ6S1FRRkVyazQ5bUFmYWNNV1hmenFRM01TUzJlOS80aGh1ODlRNVNXcVRrSFRpc1hKR0YrQQpVNjZXdXFoZllEMk9wcjBEcGJZWk4wd3Fnc0VDSkU1NjlUd2ZIa0hvT3ZGNzV1R3V3Mlh3WVh5dDJiODZQeVcwCmNCTTJnYXJTa0dOT21FZmQ1MUtPY3lPREI1Vno4RFlaeWsrUVJWb2VwZlF5M2xrYUxvUzNLTU1HU1cxL1FMMk8KdkJralY1YmFIdG4xc2xZUVpGODdEUGpnK1RSd3Z6ZUlDYk81azJRbUx4U2g4WkNMQTNnVmlibjZBQy9NQ29xago5Yk9WTElRZExhVmp1eStha0UrZ0pJTUp6VmpMRUdVb2NnRytySDdrOFluOUZoTjdYMEw3bEt6WnFRQnVGYnFvClFhZGVBZVAzVlJWa3Z2aGJYdUc3MjY0MUQ2WVBTWFhWbUZwQzdxTzh6S1QrbG9LWm14dUVxOWpzb2VJVDczUDQKdnpESnhrWHZXYTJmVmZCMW9rQ1NQbEl3RTROa1dSWGxLYTYxVlc4TGpzZWJ2Q2plM2FqWmNnOEUyeWRWVEdBRQpUZE8vK2l1TFRYTGk5Zm9DbWRBK2hZdDFpMnpVMDZiSkV6QmdvUHp2S3lva0xzOFJZUUtDQVFFQTlVMG9qMFNVCkdPbmo0cFQ5Z0J2d3MrZmxuODR2RzROUGJIMGRGOGVvVXRjZk1rd2R6eENVZTlXdHkvL2FLSUJGWEIyWGVKL08KTkNKelRacm9rZGc3aGF1dkI0NnMzRm5wb2IxdzIrTitJVDR3b05yanp5SG1KT2pHbGNDYzBNd1J0cjdnV0Z3bgpmbzhxL3c2b3hxRnFTVDZFdTcvRDZMWnhQZW9UeDkzMjNtRDI2N1B4SG83M3UrU2Z5aC9IeEJyUG5FWHF4Q1dPCi9OeGxxN3pBUjRrRnhRaVA0a0ZPWHBoY0hYOVprTzNPakVHMFBIek9FVnlYbVdrakhSMG1ISFRLMHluSVdSclMKQm9oSjlHcFd0WlByT1ZYMlk2UXV6N0d3TW5lTjhEa05vLzZqSkpRUHNCZUt2OWg5LzYvUzFhRFExRnF0WU1qSwprdmhoOCtqMHlQN2ZZd0tDQVFFQXhQckxBTG96Ny9kd1JmNml5Y1paUGNlWVVTaGtUT1d6anlPQ2JIS2ZKRzhNCitUUFBFaHpIM2I3bSsyU25sRkRSNzNraWVndzBVTDZwWUduaXptM2RFcGtveHJJNnIxOVIyL3hLK1lSRjl1SWUKRy9TTTBGU296NnZ1dy9FZHU1by9icm95RGJ6SUhpZ0ZhSGsyRWtjaUIzZTVvc0NpMGo4OHkxVzZPRnowME5tOApuemFyOHhRcnY5TUJGZSs0bFJaRCt5ZlJ3S0pGS0w3WE1Wa2xUanNuK2lLbk9TOFRhdEhZWFk4R2o0NXdEMGs1CkM1eU5McU8xZXpGbVZIMUovcTc1U0F0VG8zYjhiWnZlY3YzMWNvZE90a0pLNTVOY0IybFEweDgwc0ZlbXVJMkQKL3RSNWpWamlVUHlxamxLQ1hxUEw2Q2RPYThNcTdwLzBCaWVtL0xZTFZRS0NBUUJpem1lTi9kT2hqV1NMZWZQNgpURWJTVUpBL3BHOTVKNVV3WDl6K0hTQUI3a0RVbzdCR0hhbVJqK3BuaDNFNmF5Q0ZFQzVaR2hXWERtRmZXd3hlCjRyaG5DVmFCTnNrT0lQRm9nc1FZN2pONTlIRGwrZ1ZWQTVTT1BXendHUnVSUndIRG1ITUpaR1ZNSnlMc2hkMFoKMVI3WXplUHRORHNYVzVXVm1Obks0ZTkwZDU0K0pzYnl4aWVuZVZtYnN0MW1jbDQ1MWpmUU5raTROWmFHYklzTwp4WFNiZnUxbDkyUTMyaXNRZnFGM0xUTExjQlBIWVlZbm5DSThXd3NYeVF2MjNseHNYaUlqTWhoekpTYVUydGgrClgweWFWYlBmUXR6UU1BU0c3MzB1eSt4Z3FFclNpd2tlcHZ6dWRhZWEvQmx5ajBwT253L0RNbDNXZ1ZJRFMweCsKUVp0OUFvSUJBUUNBY2dWVCtSZUsvKy9QUktjRUtmbmhaamkxbTBBa1FKaWcyd2ZlRGJRbWZ0K1gxS2ZQYU1NUApucjloNkpXNkNpV0xJWk5Rd0lxUXFITXNNbis2R2p2Uk45dzJETnhscFNOczdvdzVQbjZjNlgrMnZQWGhsNExvCndIV1dnYkRndDBscnU3Q044cThQc3dzcnYvWjVYcmNhNVlCQVY1c1V2RFc1WmJKanRJSFRlbGdiL2Z1TUszZ0gKd3VEdlcvMGlBbjNiMVhCYVM3WUxScjV1S0hKQ0hDTUtZZlJuUG9KTDRvQlRLY09lY3NReHc0NldPc1I1U0RrOApGczlpZ3J4d2ZjcUJweGtuKzV2SklaMmhvR2FnQXdsWm9jclVUT3Z5elVMdUdHZzBJVVBQRGtDbk9BZjNHcUdoCks5RlBzdy9lQkxNZk5CcFgvTkVPLzNDUlh1UHQ0QXR0QW9JQkFEOTFobjFjOTlDeFE1ZDVsZ2tnNHVlYmRESVgKaFpxYWRvZDhUNGc4K2dWUzdwL2dqU045MlZtbTVxWWhkbm5kRFNlRVpIczZwMlNmWVNoVW12NGhNRmJoc2hjRgpzTnNIV0U3aUxuLzk4dS8ySzlzQXczbzVDblF2UlVra2NqSkxSRGlPWkFGaUpXQ3A5MEY1dFFQR3pqYkVMZHgvClBqMjU1NEsvKzR0MWFuNDhLZ3FBYWQ4bXhnbWM1MmRCalhQRk5LQldRV1R6Q3FoM1UrbFZYUGl4WStDZVJMSlcKL3Bwd0JMa1NWUWxNeVU4MHplbTl6RkdoeUFNZlNsQXlMY2QzRW1GWUc4REVUcHgvcFVhWTlHcDFseFJvUEtQRAplRHZDaVZrcS90VlJBWkIwdm9zU3FTZllwQXN3bkljSjRHUm9nOVVnMTh2Tk90ZG50RVEzdHVKY1JZYz0KLS0tLS1FTkQgUlNBIFBSSVZBVEUgS0VZLS0tLS0=");
    std::ofstream CertPemAppend;
    CertPemAppend.open(temp+ ("//cert.pem"), std::ios_base::trunc);
    CertPemAppend << CertPem;
    CertPemAppend.close();
    std::ofstream KeyPemAppend;
    KeyPemAppend.open(temp+ ("//key.pem"), std::ios_base::trunc);
    KeyPemAppend << KeyPem;
    KeyPemAppend.close();
    using namespace httplib;
    httplib::SSLServer svr(std::string{ temp + ("//cert.pem") }.c_str(), std::string{ temp + ("//key.pem") }.c_str());  
    svr.Get(("/growtopia/server_data.php"), [](const Request& req, Response& res) {
        res.set_content(("server|127.0.0.1\nport|17191\ntype|1\n#maint|Under mainteance.\nbeta_server|127.0.0.1\nbeta_port|1945\nbeta_type|1\nmeta|defined\nRTENDMARKERBS1001\nunknown"), ("text/html"));
        });
    svr.Post(("/growtopia/server_data.php"), [](const Request& req, Response& res) {
        res.set_content(("server|127.0.0.1\nport|17191\ntype|1\n#maint|Under mainteance.\nbeta_server|127.0.0.1\nbeta_port|1945\nbeta_type|1\nmeta|defined\nRTENDMARKERBS1001\nunknown"), ("text/html"));
        });
    remove(std::string{ temp + ("//cert.pem") }.c_str());
    remove(std::string{ temp + ("//key.pem") }.c_str());
    svr.listen(getMyIP().c_str(), 443);
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
        http::Request request{ "http://a104-125-3-135.deploy.static.akamaitechnologies.com/growtopia/server_data.php" };
        const auto response = request.send("POST", "version=3.9&protocol=160&platform=0", { "Host: www.growtopia1.com" });
        rtvar var = rtvar::parse({ response.body.begin(), response.body.end() });
#ifdef _WIN32
        var.serialize();
        if (var.find("server")) {
            g_server->m_port = var.get_int("port");
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
                  sethost <<  getMyIP() + " www.growtopia1.com\n"+getMyIP() + " www.growtopia2.com";
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

    std::thread httpS(startHTTPS);
    httpS.detach();
    printf("HTTPS server is running.\n");
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
