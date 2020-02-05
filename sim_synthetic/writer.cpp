#include "writer.h"
#include <array>
#include <iostream>
#include <unistd.h>

//init the writer instance
//init the staging client
Writer::Writer(MPI_Comm comm ,int proc, int appid)
{
    //this->m_serverMasterAddr = loadMasterAddr(); 
    //std::cout << "serverMaster addr " << m_serverMasterAddr << std::endl;
     dspaces_init(proc, appid, &comm, NULL);
}

//void Writer::open(const std::string &fname)
//{
//writer = io.Open(fname, adios2::Mode::Write);
// the key is fixed here
//io.SetParameters({{"verbose", "4"}, {"writerID", fname}});
//    return;
//}

//execute the data write operation
void Writer::write(const SynthSim &sim, MPI_Comm comm, int &step)
{

    std::vector<double> u = sim.u_ghost();

    std::string VarNameU = "grascott_u";
    std::array<size_t, 3> varuShape = {{sim.size_x, sim.size_y, sim.size_z}};
    std::array<size_t, 3> offSet = {{sim.offset_x, sim.offset_y, sim.offset_z}};

    uint64_t lb[10] = {0}, ub[10] = {0};

    lb[0] = sim.offset_x;
    lb[1] = sim.offset_y;
    lb[2] = sim.offset_z;

    ub[0] = sim.offset_x+sim.size_x-1;
    ub[1] = sim.offset_y+sim.size_y-1;
    ub[2] = sim.offset_z+sim.size_z-1;

    MPI_Barrier(comm);

    dspaces_lock_on_write("my_test_lock", &comm);
    int status = dspaces_put(VarNameU.data(),step, sizeof(double) ,3 ,lb ,ub ,u.data());
    //wait the put operation finish
    dspaces_put_sync();
    dspaces_unlock_on_write("my_test_lock", &comm);


    if (status!=0){
        std::cout << "error for the ts put at step " << step << "with status " << status << std::endl;
    }
}
