// The solver is based on Hiroshi Watanabe's 2D Gray-Scott reaction diffusion
// code available at:
// https://github.com/kaityo256/sevendayshpc/tree/master/day5

#include <mpi.h>
#include <random>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "../utils/json.hpp"
#include "sim_synthetic.h"

SynthSim::SynthSim(const Settings &settings, MPI_Comm comm)
    : settings(settings), comm(comm)
    {}

SynthSim::~SynthSim() {}

void SynthSim::init()
{
    init_mpi();
    init_field();
}

void SynthSim::iterate(int calcutime)
{
    this->calc(calcutime);
}

const std::vector<double> &SynthSim::u_ghost() const { return u; }

std::vector<double>
SynthSim::data_noghost(const std::vector<double> &data) const
{
    std::vector<double> buf(size_x * size_y * size_z);
    data_no_ghost_common(data, buf.data());
    return buf;
}

void SynthSim::data_noghost(const std::vector<double> &data,
                             double *data_no_ghost) const
{
    data_no_ghost_common(data, data_no_ghost);
}

void SynthSim::init_field()
{
    const int V = (size_x + 2) * (size_y + 2) * (size_z + 2);
    u.resize(V, 1.0);

    const int d = 6;
    for (int z = settings.L / 2 - d; z < settings.L / 2 + d; z++) {
        for (int y = settings.L / 2 - d; y < settings.L / 2 + d; y++) {
            for (int x = settings.L / 2 - d; x < settings.L / 2 + d; x++) {
                if (!is_inside(x, y, z)) continue;
                int i = g2i(x, y, z);
                u[i] = 0.5;
            }
        }
    }
}

void SynthSim::calc(int updating_time)
{
    //simulate data updating
    std::this_thread::sleep_for (std::chrono::seconds(updating_time));
    return;
}

void SynthSim::init_mpi()
{
    int dims[3] = {};
    const int periods[3] = {1, 1, 1};
    int coords[3] = {};

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);

    MPI_Dims_create(procs, 3, dims);
    npx = dims[0];
    npy = dims[1];
    npz = dims[2];

    MPI_Cart_create(comm, 3, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 3, coords);
    px = coords[0];
    py = coords[1];
    pz = coords[2];

    size_x = (settings.L + npx - 1) / npx;
    size_y = (settings.L + npy - 1) / npy;
    size_z = (settings.L + npz - 1) / npz;

    offset_x = size_x * px;
    offset_y = size_y * py;
    offset_z = size_z * pz;

    if (px == npx - 1) {
        size_x -= size_x * npx - settings.L;
    }
    if (py == npy - 1) {
        size_y -= size_y * npy - settings.L;
    }
    if (pz == npz - 1) {
        size_z -= size_z * npz - settings.L;
    }
}

void SynthSim::data_no_ghost_common(const std::vector<double> &data,
                                     double *data_no_ghost) const
{
    for (int z = 1; z < size_z + 1; z++) {
        for (int y = 1; y < size_y + 1; y++) {
            for (int x = 1; x < size_x + 1; x++) {
                data_no_ghost[(x - 1) + (y - 1) * size_x +
                              (z - 1) * size_x * size_y] = data[l2i(x, y, z)];
            }
        }
    }
}
