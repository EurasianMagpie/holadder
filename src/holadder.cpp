// holadder.cpp
//

#include <stdio.h>
#include "holadder.h"

void applyUfwRuleForNewPort(int portFrom, int portTo) {
    char szCmd[64] = {};
    sprintf(szCmd, "ufw deny %d/udp", portFrom);
    std::system(szCmd);
    sprintf(szCmd, "ufw allow %d/udp", portTo);
    std::system(szCmd);
}

bool modifyLadderConfByServiceName(const char* serviceName) {
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
    if (port < 3000 || port > 65353) {
        port = 3000;
    }
    char szPort[16] = {};
    sprintf(szPort, "%d", port + 1);
    v = szPort;
    conf.set("port", v);
    applyUfwRuleForNewPort(port, port + 1);

    std::cout << "    port\t" << port << " -> " << szPort << std::endl;

    return conf.save(szFilePath);
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

int main(int argc, char *argv[])
{
    std::cout << "Hello Holadder." << std::endl;
    const char* serviceName = nullptr;
    if (argc == 1) {
        std::cout << "    usage: holadder [openvpn service name]" << std::endl;
        return 0;
    }
    else {
        serviceName = argv[1];
    }

    if (!modifyLadderConfByServiceName(serviceName)) {
        return 0;
    }

    restartUfw();
    restartService(serviceName);

    return 0;
}
