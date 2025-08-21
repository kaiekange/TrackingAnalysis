#include <iostream>
#include <vector>
#include <TString.h>
#include "../../functions/fill_file_vector.cc"

const string filepath[] = {
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/JetHT/Run2022B_27Jun2023_v2_MINIAOD/250812_133628/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/JetMET/Run2022C_27Jun2023_v1_MINIAOD/250812_133654/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/JetMET/Run2022D_27Jun2023_v2_MINIAOD/250812_133706/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/JetMET/Run2022E_27Jun2023_v1_MINIAOD/250812_133719/",

    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-50to80_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_125849/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-80to120_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132251/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-120to170_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132300/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-170to300_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132309/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-300to470_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132318/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-470to600_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132327/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-600to800_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132337/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-800to1000_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132345/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1000to1400_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132354/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1400to1800_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132403/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1800to2400_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250813_132412/",

    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-50to80_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_125836/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-80to120_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132105/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-120to170_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132114/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-170to300_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132122/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-300to470_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132131/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-470to600_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132140/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-600to800_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132148/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-800to1000_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132157/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1000to1400_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132207/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1400to1800_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132216/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250717/QCD_PT-1800to2400_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250813_132224/",
};

const TString eras[] = { 
    "2022_data_B", "2022_data_C", "2022_data_D", "2022_data_E",
    "2022_mc_50to80_preEE", "2022_mc_80to120_preEE", "2022_mc_120to170_preEE", "2022_mc_170to300_preEE", "2022_mc_300to470_preEE", "2022_mc_470to600_preEE", "2022_mc_600to800_preEE", "2022_mc_800to1000_preEE", "2022_mc_1000to1400_preEE", "2022_mc_1400to1800_preEE", "2022_mc_1800to2400_preEE",
    "2022_mc_50to80_postEE", "2022_mc_80to120_postEE", "2022_mc_120to170_postEE", "2022_mc_170to300_postEE", "2022_mc_300to470_postEE", "2022_mc_470to600_postEE", "2022_mc_600to800_postEE", "2022_mc_800to1000_postEE", "2022_mc_1000to1400_postEE", "2022_mc_1400to1800_postEE", "2022_mc_1800to2400_postEE"
};

const TString outputdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/";

int mkfile( int idx ) {

    if(gSystem->AccessPathName(outputdir)) gSystem->MakeDirectory(outputdir);

    /* gErrorIgnoreLevel = kError; */
    /* gInterpreter->GenerateDictionary("vector<vector<float>>", "vector"); */

    std::vector<TString> file_vector = fill_file_vector(filepath[idx] + "*/*.root");

    TFile *outfile = new TFile(outputdir + eras[idx] + ".root", "RECREATE");
    TTree *outtree = new TTree("mytree", "");

    int ev_nPV;
    int pv_NTracks;
    float pv_SumTrackPt;
    float pv_SumTrackPt2;
    float pv_chi2;
    float pv_ndof;
    float pv_x;
    float pv_y; 
    float pv_z;
    float pv_xError;
    float pv_yError;
    float pv_zError;
    float pv_x_p1;
    float pv_y_p1;
    float pv_z_p1;
    float pv_xError_p1;
    float pv_yError_p1;
    float pv_zError_p1;
    float pv_x_p2;
    float pv_y_p2;
    float pv_z_p2;
    float pv_xError_p2;
    float pv_yError_p2;
    float pv_zError_p2;

    vector<float> * pv_trk_d0_pvunbiased = new vector<float>();
    vector<float> * pv_trk_dz_pvunbiased = new vector<float>();
    vector<float> * pv_trk_pt = new vector<float>();
    vector<float> * pv_trk_eta = new vector<float>();
    vector<float> * pv_trk_phi = new vector<float>();

    outtree->Branch("ev_nPV", &ev_nPV);
    outtree->Branch("pv_NTracks", &pv_NTracks);
    outtree->Branch("pv_SumTrackPt", &pv_SumTrackPt);
    outtree->Branch("pv_SumTrackPt2", &pv_SumTrackPt2);
    outtree->Branch("pv_chi2", &pv_chi2);
    outtree->Branch("pv_ndof", &pv_ndof);
    outtree->Branch("pv_x", &pv_x);
    outtree->Branch("pv_y", &pv_y); 
    outtree->Branch("pv_z", &pv_z);
    outtree->Branch("pv_xError", &pv_xError);
    outtree->Branch("pv_yError", &pv_yError);
    outtree->Branch("pv_zError", &pv_zError);
    outtree->Branch("pv_x_p1", &pv_x_p1);
    outtree->Branch("pv_y_p1", &pv_y_p1);
    outtree->Branch("pv_z_p1", &pv_z_p1);
    outtree->Branch("pv_xError_p1", &pv_xError_p1);
    outtree->Branch("pv_yError_p1", &pv_yError_p1);
    outtree->Branch("pv_zError_p1", &pv_zError_p1);
    outtree->Branch("pv_x_p2", &pv_x_p2);
    outtree->Branch("pv_y_p2", &pv_y_p2);
    outtree->Branch("pv_z_p2", &pv_z_p2);
    outtree->Branch("pv_xError_p2", &pv_xError_p2);
    outtree->Branch("pv_yError_p2", &pv_yError_p2);
    outtree->Branch("pv_zError_p2", &pv_zError_p2);

    outtree->Branch("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    outtree->Branch("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);
    outtree->Branch("pv_trk_pt", &pv_trk_pt);
    outtree->Branch("pv_trk_eta", &pv_trk_eta);
    outtree->Branch("pv_trk_phi", &pv_trk_phi);

    int nfile = file_vector.size();
    for(int i=0; i<nfile; i++){ 

        TString filename = file_vector[i];
        TFile *infile = TFile::Open( filename );

        if( !infile || infile->IsZombie() ) {
            std::cerr << "Error opening file: " << filename << std::endl;
            continue;
        }
        TTree * intree = (TTree*)infile->Get("residuals/tree");
        if ( !intree ) {
            std::cerr << "Error: No tree found in file: " << filename << std::endl;
            infile->Close();
            continue;
        }

        bool trig_PFJet40_pass;
        bool trig_PFJet60_pass;
        bool trig_PFJet80_pass;
        bool trig_PFJet140_pass;
        bool trig_PFJet200_pass;
        bool trig_PFJet260_pass;
        bool trig_PFJet320_pass;
        bool trig_PFJet400_pass;
        bool trig_PFJet450_pass;
        bool trig_PFJet500_pass;
        bool trig_PFJet550_pass;

        bool trig_PFHT180_pass;
        bool trig_PFHT250_pass;
        bool trig_PFHT370_pass;
        bool trig_PFHT430_pass;
        bool trig_PFHT510_pass;
        bool trig_PFHT590_pass;
        bool trig_PFHT680_pass;
        bool trig_PFHT780_pass;
        bool trig_PFHT890_pass;
        bool trig_PFHT1050_pass;
        bool trig_PFHT350_pass;
        int myev_nPV;
        vector<int> *mypv_NTracks = nullptr;
        vector<float> *mypv_SumTrackPt = nullptr;
        vector<float> *mypv_SumTrackPt2 = nullptr;
        vector<float> *mypv_chi2 = nullptr;
        vector<float> *mypv_ndof = nullptr;
        vector<float> *mypv_x = nullptr;
        vector<float> *mypv_y = nullptr;
        vector<float> *mypv_z = nullptr;
        vector<float> *mypv_xError = nullptr;
        vector<float> *mypv_yError = nullptr;
        vector<float> *mypv_zError = nullptr;
        vector<float> *mypv_x_p1 = nullptr;
        vector<float> *mypv_y_p1 = nullptr;
        vector<float> *mypv_z_p1 = nullptr;
        vector<float> *mypv_xError_p1 = nullptr;
        vector<float> *mypv_yError_p1 = nullptr;
        vector<float> *mypv_zError_p1 = nullptr;
        vector<float> *mypv_x_p2 = nullptr;
        vector<float> *mypv_y_p2 = nullptr;
        vector<float> *mypv_z_p2 = nullptr;
        vector<float> *mypv_xError_p2 = nullptr;
        vector<float> *mypv_yError_p2 = nullptr;
        vector<float> *mypv_zError_p2 = nullptr;
        vector<vector<float>> *mypv_trk_d0_pvunbiased = nullptr;
        vector<vector<float>> *mypv_trk_dz_pvunbiased = nullptr;
        vector<vector<float>> *mypv_trk_pt = nullptr;
        vector<vector<float>> *mypv_trk_eta = nullptr;
        vector<vector<float>> *mypv_trk_phi = nullptr;

        intree->SetBranchAddress("trig_PFJet40_pass", &trig_PFJet40_pass);
        intree->SetBranchAddress("trig_PFJet60_pass", &trig_PFJet60_pass);
        intree->SetBranchAddress("trig_PFJet80_pass", &trig_PFJet80_pass);
        intree->SetBranchAddress("trig_PFJet140_pass", &trig_PFJet140_pass);
        intree->SetBranchAddress("trig_PFJet200_pass", &trig_PFJet200_pass);
        intree->SetBranchAddress("trig_PFJet260_pass", &trig_PFJet260_pass);
        intree->SetBranchAddress("trig_PFJet320_pass", &trig_PFJet320_pass);
        intree->SetBranchAddress("trig_PFJet400_pass", &trig_PFJet400_pass);
        intree->SetBranchAddress("trig_PFJet450_pass", &trig_PFJet450_pass);
        intree->SetBranchAddress("trig_PFJet500_pass", &trig_PFJet500_pass);
        intree->SetBranchAddress("trig_PFJet550_pass", &trig_PFJet550_pass);
        intree->SetBranchAddress("trig_PFHT180_pass", &trig_PFHT180_pass);
        intree->SetBranchAddress("trig_PFHT250_pass", &trig_PFHT250_pass);
        intree->SetBranchAddress("trig_PFHT370_pass", &trig_PFHT370_pass);
        intree->SetBranchAddress("trig_PFHT430_pass", &trig_PFHT430_pass);
        intree->SetBranchAddress("trig_PFHT510_pass", &trig_PFHT510_pass);
        intree->SetBranchAddress("trig_PFHT590_pass", &trig_PFHT590_pass);
        intree->SetBranchAddress("trig_PFHT680_pass", &trig_PFHT680_pass);
        intree->SetBranchAddress("trig_PFHT780_pass", &trig_PFHT780_pass);
        intree->SetBranchAddress("trig_PFHT890_pass", &trig_PFHT890_pass);
        intree->SetBranchAddress("trig_PFHT1050_pass", &trig_PFHT1050_pass);
        intree->SetBranchAddress("trig_PFHT350_pass", &trig_PFHT350_pass);
        intree->SetBranchAddress("ev_nPV", &myev_nPV);
        intree->SetBranchAddress("pv_NTracks", &mypv_NTracks);
        intree->SetBranchAddress("pv_SumTrackPt", &mypv_SumTrackPt);
        intree->SetBranchAddress("pv_SumTrackPt2", &mypv_SumTrackPt2);
        intree->SetBranchAddress("pv_chi2", &mypv_chi2);
        intree->SetBranchAddress("pv_ndof", &mypv_ndof);
        intree->SetBranchAddress("pv_x", &mypv_x);
        intree->SetBranchAddress("pv_y", &mypv_y);
        intree->SetBranchAddress("pv_z", &mypv_z);
        intree->SetBranchAddress("pv_xError", &mypv_xError);
        intree->SetBranchAddress("pv_yError", &mypv_yError);
        intree->SetBranchAddress("pv_zError", &mypv_zError);
        intree->SetBranchAddress("pv_x_p1", &mypv_x_p1);
        intree->SetBranchAddress("pv_y_p1", &mypv_y_p1);
        intree->SetBranchAddress("pv_z_p1", &mypv_z_p1);
        intree->SetBranchAddress("pv_xError_p1", &mypv_xError_p1);
        intree->SetBranchAddress("pv_yError_p1", &mypv_yError_p1);
        intree->SetBranchAddress("pv_zError_p1", &mypv_zError_p1);
        intree->SetBranchAddress("pv_x_p2", &mypv_x_p2);
        intree->SetBranchAddress("pv_y_p2", &mypv_y_p2);
        intree->SetBranchAddress("pv_z_p2", &mypv_z_p2);
        intree->SetBranchAddress("pv_xError_p2", &mypv_xError_p2);
        intree->SetBranchAddress("pv_yError_p2", &mypv_yError_p2);
        intree->SetBranchAddress("pv_zError_p2", &mypv_zError_p2);
        intree->SetBranchAddress("pv_trk_d0_pvunbiased", &mypv_trk_d0_pvunbiased);
        intree->SetBranchAddress("pv_trk_dz_pvunbiased", &mypv_trk_dz_pvunbiased);
        intree->SetBranchAddress("pv_trk_pt", &mypv_trk_pt);
        intree->SetBranchAddress("pv_trk_eta", &mypv_trk_eta);
        intree->SetBranchAddress("pv_trk_phi", &mypv_trk_phi);

        int nentries = intree->GetEntries();
        /* for(int j=0; j<5; j++){ */
        for(int j=0; j<nentries; j++){

            intree->GetEntry(j);

            pv_trk_d0_pvunbiased->clear();
            pv_trk_dz_pvunbiased->clear();
            pv_trk_pt->clear();
            pv_trk_eta->clear();
            pv_trk_phi->clear();

            if(! (trig_PFJet40_pass || trig_PFJet60_pass || trig_PFJet80_pass || trig_PFJet140_pass || trig_PFJet200_pass || trig_PFJet260_pass 
                        || trig_PFJet320_pass || trig_PFJet400_pass || trig_PFJet450_pass || trig_PFJet500_pass || trig_PFJet550_pass 
                        || trig_PFHT180_pass || trig_PFHT250_pass || trig_PFHT370_pass || trig_PFHT430_pass || trig_PFHT510_pass || trig_PFHT590_pass
                        || trig_PFHT680_pass || trig_PFHT780_pass || trig_PFHT890_pass || trig_PFHT1050_pass || trig_PFHT350_pass) ) continue;

            ev_nPV = myev_nPV;
            pv_NTracks = mypv_NTracks->at(0);
            pv_SumTrackPt = mypv_SumTrackPt->at(0);
            pv_SumTrackPt2 = mypv_SumTrackPt2->at(0);
            pv_chi2 = mypv_chi2->at(0);
            pv_ndof = mypv_ndof->at(0);
            pv_x = mypv_x->at(0);
            pv_y = mypv_y->at(0);
            pv_z = mypv_z->at(0);
            pv_xError = mypv_xError->at(0);
            pv_yError = mypv_yError->at(0);
            pv_zError = mypv_zError->at(0);
            pv_x_p1 = mypv_x_p1->at(0);
            pv_y_p1 = mypv_y_p1->at(0);
            pv_z_p1 = mypv_z_p1->at(0);
            pv_xError_p1 = mypv_xError_p1->at(0);
            pv_yError_p1 = mypv_yError_p1->at(0);
            pv_zError_p1 = mypv_zError_p1->at(0);
            pv_x_p2 = mypv_x_p2->at(0);
            pv_y_p2 = mypv_y_p2->at(0);
            pv_z_p2 = mypv_z_p2->at(0);
            pv_xError_p2 = mypv_xError_p2->at(0);
            pv_yError_p2 = mypv_yError_p2->at(0);
            pv_zError_p2 = mypv_zError_p2->at(0);

            int pv_track_size = (*mypv_trk_d0_pvunbiased)[0].size();
            for(int k=0; k<pv_track_size; k++){
                pv_trk_d0_pvunbiased->push_back( (*mypv_trk_d0_pvunbiased)[0][k] );
                pv_trk_dz_pvunbiased->push_back( (*mypv_trk_dz_pvunbiased)[0][k] );
                pv_trk_pt->push_back( (*mypv_trk_pt)[0][k] );
                pv_trk_eta->push_back( (*mypv_trk_eta)[0][k] );
                pv_trk_phi->push_back( (*mypv_trk_phi)[0][k] );
            }

            outtree->Fill();
        }

        if( (i+1) % int(nfile/5.0) == 0 ) std::cout << "Processed " << (100 * (i+1)) / nfile << "% (" << i+1 << "/" << nfile << ")" << std::endl;
        if( (i+1) == nfile) std::cout << "Processed all files, now writing to output files" << std::endl;

    }
    outfile->cd(); 
    outtree->Write();
    outfile->Close();

    return 0;
    }
