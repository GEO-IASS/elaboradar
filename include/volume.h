#ifndef ARCHIVIATORE_VOLUME_CLASS_H
#define ARCHIVIATORE_VOLUME_CLASS_H

#include <string>
#include <vector>
#include <ctime>
#include <cstdio>

// TODO: prima o poi arriviamo a far senza di questi define
#define NEL 15                // n0 elevazioni massimo
#define NUM_AZ_X_PPI 400

// TODO: per compatibilità con la libsp20, anche questo toglierlo in futuro
extern int elev_array[NEL];

namespace H5 {
struct H5File;
}

namespace cumbac {
struct Site;

struct LoadLogEntry
{
    double theta;
    double alpha;

    LoadLogEntry(double theta, double alpha)
        : theta(theta), alpha(alpha)
    {
    }

    bool operator==(const LoadLogEntry& e) const
    {
        return theta == e.theta && alpha == e.alpha;
    }
};

struct Ray
{
    std::vector<unsigned char> ray;
    short alfa_true, teta_true;
    short teta, alfa;

    std::vector<LoadLogEntry> load_log;

    Ray();

    /**
     * Riempie un array di float con i dati del raggio convertiti in DB
     * Se l'array è più lungo del raggio, setta gli elementi extra a missing.
     */
    void read_db(float* out, size_t out_size, float missing=0) const;

    void log(double theta, double alpha)
    {
        load_log.push_back(LoadLogEntry(theta, alpha));
    }
    void print_load_log(FILE* out) const;
};

struct PolarScan : public std::vector<Ray>
{
    unsigned nbeams;
    double elevation;

    PolarScan();

    void fill_beam(int el_num, double theta, double alpha, unsigned size, const unsigned char* data);

protected:
    void merge_beam(int el_num, int az_num, double theta, double alpha, unsigned size, const unsigned char* dati);
};

struct VolumeStats
{
    unsigned count_zeros[NEL];
    unsigned count_ones[NEL];
    unsigned count_others[NEL];
    unsigned sum_others[NEL];

    void print(FILE* out);
};

class Volume
{
public:
    std::string filename;
    time_t acq_date;
    double size_cell;
    bool declutter_rsp; // ?

    //dato di base volume polare, struttura definita in libSP20
    PolarScan vol_pol[NEL];
    //Ray vol_pol[NEL][NUM_AZ_X_PPI];

    // elevazione finale in coordinate azimut range
    std::vector<unsigned char> elev_fin[NUM_AZ_X_PPI];

    Volume();

    /// Compute the vol_pol index of an elevation angle
    unsigned elevation_index(double elevation) const;

    inline Ray& ray_at_elev_preci(unsigned az_idx, unsigned ray_idx)
    {
        return vol_pol[elev_fin[az_idx][ray_idx]][az_idx];
    }
    inline const Ray& ray_at_elev_preci(unsigned az_idx, unsigned ray_idx) const
    {
        return vol_pol[elev_fin[az_idx][ray_idx]][az_idx];
    }

    inline unsigned char sample_at_elev_preci(unsigned az_idx, unsigned ray_idx) const
    {
        return ray_at_elev_preci(az_idx, ray_idx).ray[ray_idx];
    }

    void read_sp20(const char* nome_file, const Site& site, bool clean=true);
    void read_odim(const char* nome_file);

    void compute_stats(VolumeStats& stats) const;

    void write_info_to_debug_file(H5::H5File out);

protected:
    void resize_elev_fin();
};


}

#endif
