/*
 * Analysis code for the Gray-Scott simulation.
 * Reads variable U and and extracts the iso-surface using VTK.
 * Writes the extracted iso-surface using ADIOS.
 *
 * Keichi Takahashi <keichi@is.naist.jp>
 *
 */
#include <iostream>
#include <sstream>
#include <adios2.h>
#include <thread>
#include "settings.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#define BILLION 1000000000L

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, procs, wrank;

    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

    const unsigned int color = 5;
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, wrank, &comm);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);

    int dims[3] = {0};
    MPI_Dims_create(procs, 3, dims);
    size_t npx = dims[0];
    size_t npy = dims[1];
    size_t npz = dims[2];

    int coords[3] = {0};
    int periods[3] = {0};
    MPI_Comm cart_comm;
    MPI_Cart_create(comm, 3, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 3, coords);
    size_t px = coords[0];
    size_t py = coords[1];
    size_t pz = coords[2];

    if (argc < 2)
    {
        if (rank == 0)
        {
            std::cerr << "Too few arguments" << std::endl;
            std::cout << "Usage: isosurface input" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    const std::string input_fname(argv[1]);
    const double isovalue = 0.5;

    adios2::ADIOS adios("adios2.xml", comm, adios2::DebugON);

    adios2::IO inIO = adios.DeclareIO("SimulationOutput");
    adios2::Engine reader = inIO.Open(input_fname, adios2::Mode::Read);

    std::vector<double> u;
    int step;

    while (true)
    {

#ifdef ENABLE_TIMERS
        MPI_Barrier(comm);
        struct timespec start, end;
        double diff;
        clock_gettime(CLOCK_REALTIME, &start); /* mark start time */
#endif
        adios2::StepStatus read_status = reader.BeginStep();

        if (read_status == adios2::StepStatus::OtherError)
        {
            std::cout << "---in iso surface adios status is unknown---" << read_status << std::endl;
            break;
        }

        if (read_status != adios2::StepStatus::OK)
        {
            // std::cout << "Stream not ready yet. Waiting...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "get status for iso " << read_status << std::endl;
            if (read_status == adios2::StepStatus::EndOfStream)
            {
                break;
            }
            continue;
        }

        adios2::Variable<double> varU = inIO.InquireVariable<double>("U");
        const adios2::Variable<int> varStep = inIO.InquireVariable<int>("step");

        adios2::Dims shape = varU.Shape();

        size_t size_x = (shape[0] + npx - 1) / npx;
        size_t size_y = (shape[1] + npy - 1) / npy;
        size_t size_z = (shape[2] + npz - 1) / npz;

        size_t offset_x = size_x * px;
        size_t offset_y = size_y * py;
        size_t offset_z = size_z * pz;

        if (px == npx - 1)
        {
            size_x -= size_x * npx - shape[0];
        }
        if (py == npy - 1)
        {
            size_y -= size_y * npy - shape[1];
        }
        if (pz == npz - 1)
        {
            size_z -= size_z * npz - shape[2];
        }

        varU.SetSelection({{offset_x, offset_y, offset_z},
                           {size_x + (px != npx - 1 ? 1 : 0),
                            size_y + (py != npy - 1 ? 1 : 0),
                            size_z + (pz != npz - 1 ? 1 : 0)}});

        reader.Get<double>(varU, u);
        reader.Get<int>(varStep, step);
        reader.EndStep();


#ifdef ENABLE_TIMERS
        MPI_Barrier(comm);
        clock_gettime(CLOCK_REALTIME, &end); /* mark end time */
        diff = (end.tv_sec - start.tv_sec) * 1.0 + (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;

        //caculate the avg
        double time_sum_read;
        MPI_Reduce(&diff, &time_sum_read, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

        if (rank == 0)
        {
            std::cout << "step " << step << " avg read " << time_sum_read / procs << std::endl;
        }
#endif

    }

    reader.Close();

    MPI_Finalize();

    return 0;
}
