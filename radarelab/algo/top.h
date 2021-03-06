/**
 *  @file
 *  @ingroup radarelab 
*/
#ifndef RADARELAB_ALGO_TOP_H
#define RADARELAB_ALGO_TOP_H

#include <radarelab/volume.h>

namespace radarelab {
namespace algo {

template<typename T>
void compute_top(const Volume<T>& volume, T threshold, Matrix2D<unsigned char>& top)
{
    top.fill(0);

    for (unsigned l=0; l < volume.size(); ++l)
    {
        const auto& scan = volume[l];
        for (unsigned i = 0; i < scan.beam_count; ++i)
        {
            const double elevaz = scan.elevations_real(i); //--- elev reale in gradi
            for (unsigned k = 0; k < scan.beam_size; ++k)
                if (scan.get(i, k) > threshold)
                    //top in ettometri
                    top(i, k) = (unsigned char)(PolarScanBase::sample_height(
                                elevaz, (k + 0.5) * scan.cell_size) / 100.);
        }
    }
}

}
}

#endif

