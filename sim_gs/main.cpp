#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <adios2.h>
#include <mpi.h>
#include <thread>

#include "gray-scott.h"
#include "writer.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#define BILLION 1000000000L

void print_io_settings(const adios2::IO &io)
{
    std::cout << "Simulation writes data using engine type:              "
              << io.EngineType() << std::endl;
}

void print_settings(const Settings &s)
{
    std::cout << "grid:             " << s.L << "x" << s.L << "x" << s.L
              << std::endl;
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "F:                " << s.F << std::endl;
    std::cout << "k:                " << s.k << std::endl;
    std::cout << "dt:               " << s.dt << std::endl;
    std::cout << "Du:               " << s.Du << std::endl;
    std::cout << "Dv:               " << s.Dv << std::endl;
    std::cout << "noise:            " << s.noise << std::endl;
    std::cout << "output:           " << s.output << std::endl;
    std::cout << "adios_config:     " << s.adios_config << std::endl;
}

void print_simulator_settings(const GrayScott &s)
{
    std::cout << "process layout:   " << s.npx << "x" << s.npy << "x" << s.npz
              << std::endl;
    std::cout << "local grid size:  " << s.size_x << "x" << s.size_y << "x"
              << s.size_z << std::endl;
}

int main(int argc, char **argv)
{

    MPI_Init(&argc, &argv);
    int rank, procs, wrank;

    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

    const unsigned int color = 1;
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, wrank, &comm);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);

    if (argc < 2)
    {
        if (rank == 0)
        {
            std::cerr << "Too few arguments" << std::endl;
            std::cerr << "Usage: gray-scott settings.json" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    //send request to timer that wf start

    Settings settings = Settings::from_json(argv[1]);

    GrayScott sim(settings, comm);
    sim.init();

    adios2::ADIOS adios(settings.adios_config, comm, adios2::DebugON);

    adios2::IO io_main = adios.DeclareIO("SimulationOutput");
    adios2::IO io_ckpt = adios.DeclareIO("SimulationCheckpoint");

    Writer writer_main(settings, sim, io_main);
    Writer writer_ckpt(settings, sim, io_ckpt);

    writer_main.open(settings.output);

    if (rank == 0)
    {
        print_io_settings(io_main);
        std::cout << "========================================" << std::endl;
        print_settings(settings);
        print_simulator_settings(sim);
        std::cout << "========================================" << std::endl;
    }

    for (int i = 0; i < settings.steps;)
    {

        for (int j = 0; j < settings.plotgap; j++)
        {
            sim.iterate();
            i++;
        }

#ifdef ENABLE_TIMERS
        MPI_Barrier(comm);
        struct timespec start, end;
        double diff;
        clock_gettime(CLOCK_REALTIME, &start); /* mark start time */
#endif

        if (rank == 0)
        {
            std::cout << "Simulation at step " << i
                      << " writing output step     " << i / settings.plotgap
                      << std::endl;
        }

        writer_main.write(i, sim);

#ifdef ENABLE_TIMERS

        MPI_Barrier(comm);
        clock_gettime(CLOCK_REALTIME, &end); /* mark end time */
        diff = (end.tv_sec - start.tv_sec) * 1.0 + (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;

        //caculate the avg
        double time_sum_write;
        MPI_Reduce(&diff, &time_sum_write, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

        if (rank == 0)
        {
            std::cout << "step " << i << " avg write " << time_sum_write / procs << std::endl;
        }

#endif
    }

    writer_main.close();

    MPI_Finalize();
}
