/**
 *  @file
 *  @ingroup radarelab 
*/
#ifndef RADARELAB_ELEV_FIN_H
#define RADARELAB_ELEV_FIN_H

#include <radarelab/volume.h>
#include <H5Cpp.h>

namespace radarelab {
namespace volume {

template<typename T>
struct ElevFin
{
    const Volume<T>* volume = 0;

    // elevazione finale in coordinate azimut range
    std::vector<unsigned char> elev_fin[NUM_AZ_X_PPI];

    std::vector<unsigned char>& operator[](unsigned idx) { return elev_fin[idx]; }
    const std::vector<unsigned char>& operator[](unsigned idx) const { return elev_fin[idx]; }

    void init(const Volume<T>& volume)
    {
        this->volume = &volume;

        // FIXME: set to 0 to have the right size. We start from 512 (MAX_BIN)
        // to allocate enough memory for legacy code that iterates on MAX_BIN
        // to successfully read zeroes
        unsigned max_size = 512;
        for (unsigned iel = 0; iel < volume.size(); ++iel)
        {
            if (volume.scan(iel).beam_size && volume.scan(iel).beam_size > max_size)
                max_size = volume.scan(iel).beam_size;
        }

        for (unsigned i = 0; i < NUM_AZ_X_PPI; ++i)
        {
            elev_fin[i].resize(max_size, 0);
        }
    }

    inline double elevation_rad_at_elev_preci(unsigned az_idx, unsigned ray_idx) const
    {
        return elevation_at_elev_preci(az_idx, ray_idx) * M_PI / 180.;
    }

    inline double elevation_at_elev_preci(unsigned az_idx, unsigned ray_idx) const
    {
        unsigned el = elev_fin[az_idx][ray_idx];
        return volume->scan(el).elevations_real(az_idx);
    }

    inline double db_at_elev_preci(unsigned az_idx, unsigned ray_idx) const
    {
        const PolarScan<T>& s = volume->scan(elev_fin[az_idx][ray_idx]);
        if (ray_idx < s.beam_size)
            return s.get(az_idx, ray_idx);
        else
            // If we are reading out of bounds, return 1 (the missing value)
            return MINVAL_DB;
    }

    void write_info_to_debug_file(H5::H5File out)
    {
        using namespace H5;

        // Compute dimensions
        hsize_t dims[2] = { NUM_AZ_X_PPI, 0 };
        for (unsigned i = 0; i < NUM_AZ_X_PPI; ++i)
            if (dims[1] < elev_fin[i].size())
                dims[1] = elev_fin[i].size();

        DataSpace file_data_space(2, dims);

        // Dataset data type
        IntType datatype( PredType::NATIVE_UCHAR );

        // Dataset fill value
        DSetCreatPropList props;
        unsigned char fill_value(0);
        props.setFillValue(datatype, &fill_value);

        // Create the dataset
        DataSet ds = out.createDataSet("/elev_fin", datatype, file_data_space, props);

        // Write elev_fin to it
        for (unsigned i = 0; i < NUM_AZ_X_PPI; ++i)
        {
            hsize_t mdims[1] = { elev_fin[i].size() };
            DataSpace memory_data_space(1, mdims);

            hsize_t count[] = { 1, elev_fin[i].size() };
            hsize_t start[] = { i, 0 };
            file_data_space.selectHyperslab(H5S_SELECT_SET, count, start);

            ds.write(elev_fin[i].data(), datatype, memory_data_space, file_data_space);
        }
    }
};

}
}

#endif
