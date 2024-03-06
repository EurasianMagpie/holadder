// holadder.cpp
//

#include <stdio.h>
#include "holadder.h"

void applyUfwRuleForNewPort(int portFrom, int portTo) {
    char szCmd[64] = {};
    sprintf(szCmd, "ufw delete allow %d/udp", portFrom);
    std::system(szCmd);
    sprintf(szCmd, "ufw allow %d/udp", portTo);
    std::system(szCmd);
}

bool modifyLadderConfByServiceName(const char* serviceName, short expectedPort) {
    char szFilePath[1024] = {};
    sprintf(szFilePath, "/etc/openvpn/%s.conf", serviceName);
    std::cout << "[-] modifyLadderConf:" << serviceName << " File: " << szFilePath << std::endl;
    LadderConfUtil::LadderConf conf;
    if (!conf.parse(szFilePath)) {
        std::cout << "    invalid conf file or path > " << szFilePath << std::endl;
        return false;
    }
    std::string v;
    conf.get("port", v);
    int port = std::atoi(v.c_str());

    char szPort[16] = {};
    sprintf(szPort, "%d", expectedPort);
    v = szPort;
    conf.set("port", v);
    applyUfwRuleForNewPort(port, expectedPort);

    std::cout << "    port\t" << port << " -> " << expectedPort << std::endl;

    return conf.save(szFilePath);
}

short getBasePortFromServiceName(const char* serviceName) {
    char szFilePath[1024] = {};
    sprintf(szFilePath, "/etc/openvpn/%s.conf", serviceName);
    std::cout << "[-] get base port from :" << serviceName << " File: " << szFilePath << std::endl;
    LadderConfUtil::LadderConf conf;
    if (!conf.parse(szFilePath)) {
        std::cout << "    invalid conf file or path > " << szFilePath << std::endl;
        return false;
    }
    std::string v;
    conf.get("port", v);
    int port = std::atoi(v.c_str());
    if (port < 9000 || port > 65353) {
        return 9000;
    }
    else {
        return port;
    }
}

void restartUfw() {
    std::system("ufw disable");
    std::system("echo \"y\" | ufw enable");
}

void restartService(const char* serviceName) {
    char szCmd[128] = {};
    sprintf(szCmd, "systemctl restart openvpn@%s", serviceName);
    std::system(szCmd);
}


const char* usage = "    usage: holadder [service name] [batch count]";

int main(int argc, char *argv[])
{
    std::cout << "Hello Holadder." << std::endl;
    const char* serviceName = nullptr;
    int batchCount = 1;
    if (argc < 3) {
        std::cout << usage << std::endl;
        return 0;
    }

    serviceName = argv[1];
    batchCount = atoi(argv[2]);
    if (batchCount < 1) {
        std::cout << "    invalid batch count" << std::endl;
        std::cout << usage << std::endl;
        return 0;
    }

    short basePort = 0;
    for (int i = 0; i < batchCount; i++) {
        char szServiceName[16] = {};
        sprintf(szServiceName, "%s%d", serviceName, i);

        if (basePort == 0) {
            basePort = getBasePortFromServiceName(szServiceName) + batchCount;
            std::cout << "    using base port:" << basePort << " from " << szServiceName << std::endl;
        }

        modifyLadderConfByServiceName(szServiceName, basePort++);
        restartService(szServiceName);
    }
    restartUfw();
    return 0;
}
