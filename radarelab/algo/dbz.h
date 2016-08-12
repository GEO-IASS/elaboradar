/**
 *  @file
 *  @ingroup radarelab 
*/
#ifndef RADARELAB_ALGO_DBZ_H
#define RADARELAB_ALGO_DBZ_H

#include <cmath>

namespace radarelab {
namespace algo {

/**
 * Class to manage reflectivity functions (simply attenuation correction, conversion between Z, dBZ, R)
 */
class DBZ
{
public:
    double base_cell_size;              ///< cella size dimension
    double aMP, bMP;                    ////< Marshall-Palmer coefficient for Z-R relationship

    DBZ();

    /**
     * @brief Seasonal setup function
     * @param [in] month - month 
     * @param [in] base_cell_size - cell size dimension [m]
     */
    void setup(int month, double base_cell_size);

    /**
     *
     *  @brief   funzione che calcola l'attenuazione totale 
     *  @details Ricevuto in ingresso il dato di Z in byte e l'attenuazione complessiva sul raggio fino al punto in considerazione, calcola l'attenuazione totale 
     *  ingresso:dbz in quel punto e attenuazione fin lì
     *  Doviak,Zrnic,1984 for rain as reported in cost 717 final document
     *  @param[in]  DBZbyte valore in byte della Z nel pixel
     *  @param[in]  PIA attenuazione totale fino al punto
     *  @return att_tot attenuazione incrementata del contributo nel pixel corrente
     */
    double attenuation(unsigned char DBZbyte, double  PIA);
    double attenuation(double DBZvalue, double  PIA);   /* Doviak,Zrnic,1984 for rain as reported in cost 717 final document*/

    // TODO: find better names for these:
    // RtoDBZ calcolato su aMP e bMP
    double RtoDBZ(double rain) const;
    double DBZtoR(double dbz) const;
    double DBZ_snow(double dbz) const;
    double DBZ_conv(double dbz) const;
    double RtoDBZ_class(double R) const;
    double DBZ_to_mp_func(double sample) const;

    /**
     * @function
     * Compute the corrected value (in dB) given the original value (in dB) and the
     * beam blocking percentage (from 0 to 100) for that value
     * @param [in] val_db - uncorrected dBZ value
     * @param [in] beamblocking - percentage of beam blocking
     * @return corrected dBZ value
     */
    static constexpr inline double beam_blocking_correction(double val_db, double beamblocking)
    {
       return val_db - 10 * log10(1. - beamblocking / 100.);
    }

    /**
     * @brief funzione che converte Z unsigned char in  DBZ
     * @param[in] DBZbyte riflettivita' in byte
     * @param [in] gain  - first conversion factor 
     * @param [in] offset - second conversion factor 
     * @return equivalente in DBZ
     */
    static constexpr inline double BYTEtoDB(unsigned char DBZbyte, double gain=80./255., double offset=-20.)
    {
        return (DBZbyte * gain + offset);
    }

    /**
     * @brief funzione che converte  dB in valore intero tra 0 e 255
     * @param[in] DB dBZ in ingresso
     * @param [in] gain  - first conversion factor 
     * @param [in] offset - second conversion factor 
     * @return converted value in byte
     */
    static inline unsigned char DBtoBYTE(double DB, double gain=80./255., double offset=-20.)
    {
        int byt = round((DB - offset) / gain);
        if (byt <= 0)
            return 0;
        else if (byt <= 255)
            return (unsigned char)(byt);
        else 
            return 255;
    }

    /**
     * @brief funzione che converte byte in Z
     * @param[in] byte valore da convertire in z espresso tra 0 e 255
     * @param [in] gain  - first conversion factor 
     * @param [in] offset - second conversion factor 
     * @return Z value (linear not dBZ)
     */
    static double BYTEtoZ(unsigned char byte);
};

}
}

#endif

