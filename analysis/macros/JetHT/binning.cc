#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <numeric>

#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TH1.h>
#include <TMath.h>

#include <nlohmann/json.hpp>

const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t nbins = 200;
const Float_t sqrt_2 = sqrt(2);

// Trigger priority list (highest to lowest)
const int N_TRIGS = 10;
const char *TRIG_NAMES[N_TRIGS] = {
    "PFHT1050",
    "PFHT890",
    "PFHT780",
    "PFHT680",
    "PFHT590",
    "PFHT510",
    "PFHT430",
    "PFHT370",
    "PFHT250",
    "PFHT180"};

const char *TRIG_BRANCH_NAMES[N_TRIGS] = {
    "trig_PFHT1050_pass",
    "trig_PFHT890_pass",
    "trig_PFHT780_pass",
    "trig_PFHT680_pass",
    "trig_PFHT590_pass",
    "trig_PFHT510_pass",
    "trig_PFHT430_pass",
    "trig_PFHT370_pass",
    "trig_PFHT250_pass",
    "trig_PFHT180_pass"};

std::vector<Float_t> get_quantile_edges_from_hist(TH1 *hist, Int_t nbins, Float_t low, Float_t high)
{
    const Int_t n = nbins + 1;

    std::vector<Double_t> probs(n), qs_double(n);

    std::iota(probs.begin(), probs.end(), 0);
    for (auto &x : probs)
        x /= nbins;

    hist->GetQuantiles(n, qs_double.data(), probs.data());

    std::vector<Float_t> qs(n);
    for (Int_t i = 0; i < n; ++i)
        qs[i] = static_cast<Float_t>(qs_double[i]);

    qs.front() = low;
    qs.back() = high;

    return qs;
}

Int_t binning()
{
    // 打开 data / MC 文件
    std::ifstream tuplelist_file("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/tuplelist.json");
    nlohmann::json tuplelist;
    tuplelist_file >> tuplelist;
    tuplelist_file.close();

    // ---------- Read PU weights ----------
    std::ifstream pileup_weightfile(storage_dir + "/pileup/2022EE/pileup_ratio.json");
    nlohmann::json pu_weights_json;
    pileup_weightfile >> pu_weights_json;
    pileup_weightfile.close();

    std::vector<Float_t> PU_weights(pu_weights_json.size());
    for (const auto &item : pu_weights_json)
    {
        Int_t bin = item["bin"];
        PU_weights[bin - 1] = item["content"];
    }

    // ---------- Read cross section weight JSON ----------
    std::ifstream xsec_file(storage_dir + "/pileup/2022EE/xsec_weight.json");
    nlohmann::json xsec_json;
    xsec_file >> xsec_json;
    xsec_file.close();

    std::map<std::string, Float_t> xsec_weight_map;
    for (const auto &item : xsec_json)
        xsec_weight_map[item["dataset"].get<std::string>()] = item["xsec_weight"].get<Float_t>();

    // ---------- Read PS_weight JSON ----------
    std::ifstream ps_file(storage_dir + "/pileup/2022EE/PS_weight.json");
    nlohmann::json ps_json;
    ps_file >> ps_json;
    ps_file.close();

    Float_t ps_weights_final[N_TRIGS];
    for (Int_t it = 0; it < N_TRIGS; ++it)
        ps_weights_final[it] = ps_json[it]["PS_weight"].get<Float_t>();

    // ---------- Read trigger_mask JSON ----------
    std::ifstream mask_file(storage_dir + "/pileup/2022EE/trigger_mask.json");
    nlohmann::json mask_json;
    mask_file >> mask_json;
    mask_file.close();

    Float_t pv_SumTrackPt2;
    std::vector<Float_t>* pv_trk_pt = nullptr;
    std::vector<Float_t>* pv_trk_eta = nullptr;
    std::vector<Float_t>* pv_trk_phi = nullptr;
    Int_t NumTrueInts;
    Bool_t trig_pass_raw[N_TRIGS];

    auto bind_common_branches = [&](TChain *tree)
    {
        tree->SetBranchAddress("pv_SumTrackPt2", &pv_SumTrackPt2);
        tree->SetBranchAddress("pv_trk_pt", &pv_trk_pt);
        tree->SetBranchAddress("pv_trk_eta", &pv_trk_eta);
        tree->SetBranchAddress("pv_trk_phi", &pv_trk_phi);
        tree->SetBranchAddress("NumTrueInts", &NumTrueInts);
    };

    auto bind_trig_branches = [&](TChain *tree)
    {
        for (int it = 0; it < N_TRIGS; ++it)
            tree->SetBranchAddress(TRIG_BRANCH_NAMES[it], &trig_pass_raw[it]);
    };

    TH1F *h_pv_SumTrackPt2_sqrt = new TH1F("h_pv_SumTrackPt2_sqrt", "", 10000, 30.0, 400.0);
    TH1F *h_pv_trk_pt = new TH1F("h_pv_trk_pt", "", 10000, 0.1, 15.0);
    TH1F *h_pv_trk_pt_uleta = new TH1F("h_pv_trk_pt_uleta", "", 10000, 1, 15.0);
    TH1F *h_pv_trk_eta = new TH1F("h_pv_trk_eta", "", 10000, -3.0, 3.0);
    TH1F *h_pv_trk_phi = new TH1F("h_pv_trk_phi", "", 10000, -3.1416, 3.1416);

    for (auto &mc_entry : tuplelist["2022EE"]["mc"])
    {
        std::string path = mc_entry["path"];
        std::string dataset = mc_entry["dataset"];
        if (path.empty())
            continue;

        // --- xsec weight from JSON ---
        if (xsec_weight_map.find(dataset) == xsec_weight_map.end())
            continue;
        Float_t xsec_weight = xsec_weight_map[dataset];

        // --- PS weights with mask from JSON ---
        Float_t ps_weights_entry[N_TRIGS];
        for (Int_t it = 0; it < N_TRIGS; ++it)
        {
            std::string trig_key = std::string(TRIG_NAMES[it]);
            Bool_t masked = mask_json["2022EE"][dataset][trig_key].get<Bool_t>();
            ps_weights_entry[it] = masked ? 0.0 : ps_weights_final[it];
        }

        // --- Build MC event tree for this entry ---
        TChain *mctree_entry = new TChain("residuals/tree");
        mctree_entry->Add(TString(path) + "/*.root");

        bind_common_branches(mctree_entry);
        bind_trig_branches(mctree_entry);

        Long64_t nMC_entry = mctree_entry->GetEntries();

        for (Long64_t ie = 0; ie < nMC_entry; ++ie)
        {
            mctree_entry->GetEntry(ie);

            Float_t w_ps = 0.0;
            for (Int_t it = 0; it < N_TRIGS; ++it)
            {
                if (trig_pass_raw[it])
                {
                    w_ps = ps_weights_entry[it];
                    break;
                }
            }

            Float_t w_pu = (NumTrueInts >= 1 && NumTrueInts <= (Int_t)PU_weights.size()) ? PU_weights[NumTrueInts - 1] : 0.0;

            Float_t w = w_pu * xsec_weight * w_ps;

            h_pv_SumTrackPt2_sqrt->Fill(sqrt(pv_SumTrackPt2), w);

            size_t nTrk = pv_trk_pt->size();
            for (Int_t it = 0; it < static_cast<Int_t>(nTrk); ++it)
            {
                h_pv_trk_pt->Fill(pv_trk_pt->at(it), w);
                h_pv_trk_eta->Fill(pv_trk_eta->at(it), w);
                h_pv_trk_phi->Fill(pv_trk_phi->at(it), w);
                if (abs(pv_trk_eta->at(it)) > 2.5 && abs(pv_trk_eta->at(it)) < 3.0)
                    h_pv_trk_pt_uleta->Fill(pv_trk_pt->at(it), w);
            }
        }

        delete mctree_entry;
    }

    std::vector<Float_t> pv_SumTrackPt_binedges = get_quantile_edges_from_hist(h_pv_SumTrackPt2_sqrt, 50, 30.0, 400.0);
    std::vector<Float_t> pv_trk_pt_binedges = get_quantile_edges_from_hist(h_pv_trk_pt, 200, 0.1, 15.0);
    std::vector<Float_t> pv_trk_pt_uleta_binedges = get_quantile_edges_from_hist(h_pv_trk_pt_uleta, 200, 1, 15.0);
    std::vector<Float_t> pv_trk_eta_binedges = get_quantile_edges_from_hist(h_pv_trk_eta, 200, -3.0, 3.0);
    std::vector<Float_t> pv_trk_phi_binedges = get_quantile_edges_from_hist(h_pv_trk_phi, 200, -3.1416, 3.1416);

    nlohmann::json splitparams = {
        {"pv_SumTrackPt2_sqrt", pv_SumTrackPt_binedges},
        {"pv_trk_pt", pv_trk_pt_binedges},
        {"pv_trk_pt_uleta", pv_trk_pt_uleta_binedges},
        {"pv_trk_eta", pv_trk_eta_binedges},
        {"pv_trk_phi", pv_trk_phi_binedges}};

    TString binning_json = "/eos/home-k/kakang/IPres/analysis/JetHT/json/binning_mc.json";

    std::ofstream outfile(binning_json);
    outfile << splitparams.dump(4);

    std::cout << "Binning saved to " << binning_json << std::endl;

    return 0;
}