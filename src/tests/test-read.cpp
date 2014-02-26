#include <wibble/tests.h>
#include "cum_bac.h"
#include "site.h"
#include "logging.h"
#include <stdio.h>
#include <vector>

using namespace wibble::tests;
using namespace cumbac;

namespace tut {

struct read_shar {
    Logging logging;
};
TESTGRP(read);

namespace {

void test_0120141530gat(WIBBLE_TEST_LOCPRM, const Volume& v)
{
    // Ensure that nbeam_elev has been filled with the right values
    wassert(actual(v.vol_pol[0].nbeams) == 400);
    wassert(actual(v.vol_pol[1].nbeams) == 400);
    wassert(actual(v.vol_pol[2].nbeams) == 400);
    wassert(actual(v.vol_pol[3].nbeams) == 400);
    wassert(actual(v.vol_pol[4].nbeams) == 400);
    wassert(actual(v.vol_pol[5].nbeams) == 400);
    wassert(actual(v.vol_pol[6].nbeams) == 0);
    wassert(actual(v.vol_pol[7].nbeams) == 0);

    // Ensure that the beam sizes are what we expect
    wassert(actual(v.vol_pol[0][0].size()) == 494);
    wassert(actual(v.vol_pol[1][0].size()) == 494);
    wassert(actual(v.vol_pol[2][0].size()) == 494);
    wassert(actual(v.vol_pol[3][0].size()) == 494);
    wassert(actual(v.vol_pol[4][0].size()) == 494);
    wassert(actual(v.vol_pol[5][0].size()) == 494);

    // Ensure that the beam azimuth are what we expect
    /*
    wassert(actual(v.vol_pol[0][0].alfa) == 0);
    wassert(actual(v.vol_pol[0][1].alfa) == 10);
    wassert(actual(v.vol_pol[1][1].alfa) == 10);
    wassert(actual(v.vol_pol[2][1].alfa) == 10);
    wassert(actual(v.vol_pol[3][1].alfa) == 10);
    wassert(actual(v.vol_pol[4][1].alfa) == 10);
    wassert(actual(v.vol_pol[5][1].alfa) == 10);
    */

    // Check other header fields
    wassert(actual(v.acq_date) == 1389108600);
    wassert(actual(v.size_cell) == 250);

    // Arbitrary stats on volume contents so we can check that we read data
    // that looks correct
    VolumeStats stats;
    v.compute_stats(stats);
    wassert(actual(stats.count_zeros[0]) == 0);
    wassert(actual(stats.count_zeros[1]) == 0);
    wassert(actual(stats.count_zeros[2]) == 0);
    wassert(actual(stats.count_zeros[3]) == 0);
    wassert(actual(stats.count_zeros[4]) == 0);
    wassert(actual(stats.count_zeros[5]) == 0);
    wassert(actual(stats.count_zeros[6]) == 0);
    wassert(actual(stats.count_ones[0]) == 146703);
    wassert(actual(stats.count_ones[1]) == 184606);
    wassert(actual(stats.count_ones[2]) == 193796);
    wassert(actual(stats.count_ones[3]) == 196291);
    wassert(actual(stats.count_ones[4]) == 196160);
    wassert(actual(stats.count_ones[5]) == 196157);
    wassert(actual(stats.count_ones[6]) == 0);
    wassert(actual(stats.count_others[0]) == 50897);
    wassert(actual(stats.count_others[1]) == 12994);
    wassert(actual(stats.count_others[2]) ==  3804);
    wassert(actual(stats.count_others[3]) ==  1309);
    wassert(actual(stats.count_others[4]) ==  1440);
    wassert(actual(stats.count_others[5]) ==  1443);
    wassert(actual(stats.count_others[6]) ==     0);
    wassert(actual(stats.sum_others[0]) == 4628598);
    wassert(actual(stats.sum_others[1]) ==  890869);
    wassert(actual(stats.sum_others[2]) ==  218370);
    wassert(actual(stats.sum_others[3]) ==   46002);
    wassert(actual(stats.sum_others[4]) ==   78321);
    wassert(actual(stats.sum_others[5]) ==   88237);
    wassert(actual(stats.sum_others[6]) ==       0);
}

namespace {
struct Difference
{
    unsigned idx;
    unsigned char val;
    Difference() : idx(0), val(0) {}
    Difference(unsigned idx, unsigned char val)
        : idx(idx), val(val) {}
};
}

void test_volumes_equal(WIBBLE_TEST_LOCPRM, const Volume& vsp20, const Volume& vodim)
{
    using namespace std;

    unsigned failed_beams = 0;
    for (unsigned ie = 0; ie < NEL; ++ie)
    {
        WIBBLE_TEST_INFO(testinfo);
        testinfo() << "elevation " << ie;

        wassert(actual(vsp20.vol_pol[ie].nbeams) == vodim.vol_pol[ie].nbeams);
        if (vsp20.vol_pol[ie].nbeams == 0) continue;

        for (unsigned ia = 0; ia < vsp20.vol_pol[ie].nbeams; ++ia)
        {
            testinfo() << "elevation " << ie << " angle " << ia;

            //wassert(actual(vsp20.vol_pol[ie][ia].teta_true) == vodim.vol_pol[ie][ia].teta_true);
            //wassert(actual(vsp20.vol_pol[ie][ia].teta) == vodim.vol_pol[ie][ia].teta);
            //wassert(actual(vsp20.vol_pol[ie][ia].alfa) == vodim.vol_pol[ie][ia].alfa);
            wassert(actual(vsp20.vol_pol[ie][ia].size()) == vodim.vol_pol[ie][ia].size());

            vector<Difference> vals_sp20;
            vector<Difference> vals_odim;
            for (unsigned ib = 0; ib < vsp20.vol_pol[ie][ia].size(); ++ib)
            {
                if (vsp20.vol_pol[ie][ia][ib] != vodim.vol_pol[ie][ia][ib])
                {
                    vals_sp20.push_back(Difference(ib, vsp20.vol_pol[ie][ia][ib]));
                    vals_odim.push_back(Difference(ib, vodim.vol_pol[ie][ia][ib]));
                }
            }
            if (!vals_sp20.empty())
            {
                printf("sp20 vp[%u][%u]:", ie, ia);
                for (vector<Difference>::const_iterator i = vals_sp20.begin(); i != vals_sp20.end(); ++i)
                    printf(" %u:%d", i->idx, (int)i->val);
                printf("\n");
                printf("odim vp[%u][%u]:", ie, ia);
                for (vector<Difference>::const_iterator i = vals_odim.begin(); i != vals_odim.end(); ++i)
                    printf(" %u:%d", i->idx, (int)i->val);
                printf("\n");
                if (vsp20.vol_pol[ie][ia].load_log != vodim.vol_pol[ie][ia].load_log)
                {
                    printf("sp20 vp[%u][%u] load log: ", ie, ia); vsp20.vol_pol[ie][ia].print_load_log(stdout);
                    printf("odim vp[%u][%u] load log: ", ie, ia); vodim.vol_pol[ie][ia].print_load_log(stdout);
                }
                ++failed_beams;
            }
        }
    }

    wassert(actual(failed_beams) == 0);
}

}

template<> template<>
void to::test<1>()
{
    // Test loading of a radar volume via SP20
    static const char* fname = "testdata/DBP2_070120141530_GATTATICO";
    Volume vsp20;
    const Site& gat = Site::get("GAT");
    gat.fill_elev_array(elev_array);
    vsp20.read_sp20("testdata/DBP2_070120141530_GATTATICO", gat, false);
    // Check the contents of what we read
    wruntest(test_0120141530gat, vsp20);
}

template<> template<>
void to::test<2>()
{
    // Test loading of a radar volume via SP20
    static const char* fname = "testdata/MSG1400715300U.101.h5";
    CUM_BAC* cb = new CUM_BAC("GAT");
    bool res = cb->read_odim_volume(fname, 0);
    // Ensure that reading was successful
    wassert(actual(res).istrue());
    // Check the contents of what we read
    wruntest(test_0120141530gat, cb->volume);
    delete cb;
}

template<> template<>
void to::test<3>()
{
    using namespace std;
    Volume vsp20;
    Volume vodim;

    // FIXME: get rid of the static elev_array as soon as it is convenient to do so
    const Site& gat = Site::get("GAT");
    gat.fill_elev_array(elev_array);

    vsp20.read_sp20("testdata/DBP2_070120141530_GATTATICO", gat, false);
    vodim.read_odim("testdata/MSG1400715300U.101.h5");

    wruntest(test_volumes_equal, vsp20, vodim);
}

template<> template<>
void to::test<4>()
{
    // Test loading of a radar volume via SP20
    static const char* fname = "testdata/DBP2_060220140140_GATTATICO";
    CUM_BAC* cb = new CUM_BAC("GAT");
    bool res = cb->read_sp20_volume(fname, 0);
    // Ensure that reading was successful
    wassert(actual(res).istrue());
    // TODO: Check the contents of what we read
    //wruntest(test_0120141530gat, cb->volume);
    delete cb;
}

template<> template<>
void to::test<5>()
{
    using namespace std;
    Volume vsp20;
    Volume v_mod;

    const Site& gat = Site::get("GAT");
    gat.fill_elev_array(elev_array);

    vsp20.read_sp20("testdata/DBP2_060220140140_GATTATICO", gat);
    v_mod.read_sp20("testdata/DBP2_060220140140_GATTATICO_mod", gat, false);

    wruntest(test_volumes_equal, vsp20, v_mod);
}


}
