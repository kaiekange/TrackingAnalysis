#include <iostream>
#include <TString.h>
#include <ROOT/RDataFrame.hxx>

const double sigma[13] = {
    16760000,
    2534000,
    445800,
    113700,
    7589,
    626.4,
    178.6,
    30.57,
    8.92,
    0.8103,
    0.1148,
    0.007542,
    0.0002331
};

const double nevents_preEE[13] = {
    1245061,
    983532,
    1379163,
    805272,
    860466,
    976760,
    7839456,
    4400187,
    2447668,
    1488524,
    693521,
    195024,
    100000
};

const double nevents_postEE[13] = {
    646504,
    985744,
    425947,
    409001,
    793375,
    703560,
    945190,
    514421,
    271222,
    142993,
    81821,
    26333,
    15805
};

const double PSweights_preEE[10] = {
    1.757564517E+04,
    8.061593708E+02,
    2.447382910E+02,
    6.305400202E+01,
    1.642813646E+01,
    4.319870164E+00,
    5.003805667E-01,
    8.044478684E-02,
    5.659858108E-03,
    4.976484745E-04
};

const double PSweights_postEE[10] = {
    8.367887020E+03,
    3.700056908E+02,
    1.127610026E+02,
    2.875279127E+01,
    7.448824704E+00,
    1.960143771E+00,
    2.306340269E-01,
    3.788380970E-02,
    3.038814991E-03,
    3.326681393E-04
};

double selectPSweight(const bool trig_pass[10], const double* myweight) {
    for (int i = 0; i < 10; ++i) {
        if (trig_pass[i]) return myweight[i];
    }
    return 0.0;
}

bool selectcellmask(const bool trig_pass[10], const bool* cell_mask) {
    for (int i = 0; i < 10; ++i) {
        if (trig_pass[i]) return cell_mask[i];
    }
    return false;
}

int XsecPSweight(TString era, int iPT, TString PTrange){

    ROOT::EnableImplicitMT();

    // ROOT::RDataFrame myDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_"+PTrange+"_"+era+"/S5M*/skimmed_*.root");
    ROOT::RDataFrame myDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_"+PTrange+"_"+era+"/skimmed_*.root");

    std::vector<int> nevents;

    double Xsecweight = 0.0;
    double allnevents = 0.0;
    const double* PSweights = nullptr;

    if(era.Contains("preEE")){
        Xsecweight = sigma[iPT] / nevents_preEE[iPT];
        allnevents = nevents_preEE[iPT];
        PSweights = PSweights_preEE;
    }
    else if(era.Contains("postEE")){
        Xsecweight = sigma[iPT] / nevents_postEE[iPT];
        allnevents = nevents_postEE[iPT];
        PSweights = PSweights_postEE;
    }

    nevents.push_back(myDF.Filter("trig_PFHT1050_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && trig_PFHT890_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && trig_PFHT780_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && trig_PFHT680_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && trig_PFHT590_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass && trig_PFHT510_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass && !trig_PFHT510_pass && trig_PFHT430_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass && !trig_PFHT510_pass && !trig_PFHT430_pass && trig_PFHT370_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass && !trig_PFHT510_pass && !trig_PFHT430_pass && !trig_PFHT370_pass && trig_PFHT250_pass").Count().GetValue());
    nevents.push_back(myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass && !trig_PFHT510_pass && !trig_PFHT430_pass && !trig_PFHT370_pass && !trig_PFHT250_pass && trig_PFHT180_pass").Count().GetValue());

    std::array<bool,10> cell_mask;
    for(size_t i=0; i<nevents.size(); i++) cell_mask[i]=(nevents[i] > (allnevents/1000.0));

    auto XsecPSDF = myDF.Define("xsecweight", [=](){ return Xsecweight; })
        .Define("PSweight",
                [=](bool trig_PFHT1050_pass, bool trig_PFHT890_pass, bool trig_PFHT780_pass, bool trig_PFHT680_pass, bool trig_PFHT590_pass, bool trig_PFHT510_pass, bool trig_PFHT430_pass, bool trig_PFHT370_pass, bool trig_PFHT250_pass, bool trig_PFHT180_pass)
                {
                    bool trig_arr[10] = { trig_PFHT1050_pass, trig_PFHT890_pass, trig_PFHT780_pass, trig_PFHT680_pass, trig_PFHT590_pass, trig_PFHT510_pass, trig_PFHT430_pass, trig_PFHT370_pass, trig_PFHT250_pass, trig_PFHT180_pass };
                    return selectPSweight(trig_arr, PSweights);
                },
                {"trig_PFHT1050_pass", "trig_PFHT890_pass", "trig_PFHT780_pass", "trig_PFHT680_pass", "trig_PFHT590_pass", "trig_PFHT510_pass", "trig_PFHT430_pass", "trig_PFHT370_pass", "trig_PFHT250_pass", "trig_PFHT180_pass"}
                )
        .Define("cell_mask",
                [cell_mask](bool trig_PFHT1050_pass, bool trig_PFHT890_pass, bool trig_PFHT780_pass, bool trig_PFHT680_pass, bool trig_PFHT590_pass, bool trig_PFHT510_pass, bool trig_PFHT430_pass, bool trig_PFHT370_pass, bool trig_PFHT250_pass, bool trig_PFHT180_pass)
                {
                    bool trig_arr[10] = { trig_PFHT1050_pass, trig_PFHT890_pass, trig_PFHT780_pass, trig_PFHT680_pass, trig_PFHT590_pass, trig_PFHT510_pass, trig_PFHT430_pass, trig_PFHT370_pass, trig_PFHT250_pass, trig_PFHT180_pass };
                    return selectcellmask(trig_arr, cell_mask.data());
                },
                {"trig_PFHT1050_pass", "trig_PFHT890_pass", "trig_PFHT780_pass", "trig_PFHT680_pass", "trig_PFHT590_pass", "trig_PFHT510_pass", "trig_PFHT430_pass", "trig_PFHT370_pass", "trig_PFHT250_pass", "trig_PFHT180_pass"}
                );

    XsecPSDF.Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_XsecPSmask/all_skimmed_2022_mc_"+PTrange+"_"+era+".root");

    return 0;
}
