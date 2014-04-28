#include "volume/sp20.h"
#include "logging.h"
#include "utils.h"
#include "volume_cleaner.h"
#include "site.h"
#include <memory>
#include <cerrno>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif
// libreria radar
#include <func_SP20read.h>
#ifdef __cplusplus
}
#endif

#ifdef NEL
#undef NEL
#endif

using namespace std;

/// This is not used anymore, but it is here to satisfy libSP20 linking needs
int elev_array[15];

namespace cumbac {
namespace volume {

void SP20Loader::make_scan(unsigned idx, unsigned beam_size)
{
    Loader::make_scan(idx, beam_size);
    if (vol_db) vol_db->make_scan(idx, beam_size, elev_array[idx]);
}

void SP20Loader::load(const std::string& pathname)
{
    LOG_CATEGORY("Volume");
    // dimensioni cella a seconda del tipo di acquisizione
    static const float size_cell_by_resolution[]={62.5,125.,250.,500.,1000.,2000.};
    //LOG_CATEGORY("radar.io");
    HD_DBP_SP20_RAW hd_char;

    if (load_info) load_info->filename = pathname;

    // Replicato qui la read_dbp_SP20, per poi metterci mano e condividere codice con la lettura di ODIM

    FILE* sp20_in = fopen_checked(pathname.c_str(), "rb", "input sp20 file");

    // Read and decode file header
    if (fread(&hd_char, sizeof(hd_char), 1, sp20_in) != 1)
    {
      fclose(sp20_in);
      throw std::runtime_error("errore lettura header SP20");
    }
    HD_DBP_SP20_DECOD hd_file;
    decode_header_DBP_SP20(&hd_char, &hd_file);

    /*--------
      ATTENZIONE PRENDO LA DATA DAL NOME DEL FILE
      -------*/
    struct tm data_nome;
    time_t acq_date = get_date_from_name(0, &data_nome, pathname.c_str());
    if (load_info) load_info->acq_date = acq_date;
    if (load_info) load_info->size_cell = size_cell_by_resolution[(int)hd_file.cell_size];
    if (load_info) load_info->declutter_rsp = (bool)hd_file.filtro_clutter;

    BeamCleaner cleaner;
    cleaner.bin_wind_magic_number = site.get_bin_wind_magic_number(acq_date);

    auto_ptr<Beams> b(new Beams);

    /* Ciclo su tutti i raggi del volume */
    while (true)
    {
      char beam_header[40];
      size_t res = fread(beam_header, sizeof(beam_header), 1, sp20_in);
      if (res != 1)
      {
        if (feof(sp20_in))
          break;
        else
        {
          string errmsg("Error reading ");
          errmsg += pathname;
          errmsg += ": ";
          errmsg += strerror(errno);
          fclose(sp20_in);
          throw runtime_error(errmsg);
        }
      }

      BEAM_HD_SP20_INFO beam_info;
      decode_header_sp20_date_from_name(beam_header, &beam_info, (char*)pathname.c_str());

      // Salta beam non validi
      if (!beam_info.valid_data) continue;

      // Calcola la nuova dimensione dei raggi
      unsigned max_range = beam_info.cell_num;;
      if (clean && max_bin)
          max_range = min(max_range, max_bin);

      // TODO: controllare il valore di ritorno delle fread
      if (beam_info.flag_quantities[0]) fread(b->data_z, 1, beam_info.cell_num, sp20_in);
      if (beam_info.flag_quantities[1]) fread(b->data_d, 1, beam_info.cell_num, sp20_in);
      if (beam_info.flag_quantities[2]) fread(b->data_v, 1, beam_info.cell_num, sp20_in);
      if (beam_info.flag_quantities[3]) fread(b->data_w, 1, beam_info.cell_num, sp20_in);

      vector<bool> cleaned(max_range, false);
      if (clean)
          cleaner.clean_beams(*b, max_range, cleaned);

      int el_num = elevation_index(beam_info.elevation);
      if (el_num < 0) continue;
      make_scan(el_num, max_range);
      PolarScan<double>& scan = vol_db->scan(el_num);

      // Convert to DB
      double* dbs = new double[max_range];
      for (unsigned i = 0; i < max_range; ++i)
          dbs[i] = BYTEtoDB(b->data_z[i]);
      //scan.elevation = beam_info.elevation;
#ifdef IMPRECISE_AZIMUT
      fill_beam(scan, el_num, beam_info.elevation, (int)(beam_info.azimuth / FATT_MOLT_AZ)*FATT_MOLT_AZ, max_range, dbs);
#else
      fill_beam(scan, el_num, beam_info.elevation, beam_info.azimuth, max_range, dbs);
#endif
      delete[] dbs;
    }

    fclose(sp20_in);

    LOG_DEBUG ("Nel volume ci sono %d scan", vol_db->size());
    for (size_t i = 0; i < vol_db->size(); ++i)
        LOG_DEBUG (" Scan %2zd - dimensione beam %5d", i, vol_db->scan(i).beam_size);
    // printf("NEL %d\n", (int)old_data_header.norm.maq.num_el);  // TODO: usare questo invece di NEL
    // for (int i = 0; i < old_data_header.norm.maq.num_el; ++i)
    //     printf("VALUE %d %d\n", i, old_data_header.norm.maq.value[i]); // Questi non so se ci servono
}


}
}