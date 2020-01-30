### start the ds

update the specific configuration

DSTASK=8

srun --mpi=pmix_v2 --mem-per-cpu=20000 --ntasks=$DSTASK --ntasks-per-node=1 --constraint=hal,broadwell,edr --time=00:10:00 ./dataspaces_server -s $DSTASK

### start the gray scott

srun --mpi=pmix_v2 --ntasks=8 --mem-per-cpu=25000 --constraint=hal,broadwell,edr --time=00:10:00 ./gray-scott ~/cworkspace/src/GrayscottbyDS/setting/settings.json

### start the isosurface

srun --mpi=pmix_v2 --mem-per-cpu=2000 --ntasks=4 --constraint=hal,broadwell,edr --time=00:10:00 ./isosurface ~/cworkspace/src/GrayscottbyDS/setting/settings.json 5 0.5
