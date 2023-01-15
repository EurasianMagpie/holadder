//  ladder_conf.h
#ifndef _____LADDER_CONF_OF_HOLADDER_H_____
#define _____LADDER_CONF_OF_HOLADDER_H_____

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace LadderConfUtil {

    enum LineType {
        LT_EMPTY,
        LT_TEXT,
        LT_CONF,
    };

    struct LadderConfLine {
        LineType type;
        std::string text;
        std::pair<std::string, std::string> conf;
    };

    class LadderConf {
    public:
        bool parse(const char* path) {
            if (!path) {
                return false;
            }
            std::fstream fsConf;
            fsConf.open(path, std::ios::in);
            if (!fsConf.is_open()) {
                return false;
            }
            std::string es;
            while (std::getline(fsConf, es)) {
                lines.push_back(parseLine(es));
            }
            return true;
        }

        bool get(const char* key, std::string& outValue) {
            for (LadderConfLine& line : lines) {
                if (line.type == LT_CONF && line.conf.first.compare(key) == 0) {
                    outValue = line.conf.second;
                    return true;
                }
            }
            return false;
        }

        bool set(const char* key, std::string& inValue) {
            for (LadderConfLine& line : lines) {
                if (line.type == LT_CONF && line.conf.first.compare(key) == 0) {
                    line.conf.second = inValue;
                    return true;
                }
            }
            return false;
        }

        bool save(const char* path) {
            if (!path) {
                return false;
            }
            std::fstream fsConf;
            fsConf.open(path, std::ios::out);
            if (!fsConf.is_open()) {
                return false;
            }
            for (LadderConfLine& line : lines) {
                if (line.type == LT_TEXT) {
                    fsConf << line.text << std::endl;
                }
                else if (line.type == LT_CONF) {
                    fsConf << line.conf.first << " " << line.conf.second << std::endl;
                }
                else if (line.type == LT_EMPTY) {
                    fsConf << std::endl;
                }
            }
            return true;
        }

    private:
        LadderConfLine parseLine(std::string& lineText) {
            if (lineText.length() == 0) {
                return LadderConfLine{ LT_EMPTY, {}, {} };
            }
            else if (lineText.at(0) == '#') {
                return LadderConfLine{ LT_TEXT, lineText, {} };
            }
            else if (lineText.at(0) == ';') {
                return LadderConfLine{ LT_TEXT, lineText, {} };
            }
            else {
                std::string str(lineText);
                const char* toBeTrim = " \t";
                str.erase(str.find_last_not_of(toBeTrim) + 1);
                str.erase(0, str.find_first_not_of(toBeTrim));

                auto pos = str.find(' ');
                if (pos != std::string::npos) {
                    auto key = str.substr(0, pos);
                    str.erase(0, pos + 1);
                    if (key.length() && str.length()) {
                        return LadderConfLine{ LT_CONF, {}, {key, str} };
                    }
                }
                return LadderConfLine{ LT_TEXT, lineText, {} };
            }
        }

        

    private:
        std::vector<LadderConfLine> lines;
    };

}

#endif//_____LADDER_CONF_OF_HOLADDER_H_____