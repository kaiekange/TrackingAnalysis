#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

int mkfile( TString infilename, TString outfilename ) {

    ROOT::EnableImplicitMT();

    /* gErrorIgnoreLevel = kError; */
    gInterpreter->GenerateDictionary("vector<vector<float>>", "vector");

    TFile *infile = TFile::Open( infilename );
    if( !infile || infile->IsZombie() ) {
        std::cerr << "Error opening file: " << infilename << std::endl;
        return 1;
    }
    TTree * intree = (TTree*)infile->Get("residuals/tree");
    if ( !intree ) {
        std::cerr << "Error: No tree found in file: " << infilename << std::endl;
        infile->Close();
        return 1;
    }

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
    int myev_run;
    int myev_lumi;
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
    intree->SetBranchAddress("ev_run", &myev_run);
    intree->SetBranchAddress("ev_lumi", &myev_lumi);
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

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *outtree = new TTree("mytree", "");

    int ev_run;
    int ev_lumi;
    int ev_nPV;
    float ev_PFHT;
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

    outtree->Branch("trig_PFHT180_pass", &trig_PFHT180_pass); 
    outtree->Branch("trig_PFHT250_pass", &trig_PFHT250_pass); 
    outtree->Branch("trig_PFHT370_pass", &trig_PFHT370_pass); 
    outtree->Branch("trig_PFHT430_pass", &trig_PFHT430_pass); 
    outtree->Branch("trig_PFHT510_pass", &trig_PFHT510_pass); 
    outtree->Branch("trig_PFHT590_pass", &trig_PFHT590_pass); 
    outtree->Branch("trig_PFHT680_pass", &trig_PFHT680_pass); 
    outtree->Branch("trig_PFHT780_pass", &trig_PFHT780_pass); 
    outtree->Branch("trig_PFHT890_pass", &trig_PFHT890_pass); 
    outtree->Branch("trig_PFHT1050_pass", &trig_PFHT1050_pass); 
    outtree->Branch("ev_run", &ev_run);
    outtree->Branch("ev_lumi", &ev_lumi);
    outtree->Branch("ev_nPV", &ev_nPV);
    outtree->Branch("ev_PFHT", &ev_PFHT);
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


    int nentries = intree->GetEntries();
    for(int i=0; i<nentries; i++){

        intree->GetEntry(i);

        pv_trk_d0_pvunbiased->clear();
        pv_trk_dz_pvunbiased->clear();
        pv_trk_pt->clear();
        pv_trk_eta->clear();
        pv_trk_phi->clear();

        ev_PFHT = 0;

        if( (!trig_PFHT180_pass) && (!trig_PFHT250_pass) && (!trig_PFHT370_pass) && (!trig_PFHT430_pass) && (!trig_PFHT510_pass) && (!trig_PFHT590_pass) && (!trig_PFHT680_pass) && (!trig_PFHT780_pass) && (!trig_PFHT890_pass) && (!trig_PFHT1050_pass) ) continue; 

        ev_run = myev_run;
        ev_lumi = myev_lumi;
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

        for(int j=0; j<mypv_SumTrackPt->size(); j++){
            ev_PFHT += mypv_SumTrackPt->at(j);
        }

        int pv_track_size = (*mypv_trk_d0_pvunbiased)[0].size();
        for(int j=0; j<pv_track_size; j++){
            pv_trk_d0_pvunbiased->push_back( (*mypv_trk_d0_pvunbiased)[0][j] );
            pv_trk_dz_pvunbiased->push_back( (*mypv_trk_dz_pvunbiased)[0][j] );
            pv_trk_pt->push_back( (*mypv_trk_pt)[0][j] );
            pv_trk_eta->push_back( (*mypv_trk_eta)[0][j] );
            pv_trk_phi->push_back( (*mypv_trk_phi)[0][j] );
        }

        outtree->Fill();
    }

    outfile->cd(); 
    outtree->Write();
    outfile->Close();

    return 0;
}
