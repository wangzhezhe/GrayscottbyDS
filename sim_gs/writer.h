#ifndef __WRITER_H__
#define __WRITER_H__

#include <mpi.h>
#include "gray-scott.h"
#include "settings_gs.h"
#include "dataspaces.h"
#include <vector>


class Writer
{
public:
    Writer(MPI_Comm comm ,int proc, int appid);
    //use this if write to the files
    //void open(const std::string &fname);
    void write(const GrayScott &sim, MPI_Comm comm, int &step);

    void close(){ dspaces_finalize();}

protected:
    std::string m_serverMasterAddr;
};

#endif
