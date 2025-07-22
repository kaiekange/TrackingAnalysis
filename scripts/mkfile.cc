#include <iostream>
#include <vector>
#include <TString.h>
#include "input_list.cc"
#include "fill_file_vector.cc"

int mkfile( int idx ) {

    gErrorIgnoreLevel = kError;
    /* gInterpreter->GenerateDictionary("vector<vector<float>>", "vector"); */

    std::vector<TString> file_vector = fill_file_vector(filepath[idx] + "*/*.root");
    
    TFile *outfile = new TFile(Form("%sanalysis.root", filepath[idx].c_str()), "RECREATE");
    TTree *outtree = new TTree("mytree", "");
    
    float bs_x0;
    float bs_y0;
    float bs_z0;
    float bs_x0Error;
    float bs_y0Error;
    float bs_z0Error;
    int ev_run;
    int ev_lumi;
    float pv_x;
    float pv_y; 
    float pv_z;
    float pv_xError;
    float pv_yError;
    float pv_zError;
    float pv_SumTrackPt2;
    float pv_x_p1;
    float pv_y_p1;
    float pv_z_p1;
    float pv_x_p2;
    float pv_y_p2;
    float pv_z_p2;
    float pv_xError_p1;
    float pv_yError_p1;
    float pv_zError_p1;
    float pv_xError_p2;
    float pv_yError_p2;
    float pv_zError_p2;
    vector<float> * pv_trk_d0_pvunbiased = new vector<float>();
    vector<float> * pv_trk_dz_pvunbiased = new vector<float>();
    vector<float> * pv_trk_pvunbiased_NTracks = new vector<float>();
    vector<float> * pv_trk_pvunbiased_SumTrackPt2 = new vector<float>();
    vector<float> * pv_trk_d0_bs = new vector<float>();
    vector<float> * pv_trk_dz_bs = new vector<float>();
    vector<float> * pv_trk_pt = new vector<float>();
    vector<float> * pv_trk_eta = new vector<float>();
    vector<float> * pv_trk_phi = new vector<float>();

    outtree->Branch("pv_x", &pv_x);
    outtree->Branch("pv_y", &pv_y); 
    outtree->Branch("pv_z", &pv_z);
    outtree->Branch("pv_xError", &pv_xError);
    outtree->Branch("pv_yError", &pv_yError);
    outtree->Branch("pv_zError", &pv_zError);
    outtree->Branch("bs_x0", &bs_x0);
    outtree->Branch("bs_y0", &bs_y0);
    outtree->Branch("bs_z0", &bs_z0);
    outtree->Branch("bs_x0Error", &bs_x0Error);
    outtree->Branch("bs_y0Error", &bs_y0Error);
    outtree->Branch("bs_z0Error", &bs_z0Error);
    outtree->Branch("ev_run", &ev_run);
    outtree->Branch("ev_lumi", &ev_lumi);
    outtree->Branch("pv_SumTrackPt2", &pv_SumTrackPt2);
    outtree->Branch("pv_x_p1", &pv_x_p1);
    outtree->Branch("pv_y_p1", &pv_y_p1);
    outtree->Branch("pv_z_p1", &pv_z_p1);
    outtree->Branch("pv_x_p2", &pv_x_p2);
    outtree->Branch("pv_y_p2", &pv_y_p2);
    outtree->Branch("pv_z_p2", &pv_z_p2);
    outtree->Branch("pv_xError_p1", &pv_xError_p1);
    outtree->Branch("pv_yError_p1", &pv_yError_p1);
    outtree->Branch("pv_zError_p1", &pv_zError_p1);
    outtree->Branch("pv_xError_p2", &pv_xError_p2);
    outtree->Branch("pv_yError_p2", &pv_yError_p2);
    outtree->Branch("pv_zError_p2", &pv_zError_p2);
    outtree->Branch("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    outtree->Branch("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);
    outtree->Branch("pv_trk_pvunbiased_NTracks", &pv_trk_pvunbiased_NTracks);
    outtree->Branch("pv_trk_pvunbiased_SumTrackPt2", &pv_trk_pvunbiased_SumTrackPt2);
    outtree->Branch("pv_trk_d0_bs", &pv_trk_d0_bs);
    outtree->Branch("pv_trk_dz_bs", &pv_trk_dz_bs);
    outtree->Branch("pv_trk_pt", &pv_trk_pt);
    outtree->Branch("pv_trk_eta", &pv_trk_eta);
    outtree->Branch("pv_trk_phi", &pv_trk_phi);

    /* int nfile = 5; */ 
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

        float mybs_x0 = 0.;
        float mybs_y0 = 0.; 
        float mybs_z0 = 0.;
        float mybs_x0Error = 0.;
        float mybs_y0Error = 0.;
        float mybs_z0Error = 0.;
        int myev_run = 0;
        int myev_lumi = 0;
        vector<float> *mypv_x = nullptr;
        vector<float> *mypv_y = nullptr;
        vector<float> *mypv_z = nullptr;
        vector<float> *mypv_xError = nullptr;
        vector<float> *mypv_yError = nullptr;
        vector<float> *mypv_zError = nullptr;
        vector<float> *mypv_SumTrackPt2 = nullptr;
        vector<float> *mypv_x_p1 = nullptr;
        vector<float> *mypv_y_p1 = nullptr;
        vector<float> *mypv_z_p1 = nullptr;
        vector<float> *mypv_x_p2 = nullptr;
        vector<float> *mypv_y_p2 = nullptr;
        vector<float> *mypv_z_p2 = nullptr;
        vector<float> *mypv_xError_p1 = nullptr;
        vector<float> *mypv_yError_p1 = nullptr;
        vector<float> *mypv_zError_p1 = nullptr;
        vector<float> *mypv_xError_p2 = nullptr;
        vector<float> *mypv_yError_p2 = nullptr;
        vector<float> *mypv_zError_p2 = nullptr;
        vector<vector<float>> *mypv_trk_d0_pvunbiased = nullptr;
        vector<vector<float>> *mypv_trk_dz_pvunbiased = nullptr;
        vector<vector<float>> *mypv_trk_pvunbiased_NTracks = nullptr;
        vector<vector<float>> *mypv_trk_pvunbiased_SumTrackPt2 = nullptr;
        vector<vector<float>> *mypv_trk_d0_bs = nullptr;
        vector<vector<float>> *mypv_trk_dz_bs = nullptr;
        vector<vector<float>> *mypv_trk_pt = nullptr;
        vector<vector<float>> *mypv_trk_eta = nullptr;
        vector<vector<float>> *mypv_trk_phi = nullptr;
        
        intree->SetBranchAddress("bs_x0", &mybs_x0);
        intree->SetBranchAddress("bs_y0", &mybs_y0);
        intree->SetBranchAddress("bs_z0", &mybs_z0);
        intree->SetBranchAddress("bs_x0Error", &mybs_x0Error);
        intree->SetBranchAddress("bs_y0Error", &mybs_y0Error);
        intree->SetBranchAddress("bs_z0Error", &mybs_z0Error);
        intree->SetBranchAddress("ev_run", &myev_run);
        intree->SetBranchAddress("ev_lumi", &myev_lumi);
        intree->SetBranchAddress("pv_x", &mypv_x);
        intree->SetBranchAddress("pv_y", &mypv_y);
        intree->SetBranchAddress("pv_z", &mypv_z);
        intree->SetBranchAddress("pv_xError", &mypv_xError);
        intree->SetBranchAddress("pv_yError", &mypv_yError);
        intree->SetBranchAddress("pv_zError", &mypv_zError);
        intree->SetBranchAddress("pv_SumTrackPt2", &mypv_SumTrackPt2);
        intree->SetBranchAddress("pv_x_p1", &mypv_x_p1);
        intree->SetBranchAddress("pv_y_p1", &mypv_y_p1);
        intree->SetBranchAddress("pv_z_p1", &mypv_z_p1);
        intree->SetBranchAddress("pv_x_p2", &mypv_x_p2);
        intree->SetBranchAddress("pv_y_p2", &mypv_y_p2);
        intree->SetBranchAddress("pv_z_p2", &mypv_z_p2);
        intree->SetBranchAddress("pv_xError_p1", &mypv_xError_p1);
        intree->SetBranchAddress("pv_yError_p1", &mypv_yError_p1);
        intree->SetBranchAddress("pv_zError_p1", &mypv_zError_p1);
        intree->SetBranchAddress("pv_xError_p2", &mypv_xError_p2);
        intree->SetBranchAddress("pv_yError_p2", &mypv_yError_p2);
        intree->SetBranchAddress("pv_zError_p2", &mypv_zError_p2);
        intree->SetBranchAddress("pv_trk_d0_pvunbiased", &mypv_trk_d0_pvunbiased);
        intree->SetBranchAddress("pv_trk_dz_pvunbiased", &mypv_trk_dz_pvunbiased);
        intree->SetBranchAddress("pv_trk_pvunbiased_NTracks", &mypv_trk_pvunbiased_NTracks);
        intree->SetBranchAddress("pv_trk_pvunbiased_SumTrackPt2", &mypv_trk_pvunbiased_SumTrackPt2);
        intree->SetBranchAddress("pv_trk_d0_bs", &mypv_trk_d0_bs);
        intree->SetBranchAddress("pv_trk_dz_bs", &mypv_trk_dz_bs);
        intree->SetBranchAddress("pv_trk_pt", &mypv_trk_pt);
        intree->SetBranchAddress("pv_trk_eta", &mypv_trk_eta);
        intree->SetBranchAddress("pv_trk_phi", &mypv_trk_phi);
        int nentries = intree->GetEntries();
        /* for(int j=0; j<5; j++){ */
        for(int j=0; j<nentries; j++){
            
            intree->GetEntry(j);

            pv_trk_d0_pvunbiased->clear();
            pv_trk_dz_pvunbiased->clear();
            pv_trk_pvunbiased_NTracks->clear();
            pv_trk_pvunbiased_SumTrackPt2->clear();
            pv_trk_d0_bs->clear();
            pv_trk_dz_bs->clear();
            pv_trk_pt->clear();
            pv_trk_eta->clear();
            pv_trk_phi->clear();
            
            bs_x0 = mybs_x0;
            bs_y0 = mybs_y0;
            bs_z0 = mybs_z0;
            bs_x0Error = mybs_x0Error;
            bs_y0Error = mybs_y0Error;
            bs_z0Error = mybs_z0Error;
            ev_run = myev_run;
            ev_lumi = myev_lumi;
            pv_x = mypv_x->at(0);
            pv_y = mypv_y->at(0);
            pv_z = mypv_z->at(0);
            pv_xError = mypv_xError->at(0);
            pv_yError = mypv_yError->at(0);
            pv_zError = mypv_zError->at(0);
            pv_SumTrackPt2 = mypv_SumTrackPt2->at(0);
            pv_x_p1 = mypv_x_p1->at(0);
            pv_y_p1 = mypv_y_p1->at(0);
            pv_z_p1 = mypv_z_p1->at(0);
            pv_x_p2 = mypv_x_p2->at(0);
            pv_y_p2 = mypv_y_p2->at(0);
            pv_z_p2 = mypv_z_p2->at(0);
            pv_xError_p1 = mypv_xError_p1->at(0);
            pv_yError_p1 = mypv_yError_p1->at(0);
            pv_zError_p1 = mypv_zError_p1->at(0);
            pv_xError_p2 = mypv_xError_p2->at(0);
            pv_yError_p2 = mypv_yError_p2->at(0);
            pv_zError_p2 = mypv_zError_p2->at(0);

            int pv_track_size = (*mypv_trk_d0_pvunbiased)[0].size();
            for(int k=0; k<pv_track_size; k++){
                pv_trk_d0_pvunbiased->push_back( (*mypv_trk_d0_pvunbiased)[0][k] );
                pv_trk_dz_pvunbiased->push_back( (*mypv_trk_dz_pvunbiased)[0][k] );
                pv_trk_pvunbiased_NTracks->push_back( (*mypv_trk_pvunbiased_NTracks)[0][k] ); 
                pv_trk_pvunbiased_SumTrackPt2->push_back( (*mypv_trk_pvunbiased_SumTrackPt2)[0][k] ); 
                pv_trk_d0_bs->push_back( (*mypv_trk_d0_bs)[0][k] );
                pv_trk_dz_bs->push_back( (*mypv_trk_dz_bs)[0][k] );
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
