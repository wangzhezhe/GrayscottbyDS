#include <fstream>

#include "../utils/json.hpp"
#include "settings_syn.h"

/*
    std::cout << "grid:             " << s.L << "x" << s.L << "x" << s.L << std::endl;
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "cacutime:         " << s.dt << std::endl;
    std::cout << "output:           " << s.output << std::endl;
*/


void to_json(nlohmann::json &j, const Settings &s)
{
    j = nlohmann::json{{"L", s.L},
                       {"steps", s.steps},
                       {"plotgap", s.plotgap},
                       {"calcutime", s.calcutime},
                       {"output", s.output}};
}

void from_json(const nlohmann::json &j, Settings &s)
{
    j.at("L").get_to(s.L);
    j.at("steps").get_to(s.steps);
    j.at("plotgap").get_to(s.plotgap);
    j.at("calcutime").get_to(s.calcutime);
    j.at("output").get_to(s.output);

}

Settings::Settings()
{
    L = 128;
    steps = 20000;
    plotgap = 200;
    calcutime = 1;
    output = "foo.bp";
}

Settings Settings::from_json(const std::string &fname)
{
    std::ifstream ifs(fname);
    nlohmann::json j;

    ifs >> j;

    return j.get<Settings>();
}
