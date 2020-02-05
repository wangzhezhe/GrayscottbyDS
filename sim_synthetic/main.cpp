#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <mpi.h>
#include <thread>

#include "sim_synthetic.h"
#include "writer.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>


#define BILLION 1000000000L
//#include "../putgetMeta/metaclient.h"
//#include <thallium.hpp>

//namespace tl = thallium;

static MPI_Comm gcomm_;

void print_settings(const Settings &s)
{
    std::cout << "grid:             " << s.L << "x" << s.L << "x" << s.L << std::endl;
    std::cout << "steps:            " << s.steps << std::endl;
    std::cout << "plotgap:          " << s.plotgap << std::endl;
    std::cout << "calcutime:        " << s.calcutime << std::endl;
    std::cout << "output:           " << s.output << std::endl;
}

void print_simulator_settings(const SynthSim &s)
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

    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);

    if (argc < 2)
    {
        if (rank == 0)
        {
            std::cerr << "Too few arguments" << std::endl;
            std::cerr << "Usage: synthetic-sim setting_syn.json" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    Settings settings = Settings::from_json(argv[1]);

    SynthSim sim(settings, comm);
    sim.init();

    if (rank == 0)
    {
        print_settings(settings);
        print_simulator_settings(sim);
        std::cout << "========================================" << std::endl;
    }

#ifdef ENABLE_TIMERS
    //Timer timer_total;
    //Timer timer_compute;
    //Timer timer_write;

    //std::ostringstream log_fname;
    //log_fname << "gray_scott_pe_" << rank << ".log";

    //std::ofstream log(log_fname.str());
    //log << "step\ttotal_gs\tcompute_gs\twrite_gs" << std::endl;

#endif

    //Init the writer
    Writer dswriter(MPI_COMM_WORLD, procs, 1);

    //this is used for the distributed timer
    //tl::engine globalclientEngine("verbs", THALLIUM_CLIENT_MODE);

    for (int i = 0; i < settings.steps;)
    {

        for (int j = 0; j < settings.plotgap; j++)
        {
            sim.iterate(settings.calcutime);
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

        // the simulation start from the step 1 in this case
        size_t step = i;

        //send record to the metadata server
        //if (rank == 0)
        //{
        //MetaClient *metaclient = new MetaClient(&globalclientEngine);
        //std::string recordKey = "Trigger_" + std::to_string(step);
        //metaclient->Recordtime(recordKey);
        //}

        dswriter.write(sim, MPI_COMM_WORLD, i);

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

        //if (rank < 5)
        //{
        //    log << i << "\t" << time_total << "\t" << time_compute << "\t"
        //        << time_write << std::endl;
        //}

#endif

        //if the inline engine is used, read data and generate the vtkm data here
        //the adis needed to be installed before using
    }

#ifdef ENABLE_TIMERS
    //log << "total\t" << timer_total.elapsed() << "\t" << timer_compute.elapsed()
    //    << "\t" << timer_write.elapsed() << std::endl;

    //log.close();

#endif

    //dswriter.close();
    MPI_Finalize();
}
