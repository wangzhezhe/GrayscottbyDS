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
    void lock(MPI_Comm comm, std::string lockName){dspaces_lock_on_read(lockName.data(), &comm);}
    void unlock(MPI_Comm comm, std::string lockName){dspaces_unlock_on_read(lockName.data(), &comm);}
    void close(){ dspaces_finalize();}

protected:
    std::string m_serverMasterAddr;
};

#endif
