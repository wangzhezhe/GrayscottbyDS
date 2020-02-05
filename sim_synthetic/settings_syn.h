#ifndef __SYN_SETTINGS_H__
#define __SYN_SETTINGS_H__

#include <string>

struct Settings {
    size_t L;
    int steps;
    int plotgap;
    int calcutime;
    std::string output;


    Settings();
    static Settings from_json(const std::string &fname);
};

#endif
