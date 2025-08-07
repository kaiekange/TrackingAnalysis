#include <iostream>
#include <vector>
#include <TString.h>
#include "input_list.cc"
#include "../../functions/fill_file_vector.cc"

int mkfile( int idx ) {

    gErrorIgnoreLevel = kError;
    /* gInterpreter->GenerateDictionary("vector<vector<float>>", "vector"); */

    std::vector<TString> file_vector = fill_file_vector(filepath[idx] + "*/*.root");
    
    TFile *outfile = new TFile(Form("%sanalysis.root", filepath[idx].c_str()), "RECREATE");
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
