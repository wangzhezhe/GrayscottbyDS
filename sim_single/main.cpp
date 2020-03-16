#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "sim_single.h"
#include <stdio.h>
#include <unistd.h>

void print_settings(const Settings &s)
{
    std::cout << "grid:             " << s.lenArray[0] << "x" << s.lenArray[1] << "x" << s.lenArray[2] << std::endl;
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "calcutime:        " << s.calcutime << std::endl;
    std::cout << "output:           " << s.output << std::endl;
    std::cout << "grid:             " << s.processLayout[0] << "x" << s.processLayout[1] << "x" << s.processLayout[2] << std::endl;
}

void print_simulator_settings(const SingleSim &s)
{
    std::cout << "process layout:   " << s.npx << "x" << s.npy << "x" << s.npz
              << std::endl;
    std::cout << "local grid size:  " << s.size_x << "x" << s.size_y << "x"
              << s.size_z << std::endl;
}



int main(int argc, char **argv)
{

    if (argc < 2)
    {

        std::cerr << "Too few arguments" << std::endl;
        std::cerr << "Usage: synthetic-single setting_single.json" << std::endl;
    }

    Settings settings = Settings::from_json(argv[1]);

    std::vector<SingleSim *> simList;
    int npx = settings.processLayout[0];
    int npy = settings.processLayout[1];
    int npz = settings.processLayout[2];

    int totalNum = npx * npy * npz;

    for (int pz = 0; pz < npz; pz++)
    {
        for (int py = 0; py < npy; py++)
        {
            for (int px = 0; px < npx; px++)
            {
                int id = pz * npy *npx + py * npx + px;
                SingleSim *simInstance = new SingleSim(settings, id, totalNum);
                int coord[3] = {px, py, pz};
                simInstance->init(settings, coord, 1.0*id);
                simList.push_back(simInstance);
            }
        }
    }

    print_settings(settings);
    print_simulator_settings(*simList[0]);
    std::cout << "========================================" << std::endl;

    // go through all the ranks
    for (int t = 0; t < settings.steps; t++)
    {
        for (int pz = 0; pz < npz; pz++)
        {
            for (int py = 0; py < npy; py++)
            {
                for (int px = 0; px < npx; px++)
                {
                    int id = pz * npy *npx + py * npx + px;
                    SingleSim *simInstance = simList[id];
                    simInstance->iterate(settings.calcutime);
                    std::cout << " iterate step " << t << " rank " << id << std::endl;
                }
            }
        }
    }

    return 0;
}
