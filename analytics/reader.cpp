#include "reader.h"
#include "iostream"
#include <unistd.h>

Reader::Reader(MPI_Comm comm, int numproc, int appid)
{
	dspaces_init(numproc, appid, &comm, NULL);
}

void Reader::read(MPI_Comm comm, uint64_t *lb, uint64_t *ub, int step, void *data)
{

	MPI_Barrier(comm);

	std::string VarNameU = "grascott_u";

	while (true)
	{
		dspaces_lock_on_read("my_test_lock", &comm);
		int status = dspaces_get(VarNameU.data(), step, sizeof(double), 3, lb, ub, data);
		dspaces_unlock_on_read("my_test_lock", &comm);

		if (status != 0)
		{
			//sleep to wait the data put
			usleep(500000);
		}
		else
		{
			break;
		}
	}

	return;
}