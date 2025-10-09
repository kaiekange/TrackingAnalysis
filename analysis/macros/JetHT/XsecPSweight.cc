#include <iostream>
#include <TString.h>
#include <ROOT/RDataFrame.hxx>

const double sigma[] = {
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
}

const double Xsecweights_preEE[] = {
    1.346118785E+04,
    2.576428627E+03,
    3.232395301E+02,
    1.411945281E+02,
    8.819639591E+00,
    6.413039027E-01,
    2.278219305E-02,
    6.947432007E-03,
    3.644285091E-03,
    5.443647533E-04,
    1.655321180E-04,
    3.867216343E-05,
    2.331000000E-06
};

const double Xsecweights_postEE[] = {
    8.642421957E+02,
    8.569413807E+01,
    3.489041256E+01,
    9.266847304E+00,
    3.188779664E-01,
    2.968123238E-02,
    6.298806048E-03,
    1.981796258E-03,
    1.096614437E-03,
    1.889057779E-04,
    4.679588799E-05,
    9.548225375E-06,
    4.919579335E-07
};

const double PSweights_preEE[] = {
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

const double PSweights_postEE[] = {
    8.371926083E+03,
    3.701355044E+02,
    1.127954482E+02,
    2.876832298E+01,
    7.449626204E+00,
    1.961165419E+00,
    2.307090802E-01,
    3.790584761E-02,
    3.040074513E-03,
    3.327949802E-04
};

double selectPSweight(const bool trig_PFHT1050_pass,
        const bool trig_PFHT890_pass,
        const bool trig_PFHT780_pass,
        const bool trig_PFHT680_pass,
        const bool trig_PFHT590_pass,
        const bool trig_PFHT510_pass,
        const bool trig_PFHT430_pass,
        const bool trig_PFHT370_pass,
        const bool trig_PFHT250_pass,
        const bool trig_PFHT180_pass,
        const double* myweight) {
    if      (trig_PFHT1050_pass) return myweight[0];
    else if (trig_PFHT890_pass)  return myweight[1];
    else if (trig_PFHT780_pass)  return myweight[2];
    else if (trig_PFHT680_pass)  return myweight[3];
    else if (trig_PFHT590_pass)  return myweight[4];
    else if (trig_PFHT510_pass)  return myweight[5];
    else if (trig_PFHT430_pass)  return myweight[6];
    else if (trig_PFHT370_pass)  return myweight[7];
    else if (trig_PFHT250_pass)  return myweight[8];
    else if (trig_PFHT180_pass)  return myweight[9];
    else return 0.0;
}

int XsecPSweight(TString era, int iPT, TString PTrange){

    ROOT::EnableImplicitMT();

    double Xsecweight = 0.0;
    const double* PSweights = nullptr;
    
    if(era.Contains("preEE")){
        Xsecweight = Xsecweights_preEE[iPT];
        PSweights = PSweights_preEE;
    }
    else if(era.Contains("postEE")){
        Xsecweight = Xsecweights_postEE[iPT];
        PSweights = PSweights_postEE;
    }

    ROOT::RDataFrame myDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_"+PTrange+"_"+era+"/S5M*/skimmed_*.root");
  
    std::string mycut;
    if(iPT == 0) mycut = "!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass";
    else if(iPT == 0) mycut = "!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass";
    else if(iPT == 0) mycut = "!trig_PFHT1050_pass";
    else mycut = "1";

    auto myDF_filter = myDF.Filter(mycut);

    /* auto myDF_filter = myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass && !trig_PFHT680_pass && !trig_PFHT590_pass"); */
    /* auto myDF_filter = myDF.Filter("!trig_PFHT1050_pass && !trig_PFHT890_pass && !trig_PFHT780_pass"); */
    /* auto myDF_filter = myDF.Filter("!trig_PFHT1050_pass"); */

    auto XsecPSDF = myDF_filter.Define("xsecweight", [=](){ return Xsecweight; })
        .Define("PSweight",
                [=](bool trig_PFHT1050_pass, bool trig_PFHT890_pass, bool trig_PFHT780_pass,
                    bool trig_PFHT680_pass, bool trig_PFHT590_pass, bool trig_PFHT510_pass,
                    bool trig_PFHT430_pass, bool trig_PFHT370_pass, bool trig_PFHT250_pass,
                    bool trig_PFHT180_pass) {
                return selectPSweight(trig_PFHT1050_pass, trig_PFHT890_pass, trig_PFHT780_pass,
                        trig_PFHT680_pass, trig_PFHT590_pass, trig_PFHT510_pass,
                        trig_PFHT430_pass, trig_PFHT370_pass, trig_PFHT250_pass,
                        trig_PFHT180_pass, PSweights);
                },
                {"trig_PFHT1050_pass", "trig_PFHT890_pass", "trig_PFHT780_pass", "trig_PFHT680_pass",
                "trig_PFHT590_pass", "trig_PFHT510_pass", "trig_PFHT430_pass", "trig_PFHT370_pass",
                "trig_PFHT250_pass", "trig_PFHT180_pass"});

    XsecPSDF.Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_XsecPS/all_skimmed_2022_mc_"+PTrange+"_"+era+".root");

    return 0;
}
