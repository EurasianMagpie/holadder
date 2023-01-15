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

bool modifyLadderConfFile(const char* filePath) {
    std::cout << "[-] modifyLadderConfFile:" << std::endl;
    LadderConfUtil::LadderConf conf;
    if (!conf.parse(filePath)) {
        std::cout << "    invalid conf file or path > " << filePath << std::endl;
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

    return conf.save(filePath);
}

void restartUfw() {
    std::system("ufw disable");
    std::system("echo \"y\" | ufw enable");
}

void restartService() {
    std::system("systemctl restart openvpn@x1");
}

int main(int argc, char *argv[])
{
    std::cout << "Hello Holadder." << std::endl;
    if (argc != 2) {
        std::cout << "    usage: holadder <conf file path>" << std::endl;
        return 0;
    }

    if (!modifyLadderConfFile(argv[1])) {
        return 0;
    }

    restartUfw();
    restartService();

    return 0;
}
