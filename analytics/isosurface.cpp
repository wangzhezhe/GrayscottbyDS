#include <iostream>
#include <sstream>

#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMarchingCubes.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLDataSetWriter.h>
#include "reader.h"
#include <thread>

void writeImageData(std::string fileName,
                    std::array<uint64_t, 3> &shape,
                    std::array<uint64_t, 3> &offset,
                    void* field)
{
    auto importer = vtkSmartPointer<vtkImageImport>::New();
    importer->SetDataSpacing(1, 1, 1);
    importer->SetDataOrigin(1.0 * offset[2], 1.0 * offset[1],
                            1.0 * offset[0]);
    importer->SetWholeExtent(0, shape[2] - 1, 0,
                             shape[1] - 1, 0,
                             shape[0] - 1);
    importer->SetDataExtentToWholeExtent();
    importer->SetDataScalarTypeToDouble();
    importer->SetNumberOfScalarComponents(1);
    importer->SetImportVoidPointer((double *)(field));
    importer->Update();

    // Write the file by vtkXMLDataSetWriter
    vtkSmartPointer<vtkXMLImageDataWriter> writer =
        vtkSmartPointer<vtkXMLImageDataWriter>::New();
    writer->SetFileName(fileName.data());

    // get the specific polydata and check the results
    writer->SetInputConnection(importer->GetOutputPort());
    //writer->SetInputData(importer->GetOutputPort());
    // Optional - set the mode. The default is binary.
    writer->SetDataModeToBinary();
    // writer->SetDataModeToAscii();
    writer->Write();
}

vtkSmartPointer<vtkPolyData>
    compute_isosurface(std::array<size_t, 3> &shape,
                       std::array<size_t, 3> &offset,
                       const std::vector<double> &field, double isovalue)
{
    // Convert field values to vtkImageData
    auto importer = vtkSmartPointer<vtkImageImport>::New();
    importer->SetDataSpacing(1, 1, 1);
    importer->SetDataOrigin(1.0 * offset[2], 1.0 * offset[1],
                            1.0 * offset[0]);
    importer->SetWholeExtent(0, shape[2], 0,
                             shape[1], 0,
                             shape[0]);
    importer->SetDataExtentToWholeExtent();
    importer->SetDataScalarTypeToDouble();
    importer->SetNumberOfScalarComponents(1);
    importer->SetImportVoidPointer(const_cast<double *>(field.data()));

    // Run the marching cubes algorithm
    auto mcubes = vtkSmartPointer<vtkMarchingCubes>::New();
    mcubes->SetInputConnection(importer->GetOutputPort());
    mcubes->ComputeNormalsOn();
    mcubes->SetValue(0, isovalue);
    mcubes->Update();

    // Return the isosurface as vtkPolyData
    return mcubes->GetOutput();
}

void writePolyvtk(const std::string &fname,
                  const vtkSmartPointer<vtkPolyData> polyData)
{
    auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetFileName(fname.c_str());
    writer->SetInputData(polyData);
    writer->Write();
}

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
    int npx = dims[0];
    int npy = dims[1];
    int npz = dims[2];

    int coords[3] = {0};
    int periods[3] = {0};
    MPI_Comm cart_comm;
    MPI_Cart_create(comm, 3, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 3, coords);
    uint64_t px = coords[0];
    uint64_t py = coords[1];
    uint64_t pz = coords[2];

    //TODO this info is loaded from the setting.json
    int shape[3] = {162, 162, 162};

    uint64_t size_x = (shape[0] + npx - 1) / npx;
    uint64_t size_y = (shape[1] + npy - 1) / npy;
    uint64_t size_z = (shape[2] + npz - 1) / npz;

    uint64_t offset_x = size_x * px;
    uint64_t offset_y = size_y * py;
    uint64_t offset_z = size_z * pz;

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

    if (argc != 3)
    {
        if (rank == 0)
        {
            std::cerr << "Too few arguments" << std::endl;
            std::cout << "Usage: isosurface steps isovalue" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    const double steps = std::stoi(argv[1]);
    const double isovalue = std::stod(argv[2]);

    std::vector<double> variableU;
    int step;

#ifdef ENABLE_TIMERS
    Timer timer_total;
    Timer timer_read;
    Timer timer_compute;
    Timer timer_write;

    std::ostringstream log_fname;
    log_fname << "isosurface_pe_" << rank << ".log";

    std::ofstream log(log_fname.str());
    //log << "step\ttotal_iso\tread_iso\tcompute_write_iso" << std::endl;
#endif

    //tl::engine clientEngine("verbs", THALLIUM_CLIENT_MODE);
    //std::string serverAddr = loadMasterAddr();

    //gray scott simulation start from 1

    Reader reader(MPI_COMM_WORLD, procs, 5);

    for (int step = 1; step <= steps; step++)
    {

#ifdef ENABLE_TIMERS

        MPI_Barrier(comm);
        timer_total.start();
        timer_read.start();
#endif

        //read data

#ifdef ENABLE_TIMERS
        double time_read = timer_read.stop();
        MPI_Barrier(comm);
        timer_compute.start();
#endif

        //get blockMeta, extract the shape, offset, variableName

        //get variable

        uint64_t lb[3] = {offset_x, offset_y, offset_z};
        uint64_t up[3] = {offset_x + size_x, offset_y + size_y, offset_z + size_z};

        void *data = (void *)malloc(size_x * size_y * size_z * sizeof(double));

        reader.read(MPI_COMM_WORLD, lb, up, step, data);

        char countstr[50];
        sprintf(countstr, "%02d_%04d", rank, step);
        std::string fname = "./vtkdata/vtkiso_" + std::string(countstr) + ".vti";
        
        std::array<uint64_t, 3> shape = {{size_x,size_y,size_z}};
        std::array<uint64_t, 3> offset = {{offset_x,offset_y,offset_z}};

        //writeImageData(fname, shape, offset, data);
        //writePolyvtk(fname, polyData);
        std::cout << "ok for ts " << step << std::endl;

        free(data);
    }

#ifdef ENABLE_TIMERS
    //log << "total\t" << timer_total.elapsed() << "\t" << timer_read.elapsed()
    //   << "\t" << timer_compute.elapsed() << std::endl;

    log.close();
#endif

    reader.close();

    MPI_Finalize();

    return 0;
}
