#ifndef __SYN_SETTINGS_H__
#define __SYN_SETTINGS_H__

#include <string>
#include <vector>

struct Settings {
    std::vector<int> lenArray;
    int steps;
    int plotgap;
    double calcutime;
    std::string output;
    std::vector<int> processLayout;


    Settings();
    static Settings from_json(const std::string &fname);
};

#endif
