#include <fstream>

#include "../utils/json.hpp"
#include "settings_single.h"

/*
    std::cout << "grid:             " << s.L << "x" << s.L << "x" << s.L << std::endl;
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "cacutime:         " << s.dt << std::endl;
    std::cout << "output:           " << s.output << std::endl;
*/


void to_json(nlohmann::json &j, const Settings &s)
{
    j = nlohmann::json{{"lenArray", s.lenArray},
                       {"steps", s.steps},
                       {"plotgap", s.plotgap},
                       {"calcutime", s.calcutime},
                       {"output", s.output},
                       {"processLayout", s.processLayout}};
}

void from_json(const nlohmann::json &j, Settings &s)
{
    j.at("lenArray").get_to(s.lenArray);
    j.at("steps").get_to(s.steps);
    j.at("plotgap").get_to(s.plotgap);
    j.at("calcutime").get_to(s.calcutime);
    j.at("output").get_to(s.output);
    j.at("processLayout").get_to(s.processLayout);


}

Settings::Settings()
{
    steps = 20000;
    plotgap = 200;
    calcutime = 1.0;
    output = "foo.bp";
}

Settings Settings::from_json(const std::string &fname)
{
    std::ifstream ifs(fname);
    nlohmann::json j;

    ifs >> j;

    return j.get<Settings>();
}
