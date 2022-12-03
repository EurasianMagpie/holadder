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
		return false;
	}
	std::string v;
	conf.get("port", v);
	int port = std::atoi(v.c_str());
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

int main()
{
	std::cout << "Hello Holadder." << std::endl;

	if (!modifyLadderConfFile("/etc/openvpn/x1.conf")) {
		return 0;
	}

	restartUfw();
	restartService();

	return 0;
}
