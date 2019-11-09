#ifndef __READER_H__
#define __READER_H__

#include <mpi.h>
#include "dataspaces.h"
#include <string>


class Reader
{
public:
    Reader(MPI_Comm comm, int numproc, int appid);
    //use this if write to the files
    //void open(const std::string &fname);
    void read(MPI_Comm comm, uint64_t *lb, uint64_t *ub, int step, void *data);

    void close(){ dspaces_finalize();}

protected:
    std::string m_serverMasterAddr;
};

#endif
