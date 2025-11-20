#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

Int_t mkfile(TString infilename, TString outfilename, Bool_t isData)
{
    gInterpreter->GenerateDictionary("vector<vector<float>>", "vector");

    TFile *infile = TFile::Open(infilename);
    if (!infile || infile->IsZombie())
    {
        std::cerr << "Error opening file: " << infilename << std::endl;
        return 1;
    }
    TTree *intree = (TTree *)infile->Get("residuals/tree");
    if (!intree)
    {
        std::cerr << "Error: No tree found in file: " << infilename << std::endl;
        infile->Close();
        return 1;
    }

    Bool_t trig_ZeroBias_pass;
    Int_t ev_nPV;
    std::vector<Int_t> *pv_NTracks = nullptr;
    std::vector<Float_t> *pv_SumTrackPt2 = nullptr;
    std::vector<Float_t> *pv_x_p1 = nullptr;
    std::vector<Float_t> *pv_y_p1 = nullptr;
    std::vector<Float_t> *pv_z_p1 = nullptr;
    std::vector<Float_t> *pv_xError_p1 = nullptr;
    std::vector<Float_t> *pv_yError_p1 = nullptr;
    std::vector<Float_t> *pv_zError_p1 = nullptr;
    std::vector<Float_t> *pv_x_p2 = nullptr;
    std::vector<Float_t> *pv_y_p2 = nullptr;
    std::vector<Float_t> *pv_z_p2 = nullptr;
    std::vector<Float_t> *pv_xError_p2 = nullptr;
    std::vector<Float_t> *pv_yError_p2 = nullptr;
    std::vector<Float_t> *pv_zError_p2 = nullptr;
    std::vector<std::vector<Float_t>> *pv_trk_d0_pvunbiased = nullptr;
    std::vector<std::vector<Float_t>> *pv_trk_dz_pvunbiased = nullptr;
    std::vector<std::vector<Float_t>> *pv_trk_pt = nullptr;
    std::vector<std::vector<Float_t>> *pv_trk_eta = nullptr;
    std::vector<std::vector<Float_t>> *pv_trk_phi = nullptr;

    intree->SetBranchAddress("trig_ZeroBias_pass", &trig_ZeroBias_pass);
    intree->SetBranchAddress("ev_nPV", &ev_nPV);
    intree->SetBranchAddress("pv_NTracks", &pv_NTracks);
    intree->SetBranchAddress("pv_SumTrackPt2", &pv_SumTrackPt2);
    intree->SetBranchAddress("pv_x_p1", &pv_x_p1);
    intree->SetBranchAddress("pv_y_p1", &pv_y_p1);
    intree->SetBranchAddress("pv_z_p1", &pv_z_p1);
    intree->SetBranchAddress("pv_xError_p1", &pv_xError_p1);
    intree->SetBranchAddress("pv_yError_p1", &pv_yError_p1);
    intree->SetBranchAddress("pv_zError_p1", &pv_zError_p1);
    intree->SetBranchAddress("pv_x_p2", &pv_x_p2);
    intree->SetBranchAddress("pv_y_p2", &pv_y_p2);
    intree->SetBranchAddress("pv_z_p2", &pv_z_p2);
    intree->SetBranchAddress("pv_xError_p2", &pv_xError_p2);
    intree->SetBranchAddress("pv_yError_p2", &pv_yError_p2);
    intree->SetBranchAddress("pv_zError_p2", &pv_zError_p2);
    intree->SetBranchAddress("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    intree->SetBranchAddress("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);
    intree->SetBranchAddress("pv_trk_pt", &pv_trk_pt);
    intree->SetBranchAddress("pv_trk_eta", &pv_trk_eta);
    intree->SetBranchAddress("pv_trk_phi", &pv_trk_phi);

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *outtree = new TTree("mytree", "");

    Int_t ev_nPV_flatten;
    Int_t pv_NTracks_flatten;
    Float_t pv_SumTrackPt2_flatten;
    Float_t pv_x_p1_flatten;
    Float_t pv_y_p1_flatten;
    Float_t pv_z_p1_flatten;
    Float_t pv_xError_p1_flatten;
    Float_t pv_yError_p1_flatten;
    Float_t pv_zError_p1_flatten;
    Float_t pv_x_p2_flatten;
    Float_t pv_y_p2_flatten;
    Float_t pv_z_p2_flatten;
    Float_t pv_xError_p2_flatten;
    Float_t pv_yError_p2_flatten;
    Float_t pv_zError_p2_flatten;
    std::vector<Float_t> pv_trk_d0_pvunbiased_flatten;
    std::vector<Float_t> pv_trk_dz_pvunbiased_flatten;
    std::vector<Float_t> pv_trk_pt_flatten;
    std::vector<Float_t> pv_trk_eta_flatten;
    std::vector<Float_t> pv_trk_phi_flatten;

    outtree->Branch("ev_nPV", &ev_nPV_flatten);
    outtree->Branch("pv_NTracks", &pv_NTracks_flatten);
    outtree->Branch("pv_SumTrackPt2", &pv_SumTrackPt2_flatten);
    outtree->Branch("pv_x_p1", &pv_x_p1_flatten);
    outtree->Branch("pv_y_p1", &pv_y_p1_flatten);
    outtree->Branch("pv_z_p1", &pv_z_p1_flatten);
    outtree->Branch("pv_xError_p1", &pv_xError_p1_flatten);
    outtree->Branch("pv_yError_p1", &pv_yError_p1_flatten);
    outtree->Branch("pv_zError_p1", &pv_zError_p1_flatten);
    outtree->Branch("pv_x_p2", &pv_x_p2_flatten);
    outtree->Branch("pv_y_p2", &pv_y_p2_flatten);
    outtree->Branch("pv_z_p2", &pv_z_p2_flatten);
    outtree->Branch("pv_xError_p2", &pv_xError_p2_flatten);
    outtree->Branch("pv_yError_p2", &pv_yError_p2_flatten);
    outtree->Branch("pv_zError_p2", &pv_zError_p2_flatten);
    outtree->Branch("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased_flatten);
    outtree->Branch("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased_flatten);
    outtree->Branch("pv_trk_pt", &pv_trk_pt_flatten);
    outtree->Branch("pv_trk_eta", &pv_trk_eta_flatten);
    outtree->Branch("pv_trk_phi", &pv_trk_phi_flatten);

    Int_t NumTrueInts = -1;
    Int_t NumPUInts = -1;
    if (!isData)
    {
        intree->SetBranchAddress("NumTrueInts", &NumTrueInts);
        outtree->Branch("NumTrueInts", &NumTrueInts);
        intree->SetBranchAddress("NumPUInts", &NumPUInts);
        outtree->Branch("NumPUInts", &NumPUInts);
    }

    Int_t nentries = intree->GetEntries();
    for (Int_t i = 0; i < nentries; i++)
    {

        intree->GetEntry(i);

        pv_trk_d0_pvunbiased_flatten.clear();
        pv_trk_dz_pvunbiased_flatten.clear();
        pv_trk_pt_flatten.clear();
        pv_trk_eta_flatten.clear();
        pv_trk_phi_flatten.clear();

        if (!trig_ZeroBias_pass)
            continue;
        if (pv_NTracks->size() == 0)
            continue;
        if (pv_x_p1->size() == 0)
            continue;

        ev_nPV_flatten = ev_nPV;
        pv_NTracks_flatten = pv_NTracks->at(0);
        pv_SumTrackPt2_flatten = pv_SumTrackPt2->at(0);
        pv_x_p1_flatten = pv_x_p1->at(0);
        pv_y_p1_flatten = pv_y_p1->at(0);
        pv_z_p1_flatten = pv_z_p1->at(0);
        pv_xError_p1_flatten = pv_xError_p1->at(0);
        pv_yError_p1_flatten = pv_yError_p1->at(0);
        pv_zError_p1_flatten = pv_zError_p1->at(0);
        pv_x_p2_flatten = pv_x_p2->at(0);
        pv_y_p2_flatten = pv_y_p2->at(0);
        pv_z_p2_flatten = pv_z_p2->at(0);
        pv_xError_p2_flatten = pv_xError_p2->at(0);
        pv_yError_p2_flatten = pv_yError_p2->at(0);
        pv_zError_p2_flatten = pv_zError_p2->at(0);

        if (pv_trk_d0_pvunbiased->size() == 0)
            continue;
        Int_t track_size = (*pv_trk_d0_pvunbiased)[0].size();

        for (Int_t j = 0; j < track_size; j++)
        {
            pv_trk_d0_pvunbiased_flatten.push_back((*pv_trk_d0_pvunbiased)[0][j]);
            pv_trk_dz_pvunbiased_flatten.push_back((*pv_trk_dz_pvunbiased)[0][j]);
            pv_trk_pt_flatten.push_back((*pv_trk_pt)[0][j]);
            pv_trk_eta_flatten.push_back((*pv_trk_eta)[0][j]);
            pv_trk_phi_flatten.push_back((*pv_trk_phi)[0][j]);
        }

        outtree->Fill();
    }

    outfile->cd();
    outtree->Write();
    outfile->Close();
    delete outfile;
    infile->Close();
    delete infile;

    return 0;
}
