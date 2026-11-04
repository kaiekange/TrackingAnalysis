// Steps 1 and 2 of the PV/IP resolution measurement (step 3 = pv_res.cc, then ip_res.cc).
// Each condor job handles files_per_job files of <storage_dir>/hists/<period>/files.txt
// (written by fill_hists.py):
//
//   fill_hists.cc+("2022_22Sep", "range", <chunk>, 10)  -> hists/<period>/range/range_<chunk>.root
//       n / sum / sum2 of every histogram variable inside its initial window; hadd -> range.root
//   fill_hists.cc+("2022_22Sep", "fill", <chunk>, 10)   -> hists/<period>/fill/fill_<chunk>.root
//       histograms with ranges mean +- 8 RMS from range.root; hadd -> fill.root
//
// fill.root contains
//   pv_<k>_<suffix>             PV split histograms, k = sqrt(SumPt2) bin
//   ip_<d0|dz>_<idx>_<suffix>   IP histograms, idx = pt/eta/phi bin
//   pverr_<d0|dz>_<idx>_<suffix> per IP histogram: sum over its tracks of the unbiased-PV covariance
//                               projected on d0/dz, split by the sqrt(SumPt2) bin k of each track's
//                               unbiased PV (x axis; the pull lookup) and
//                               covariance term (y bins: xx yy zz xy xz yz, 7 = track count).
//                               ip_res.cc applies the PV pulls to these sums.

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>

#include <nlohmann/json.hpp>

namespace fill
{
    const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

    const Int_t nbins = 200;
    const Double_t nsigma = 8.0;
    const Float_t null = -777;

    // must match COMB_SUFFIX in pv_res.cc / ip_res.cc (histograms are looked up by suffix)
    enum PvComb { PV_X = 0, PV_Y, PV_Z, PULL_X, PULL_Y, PULL_Z, N_PV };
    const char *PV_SUFFIX[N_PV] = {"pvx", "pvy", "pvz", "pullx", "pully", "pullz"};
    const Double_t pv_init_min[N_PV] = {-300, -300, -300, -10, -10, -10};
    const Double_t pv_init_max[N_PV] = {300, 300, 300, 10, 10, 10};

    enum IpComb { PT_LOETA = 0, PT_HIETA, PT_ULETA, PT_ALLETA, ETA_LOPT, ETA_HIPT, ETA_ULPT, ETA_ALLPT,
                  PHI_LOPT, PHI_HIPT, PHI_ULPT, PHI_ALLPT, N_IP };
    const char *IP_SUFFIX[N_IP] = {"pt_loeta", "pt_hieta", "pt_uleta", "pt_alleta", "eta_lopt", "eta_hipt",
                                   "eta_ulpt", "eta_allpt", "phi_lopt", "phi_hipt", "phi_ulpt", "phi_allpt"};
    const Double_t init_d0_min[N_IP] = {-2000, -4000, -6000, -4000, -2000, -1000, -400, -2000, -2000, -500, -400, -1500};
    const Double_t init_d0_max[N_IP] = {2000, 4000, 6000, 4000, 2000, 1000, 400, 2000, 2000, 500, 400, 1500};
    const Double_t init_dz_min[N_IP] = {-2000, -10000, -10000, -10000, -8000, -4000, -2000, -5000, -3000, -1500, -600, -2000};
    const Double_t init_dz_max[N_IP] = {2000, 10000, 10000, 10000, 8000, 4000, 2000, 5000, 3000, 1500, 600, 2000};

    enum CovTerm { XX = 0, YY, ZZ, XY, XZ, YZ, NTRK, N_TERM };

    // Groups in the track's own PV error projected on d0/dz (no pull). Each IP histogram is also split
    // into groups [edge_j, edge_j+1). d0: fixed ranges, the same in every histogram, inside the central
    // 90 % of the 2022C error distribution (5 %: 2.82, 95 %: 7.40 um); tracks outside are in no group.
    // dz: N_GRP groups at the histogram's own quantiles of that error, from the range step.
    const std::vector<Double_t> D0_GRP_EDGES = {2.8, 3.5, 4.0, 4.5, 5.0, 6.0, 7.4};  // um
    const Double_t GRP_OPEN = 1e9;  // upper edge of an open-ended last group
    const Int_t N_GRP = 4;
    const Double_t GRP_Q[N_GRP - 1] = {0.25, 0.50, 0.75};
    const Int_t SIG_NBINS[2] = {400, 1000};       // d0, dz
    const Double_t SIG_MAX[2] = {40.0, 200.0};    // um

    struct Stat
    {
        Long64_t n = 0;
        Double_t sum = 0.0;
        Double_t sum2 = 0.0;
        void Fill(Double_t x) { n++; sum += x; sum2 += x * x; }
        Bool_t Valid() const { return n > 0; }
        Double_t Mean() const { return sum / n; }
        Double_t Sigma() const
        {
            Double_t m = Mean(), v = sum2 / n - m * m;
            return v > 0.0 ? std::sqrt(v) : 0.0;
        }
    };

    // histogram range as before: mean +- nsigma * RMS of the entries inside the initial window
    void hist_range(const Stat &s, Double_t init_min, Double_t init_max, Double_t &lo, Double_t &hi)
    {
        Double_t mean = s.Mean(), sigma = s.Sigma();
        if (sigma <= 0.0)
        {
            mean = 0.5 * (init_min + init_max);
            sigma = (init_max - init_min) / (2.0 * nsigma);
        }
        lo = mean - nsigma * sigma;
        hi = mean + nsigma * sigma;
    }

    // bin index with the old strict edges (lo < x < hi); -1 if outside or exactly on an edge
    Int_t find_bin(const std::vector<Float_t> &edges, Float_t x)
    {
        auto it = std::upper_bound(edges.begin(), edges.end(), x);
        Int_t i = Int_t(it - edges.begin()) - 1;
        if (i < 0 || i + 1 >= (Int_t)edges.size())
            return -1;
        return (x > edges[i] && x < edges[i + 1]) ? i : -1;
    }
}

Int_t fill_hists(TString period, TString step, Int_t chunk, Int_t files_per_job)
{
    using namespace fill;

    if (step != "range" && step != "fill")
    {
        std::cerr << "[ERROR] fill_hists: step must be \"range\" or \"fill\"" << std::endl;
        gSystem->Exit(1);
    }
    const TString outdir = storage_dir + "/hists/" + period;
    gSystem->mkdir(outdir + "/" + step, kTRUE);

    // ---------- Inputs ----------
    std::ifstream binning_file(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    binning_file >> binning;

    const auto pv_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();
    const auto pt_edges = binning["pv_trk_pt"].get<std::vector<Float_t>>();
    const auto pt_uleta_edges = binning["pv_trk_pt_uleta"].get<std::vector<Float_t>>();
    const auto eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    const auto phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();
    const Int_t n_pv = pv_edges.size() - 1;
    const Int_t n_ip = pt_edges.size() - 1;
    if ((Int_t)pt_uleta_edges.size() - 1 != n_ip || (Int_t)eta_edges.size() - 1 != n_ip || (Int_t)phi_edges.size() - 1 != n_ip)
    {
        std::cerr << "[ERROR] fill_hists: pt/pt_uleta/eta/phi binnings must have the same number of bins" << std::endl;
        gSystem->Exit(1);
    }

    // this job's files: lines [chunk * files_per_job, (chunk + 1) * files_per_job) of files.txt,
    // which fill_hists.py lists on the submit host (no directory listing on the worker)
    std::ifstream filelist(outdir + "/files.txt");
    std::vector<std::string> files;
    for (std::string line; std::getline(filelist, line);)
        if (!line.empty())
            files.push_back(line);
    TChain *chain = new TChain("residuals/tree");
    for (Int_t i = chunk * files_per_job; i < std::min<Int_t>((chunk + 1) * files_per_job, files.size()); ++i)
        chain->Add(files[i].c_str());

    // an xrootd/auth failure leaves the chain without events; exit non-zero so condor sees (and retries) it
    const Long64_t n_entries = chain->GetEntries();
    std::cout << "[INFO] fill_hists: step " << step << ", chunk " << chunk << ": " << chain->GetListOfFiles()->GetEntries()
              << " files, " << n_entries << " events" << std::endl;
    if (chain->GetListOfFiles()->GetEntries() == 0 || n_entries <= 0)
    {
        std::cerr << "[ERROR] fill_hists: no input files or events" << std::endl;
        gSystem->Exit(1);
    }
    // TChain skips a file it cannot open (it only prints an error), so check every file has entries
    for (Int_t i = 0; i < chain->GetNtrees(); ++i)
        if (chain->GetTreeOffset()[i + 1] - chain->GetTreeOffset()[i] <= 0)
        {
            std::cerr << "[ERROR] fill_hists: no entries from " << chain->GetListOfFiles()->At(i)->GetTitle() << std::endl;
            gSystem->Exit(1);
        }

    // ---------- Branches ----------
    Float_t pv_SumTrackPt2;
    Float_t x1, y1, z1, x2, y2, z2, ex1, ey1, ez1, ex2, ey2, ez2;
    Bool_t valid1, fake1, valid2, fake2;
    std::vector<Float_t> *pt = nullptr, *eta = nullptr, *phi = nullptr, *d0 = nullptr, *dz = nullptr;
    std::vector<Bool_t> *u_valid = nullptr, *u_fake = nullptr;
    std::vector<Float_t> *u_ex = nullptr, *u_ey = nullptr, *u_ez = nullptr, *u_cxy = nullptr, *u_cxz = nullptr, *u_cyz = nullptr, *u_spt2 = nullptr;

    chain->SetBranchStatus("*", 0);
    auto bind = [&](const char *name, void *addr)
    {
        chain->SetBranchStatus(name, 1);
        chain->SetBranchAddress(name, addr);
    };
    bind("pv_SumTrackPt2", &pv_SumTrackPt2);
    bind("pv_x_p1", &x1); bind("pv_y_p1", &y1); bind("pv_z_p1", &z1);
    bind("pv_x_p2", &x2); bind("pv_y_p2", &y2); bind("pv_z_p2", &z2);
    bind("pv_xError_p1", &ex1); bind("pv_yError_p1", &ey1); bind("pv_zError_p1", &ez1);
    bind("pv_xError_p2", &ex2); bind("pv_yError_p2", &ey2); bind("pv_zError_p2", &ez2);
    bind("pv_IsValid_p1", &valid1); bind("pv_IsFake_p1", &fake1);
    bind("pv_IsValid_p2", &valid2); bind("pv_IsFake_p2", &fake2);
    bind("pv_trk_pt", &pt); bind("pv_trk_eta", &eta); bind("pv_trk_phi", &phi);
    bind("pv_trk_d0_pvunbiased", &d0); bind("pv_trk_dz_pvunbiased", &dz);
    bind("pv_trk_pvunbiased_IsValid", &u_valid); bind("pv_trk_pvunbiased_IsFake", &u_fake);
    // the errors are needed in both steps: tracks whose unbiased PV has a non-finite error
    // (negative variance from the fit) are dropped everywhere
    bind("pv_trk_pvunbiased_xError", &u_ex); bind("pv_trk_pvunbiased_yError", &u_ey); bind("pv_trk_pvunbiased_zError", &u_ez);
    // the covariance gives the PV error per track, used for the grouping in both steps
    bind("pv_trk_pvunbiased_covXY", &u_cxy); bind("pv_trk_pvunbiased_covXZ", &u_cxz); bind("pv_trk_pvunbiased_covYZ", &u_cyz);

    // ---------- Event loop ----------
    // Calls pv_action(k, comb, value) for the PV split and
    // ip_action(idx, comb, track index) for every IP histogram a track belongs to.
    // The track angles and the track's projected PV errors (no pull) are set before ip_action is called.
    Double_t s_phi = 0, c_phi = 0, sh_eta = 0, sig_pv[2] = {0, 0};
    Long64_t n_bad_cov = 0;
    auto loop = [&](auto &&pv_action, auto &&ip_action)
    {
        const Long64_t n = chain->GetEntries();
        for (Long64_t ie = 0; ie < n; ++ie)
        {
            if (ie % 1000000 == 0)
                std::cout << "[INFO] event " << ie << " / " << n << std::endl;
            // files are reopened during the loop; a failed reopen must not be skipped silently
            if (chain->LoadTree(ie) < 0 || chain->GetEntry(ie) <= 0)
            {
                std::cerr << "[ERROR] fill_hists: cannot read entry " << ie << std::endl;
                gSystem->Exit(1);
            }

            const Float_t r = std::sqrt(pv_SumTrackPt2);

            // PV resolution: split vertices, binned in sqrt(SumPt2) of the full PV
            const Int_t k = find_bin(pv_edges, r);
            if (k >= 0 && valid1 && !fake1 && valid2 && !fake2 &&
                std::isfinite(ex1) && std::isfinite(ey1) && std::isfinite(ez1) &&
                std::isfinite(ex2) && std::isfinite(ey2) && std::isfinite(ez2))
            {
                if (x1 != null && x2 != null && ex1 != null && ex2 != null)
                {
                    pv_action(k, PV_X, (x1 - x2) / std::sqrt(2.0));
                    pv_action(k, PULL_X, (x1 - x2) / std::sqrt(ex1 * ex1 + ex2 * ex2));
                }
                if (y1 != null && y2 != null && ey1 != null && ey2 != null)
                {
                    pv_action(k, PV_Y, (y1 - y2) / std::sqrt(2.0));
                    pv_action(k, PULL_Y, (y1 - y2) / std::sqrt(ey1 * ey1 + ey2 * ey2));
                }
                if (z1 != null && z2 != null && ez1 != null && ez2 != null)
                {
                    pv_action(k, PV_Z, (z1 - z2) / std::sqrt(2.0));
                    pv_action(k, PULL_Z, (z1 - z2) / std::sqrt(ez1 * ez1 + ez2 * ez2));
                }
            }

            // IP resolution: tracks w.r.t. the unbiased PV
            for (size_t it = 0; it < pt->size(); ++it)
            {
                if (!(*u_valid)[it] || (*u_fake)[it] || (*d0)[it] == null || (*dz)[it] == null ||
                    !std::isfinite((*u_ex)[it]) || !std::isfinite((*u_ey)[it]) || !std::isfinite((*u_ez)[it]))
                    continue;

                const Float_t tpt = (*pt)[it], teta = (*eta)[it], aeta = std::fabs(teta);
                const Int_t i_pt = find_bin(pt_edges, tpt);
                const Int_t i_ptul = find_bin(pt_uleta_edges, tpt);
                const Int_t i_eta = find_bin(eta_edges, teta);
                const Int_t i_phi = find_bin(phi_edges, (*phi)[it]);

                const Bool_t loeta = (aeta < 1.3);
                const Bool_t hieta = (aeta > 1.3 && aeta < 2.5);
                const Bool_t uleta = (aeta > 2.5 && aeta < 3.0);
                const Bool_t alleta = (aeta < 10.0);
                const Bool_t lopt = (tpt > 0.1 && tpt < 1.0);
                const Bool_t hipt = (tpt > 1.0 && tpt < 3.0);
                const Bool_t ulpt = (tpt > 3.0 && tpt < 10.0);
                const Bool_t allpt = (tpt > 0);

                s_phi = std::sin((*phi)[it]);
                c_phi = std::cos((*phi)[it]);
                sh_eta = std::sinh(teta);
                {
                    const Double_t cxx = (*u_ex)[it] * (*u_ex)[it], cyy = (*u_ey)[it] * (*u_ey)[it], czz = (*u_ez)[it] * (*u_ez)[it];
                    const Double_t cxy = (*u_cxy)[it], cxz = (*u_cxz)[it], cyz = (*u_cyz)[it];
                    const Double_t a = c_phi * sh_eta, b = s_phi * sh_eta;
                    const Double_t v_d0 = s_phi * s_phi * cxx + c_phi * c_phi * cyy - 2 * s_phi * c_phi * cxy;
                    const Double_t v_dz = a * a * cxx + b * b * cyy + czz + 2 * a * b * cxy - 2 * a * cxz - 2 * b * cyz;
                    // a covariance that is not positive along d0/dz is a broken fit: drop the track everywhere
                    if (!(v_d0 > 0) || !(v_dz > 0) || !std::isfinite(v_d0) || !std::isfinite(v_dz))
                    {
                        n_bad_cov++;
                        continue;
                    }
                    sig_pv[0] = std::sqrt(v_d0);
                    sig_pv[1] = std::sqrt(v_dz);
                }

                if (i_pt >= 0 && loeta) ip_action(i_pt, PT_LOETA, it);
                if (i_pt >= 0 && hieta) ip_action(i_pt, PT_HIETA, it);
                if (i_ptul >= 0 && uleta) ip_action(i_ptul, PT_ULETA, it);
                if (i_pt >= 0 && alleta) ip_action(i_pt, PT_ALLETA, it);

                if (i_eta >= 0 && lopt) ip_action(i_eta, ETA_LOPT, it);
                if (i_eta >= 0 && hipt) ip_action(i_eta, ETA_HIPT, it);
                if (i_eta >= 0 && ulpt) ip_action(i_eta, ETA_ULPT, it);
                if (i_eta >= 0 && allpt) ip_action(i_eta, ETA_ALLPT, it);

                if (i_phi >= 0 && lopt) ip_action(i_phi, PHI_LOPT, it);
                if (i_phi >= 0 && hipt) ip_action(i_phi, PHI_HIPT, it);
                if (i_phi >= 0 && ulpt) ip_action(i_phi, PHI_ULPT, it);
                if (i_phi >= 0 && allpt) ip_action(i_phi, PHI_ALLPT, it);
            }
        }
    };

    // ---------- Step "range": sums for the histogram ranges ----------
    // Stored as n / sum / sum2 per histogram in TH1Ds, so hadd of all chunks gives the
    // same mean and RMS as a single pass over all files.
    std::vector<std::vector<Stat>> st_pv(n_pv, std::vector<Stat>(N_PV));
    std::vector<std::vector<Stat>> st_d0(n_ip, std::vector<Stat>(N_IP)), st_dz(n_ip, std::vector<Stat>(N_IP));
    const char *ST_NAME[3] = {"st_pv", "st_d0", "st_dz"};
    auto st_size = [&](Int_t which) { return which == 0 ? n_pv * N_PV : n_ip * N_IP; };
    auto st_at = [&](Int_t which, Int_t i) -> Stat &
    {
        return which == 0 ? st_pv[i / N_PV][i % N_PV] : (which == 1 ? st_d0 : st_dz)[i / N_IP][i % N_IP];
    };

    const char *VAR[2] = {"d0", "dz"};
    if (step == "range")
    {
        // projected PV error distribution of every IP histogram (x: idx * N_IP + comb), for the group edges
        TH1::AddDirectory(kFALSE);
        TH2F *h_sig[2];
        for (Int_t v = 0; v < 2; ++v)
            h_sig[v] = new TH2F(Form("sig_%s", VAR[v]), ";IP histogram (idx*N_IP+comb);PV error [um]",
                                n_ip * N_IP, 0, n_ip * N_IP, SIG_NBINS[v], 0, SIG_MAX[v]);
        loop([&](Int_t k, Int_t c, Double_t v)
             { if (v >= pv_init_min[c] && v <= pv_init_max[c]) st_pv[k][c].Fill(v); },
             [&](Int_t i, Int_t c, size_t it)
             {
                 const Float_t vd0 = (*d0)[it], vdz = (*dz)[it];
                 if (vd0 >= init_d0_min[c] && vd0 <= init_d0_max[c]) st_d0[i][c].Fill(vd0);
                 if (vdz >= init_dz_min[c] && vdz <= init_dz_max[c]) st_dz[i][c].Fill(vdz);
                 for (Int_t v = 0; v < 2; ++v)
                     h_sig[v]->Fill(i * N_IP + c + 0.5, sig_pv[v]);
             });
        std::cout << "[INFO] fill_hists: dropped " << n_bad_cov << " tracks with a non-positive projected PV error" << std::endl;

        TString outname = outdir + Form("/range/range_%d.root", chunk);
        TFile *out = TFile::Open(outname, "RECREATE");
        for (Int_t v = 0; v < 2; ++v)
            h_sig[v]->Write();
        for (Int_t w = 0; w < 3; ++w)
        {
            const Int_t n = st_size(w);
            TH1D hn(TString(ST_NAME[w]) + "_n", "", n, 0, n), hs(TString(ST_NAME[w]) + "_sum", "", n, 0, n), hs2(TString(ST_NAME[w]) + "_sum2", "", n, 0, n);
            for (Int_t i = 0; i < n; ++i)
            {
                hn.SetBinContent(i + 1, st_at(w, i).n);
                hs.SetBinContent(i + 1, st_at(w, i).sum);
                hs2.SetBinContent(i + 1, st_at(w, i).sum2);
            }
            hn.Write();
            hs.Write();
            hs2.Write();
        }
        out->Close();
        std::cout << "[INFO] fill_hists: wrote " << outname << std::endl;
        return 0;
    }

    // [var][idx * N_IP + comb] -> group edges, lower and upper included: n groups = size - 1 (empty: no grouping)
    std::vector<std::vector<Double_t>> grp_edges[2];

    // ---------- Step "fill": ranges from the merged range.root ----------
    {
        TFile *rf = TFile::Open(outdir + "/range.root");
        if (!rf || rf->IsZombie())
        {
            std::cerr << "[ERROR] fill_hists: cannot open " << outdir << "/range.root" << std::endl;
            gSystem->Exit(1);
        }
        for (Int_t w = 0; w < 3; ++w)
        {
            TH1D *hn = (TH1D *)rf->Get(TString(ST_NAME[w]) + "_n");
            TH1D *hs = (TH1D *)rf->Get(TString(ST_NAME[w]) + "_sum");
            TH1D *hs2 = (TH1D *)rf->Get(TString(ST_NAME[w]) + "_sum2");
            if (!hn || !hs || !hs2 || hn->GetNbinsX() != st_size(w))
            {
                std::cerr << "[ERROR] fill_hists: range.root does not match the binning" << std::endl;
                gSystem->Exit(1);
            }
            for (Int_t i = 0; i < st_size(w); ++i)
            {
                st_at(w, i).n = Long64_t(hn->GetBinContent(i + 1) + 0.5);
                st_at(w, i).sum = hs->GetBinContent(i + 1);
                st_at(w, i).sum2 = hs2->GetBinContent(i + 1);
            }
        }
        // group edges. d0: the fixed D0_GRP_EDGES. dz: {0, quantiles GRP_Q, GRP_OPEN} of the projected PV
        // error of each IP histogram (linear interpolation inside the 0.2 um bins; overflow counted),
        // empty if not strictly increasing. Both only for histograms with entries.
        for (Int_t v = 0; v < 2; ++v)
        {
            TH2F *hs = (TH2F *)rf->Get(Form("sig_%s", VAR[v]));
            if (!hs || hs->GetNbinsX() != n_ip * N_IP)
            {
                std::cerr << "[ERROR] fill_hists: range.root has no matching sig_" << VAR[v] << std::endl;
                gSystem->Exit(1);
            }
            grp_edges[v].assign(n_ip * N_IP, std::vector<Double_t>());
            for (Int_t x = 0; x < n_ip * N_IP; ++x)
            {
                const Int_t ny = hs->GetNbinsY();
                Double_t tot = 0;
                for (Int_t y = 1; y <= ny + 1; ++y)
                    tot += hs->GetBinContent(x + 1, y);
                if (tot <= 0)
                    continue;
                if (v == 0)
                {
                    grp_edges[v][x] = D0_GRP_EDGES;
                    continue;
                }
                std::vector<Double_t> e;
                Double_t cum = 0;
                Int_t q = 0;
                for (Int_t y = 1; y <= ny && q < N_GRP - 1; ++y)
                {
                    const Double_t c = hs->GetBinContent(x + 1, y);
                    while (q < N_GRP - 1 && cum + c >= GRP_Q[q] * tot && c > 0)
                    {
                        const Double_t frac = (GRP_Q[q] * tot - cum) / c;
                        e.push_back(hs->GetYaxis()->GetBinLowEdge(y) + frac * hs->GetYaxis()->GetBinWidth(y));
                        q++;
                    }
                    cum += c;
                }
                Bool_t ok = ((Int_t)e.size() == N_GRP - 1);
                for (size_t j = 1; ok && j < e.size(); ++j)
                    ok = e[j] > e[j - 1];
                if (ok)
                {
                    e.insert(e.begin(), 0.0);
                    e.push_back(GRP_OPEN);
                    grp_edges[v][x] = e;
                }
            }
        }
        rf->Close();
    }

    // ---------- Book histograms ----------
    const TString outname = outdir + Form("/fill/fill_%d.root", chunk);
    TFile *outfile = TFile::Open(outname, "RECREATE");
    TH1::AddDirectory(kFALSE);

    std::vector<std::vector<TH1F *>> h_pv(n_pv, std::vector<TH1F *>(N_PV, nullptr));
    const TString pv_xtitle[N_PV] = {"(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum]", "(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum]",
                                     "(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [#mum]",
                                     "(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}}",
                                     "(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}}",
                                     "(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}}"};
    for (Int_t k = 0; k < n_pv; ++k)
    {
        TString cut = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_edges[k], pv_edges[k + 1]);
        for (Int_t c = 0; c < N_PV; ++c)
        {
            if (!st_pv[k][c].Valid())
                continue;
            Double_t lo, hi;
            hist_range(st_pv[k][c], pv_init_min[c], pv_init_max[c], lo, hi);
            h_pv[k][c] = new TH1F(Form("pv_%d_%s", k, PV_SUFFIX[c]), cut + ";" + pv_xtitle[c] + ";# PV", nbins, lo, hi);
            h_pv[k][c]->Sumw2();
        }
    }

    std::vector<std::vector<TH1F *>> h_d0(n_ip, std::vector<TH1F *>(N_IP, nullptr)), h_dz = h_d0;
    // PV covariance sums per IP histogram: [k * N_TERM + term]
    std::vector<std::vector<std::vector<Double_t>>> e_d0(n_ip, std::vector<std::vector<Double_t>>(N_IP)), e_dz = e_d0;
    // the same per PV-error group: [var][idx * N_IP + comb][group]
    std::vector<std::vector<TH1F *>> h_grp[2];
    std::vector<std::vector<std::vector<Double_t>>> e_grp[2];
    for (Int_t v = 0; v < 2; ++v)
    {
        // sized per histogram from its edges when booked
        h_grp[v].assign(n_ip * N_IP, std::vector<TH1F *>());
        e_grp[v].assign(n_ip * N_IP, std::vector<std::vector<Double_t>>());
    }
    for (Int_t i = 0; i < n_ip; ++i)
    {
        TString ptcut = Form("%.3f<#it{p_{T}}<%.3f GeV", pt_edges[i], pt_edges[i + 1]);
        TString ptulcut = Form("%.3f<#it{p_{T}}<%.3f GeV", pt_uleta_edges[i], pt_uleta_edges[i + 1]);
        TString etacut = Form("%.2f<#it{#eta}<%.2f", eta_edges[i], eta_edges[i + 1]);
        TString phicut = Form("%.2f<#it{#phi}<%.2f", phi_edges[i], phi_edges[i + 1]);
        auto two = [](TString a, TString b) { return "#splitline{" + a + "}{" + b + "}"; };
        const TString lopt = "0.1<#it{p_{T}}<1 GeV", hipt = "1<#it{p_{T}}<3 GeV", ulpt = "3<#it{p_{T}}<10 GeV";
        const TString loeta = "|#it{#eta}|<1.3", hieta = "1.3<|#it{#eta}|<2.5", uleta = "2.5<|#it{#eta}|<3.0";
        const TString title[N_IP] = {two(ptcut, loeta), two(ptcut, hieta), two(ptulcut, uleta), ptcut,
                                     two(etacut, lopt), two(etacut, hipt), two(etacut, ulpt), etacut,
                                     two(phicut, lopt), two(phicut, hipt), two(phicut, ulpt), phicut};
        for (Int_t c = 0; c < N_IP; ++c)
        {
            // as before: no histogram (fit result 0) unless both d0 and dz have entries
            if (!st_d0[i][c].Valid() || !st_dz[i][c].Valid())
                continue;
            Double_t lo, hi;
            hist_range(st_d0[i][c], init_d0_min[c], init_d0_max[c], lo, hi);
            h_d0[i][c] = new TH1F(Form("ip_d0_%d_%s", i, IP_SUFFIX[c]), title[c] + ";Track IP #it{d_{xy}} [#mum];# tracks", nbins, lo, hi);
            hist_range(st_dz[i][c], init_dz_min[c], init_dz_max[c], lo, hi);
            h_dz[i][c] = new TH1F(Form("ip_dz_%d_%s", i, IP_SUFFIX[c]), title[c] + ";Track IP #it{d_{z}} [#mum];# tracks", nbins, lo, hi);
            h_d0[i][c]->Sumw2();
            h_dz[i][c]->Sumw2();
            e_d0[i][c].assign(n_pv * N_TERM, 0.0);
            e_dz[i][c].assign(n_pv * N_TERM, 0.0);
            // grouped copies: same range as the ungrouped histogram
            for (Int_t v = 0; v < 2; ++v)
            {
                const auto &edges = grp_edges[v][i * N_IP + c];
                if (edges.empty())
                    continue;
                TH1F *base = v == 0 ? h_d0[i][c] : h_dz[i][c];
                const Int_t ng = edges.size() - 1;
                h_grp[v][i * N_IP + c].assign(ng, nullptr);
                e_grp[v][i * N_IP + c].assign(ng, std::vector<Double_t>());
                for (Int_t g = 0; g < ng; ++g)
                {
                    const Double_t glo = edges[g], ghi = edges[g + 1];
                    TString gcut = ghi >= GRP_OPEN ? Form("#sigma_{PV}>%.1f #mum", glo) : Form("%.1f<#sigma_{PV}<%.1f #mum", glo, ghi);
                    TH1F *h = (TH1F *)base->Clone(Form("ip_%s_%d_%s_g%d", VAR[v], i, IP_SUFFIX[c], g));
                    h->SetTitle("#splitline{" + title[c] + "}{" + gcut + "}");
                    h_grp[v][i * N_IP + c][g] = h;
                    e_grp[v][i * N_IP + c][g].assign(n_pv * N_TERM, 0.0);
                }
            }
        }
    }

    // ---------- Fill ----------
    // the unbiased-PV SumPt2 (pull lookup) is only needed in the fill step. Enable the branch before
    // setting its address: an address set on a disabled branch is lost if the chain never switches
    // trees (one-file chunk), leaving u_spt2 null
    bind("pv_trk_pvunbiased_SumTrackPt2", &u_spt2);

    // adds the unbiased-PV covariance of track `it`, projected with Jacobian J, to `e` in the bin k of
    // sqrt(SumPt2) of the track's unbiased PV (clamped to the binning range) -- the pull applied later
    // (J for d0: (sin phi, -cos phi, 0); for dz: (cos phi sinh eta, sin phi sinh eta, -1))
    auto add_cov = [&](std::vector<Double_t> &e, Double_t jx, Double_t jy, Double_t jz, size_t it)
    {
        const Double_t cxx = (*u_ex)[it] * (*u_ex)[it], cyy = (*u_ey)[it] * (*u_ey)[it], czz = (*u_ez)[it] * (*u_ez)[it];
        const Float_t r = std::sqrt((*u_spt2)[it]);
        const Int_t k = r <= pv_edges.front() ? 0
                      : r >= pv_edges.back() ? n_pv - 1
                      : Int_t(std::upper_bound(pv_edges.begin(), pv_edges.end(), r) - pv_edges.begin()) - 1;
        Double_t *s = &e[k * N_TERM];
        s[XX] += jx * jx * cxx;
        s[YY] += jy * jy * cyy;
        s[ZZ] += jz * jz * czz;
        s[XY] += 2 * jx * jy * (*u_cxy)[it];
        s[XZ] += 2 * jx * jz * (*u_cxz)[it];
        s[YZ] += 2 * jy * jz * (*u_cyz)[it];
        s[NTRK] += 1.0;
    };

    loop([&](Int_t k, Int_t c, Double_t v)
         { if (h_pv[k][c]) h_pv[k][c]->Fill(v); },
         [&](Int_t i, Int_t c, size_t it)
         {
             TH1F *hd0 = h_d0[i][c], *hdz = h_dz[i][c];
             if (!hd0)
                 return;
             const Float_t vd0 = (*d0)[it], vdz = (*dz)[it];
             hd0->Fill(vd0);
             hdz->Fill(vdz);
             // PV error sums only over the tracks inside the fitted range
             const Bool_t in_d0 = vd0 >= hd0->GetXaxis()->GetXmin() && vd0 < hd0->GetXaxis()->GetXmax();
             const Bool_t in_dz = vdz >= hdz->GetXaxis()->GetXmin() && vdz < hdz->GetXaxis()->GetXmax();
             if (in_d0)
                 add_cov(e_d0[i][c], s_phi, -c_phi, 0.0, it);
             if (in_dz)
                 add_cov(e_dz[i][c], c_phi * sh_eta, s_phi * sh_eta, -1.0, it);
             // the same in the track's PV-error group
             const Float_t val[2] = {vd0, vdz};
             const Bool_t in[2] = {in_d0, in_dz};
             const Double_t jac[2][3] = {{s_phi, -c_phi, 0.0}, {c_phi * sh_eta, s_phi * sh_eta, -1.0}};
             for (Int_t v = 0; v < 2; ++v)
             {
                 const auto &edges = grp_edges[v][i * N_IP + c];
                 if (edges.empty())
                     continue;
                 // group [edge_g, edge_g+1); none outside the edges (d0: outside the central 90 %)
                 const Int_t g = Int_t(std::upper_bound(edges.begin(), edges.end(), sig_pv[v]) - edges.begin()) - 1;
                 if (g < 0 || g >= (Int_t)edges.size() - 1)
                     continue;
                 h_grp[v][i * N_IP + c][g]->Fill(val[v]);
                 if (in[v])
                     add_cov(e_grp[v][i * N_IP + c][g], jac[v][0], jac[v][1], jac[v][2], it);
             }
         });
    std::cout << "[INFO] fill_hists: done filling; dropped " << n_bad_cov
              << " tracks with a non-positive projected PV error" << std::endl;

    // ---------- Write ----------
    auto write_sums = [&](const std::vector<Double_t> &e, TString name)
    {
        TH2D h(name, ";sqrt(SumPt2) bin;term (xx yy zz xy xz yz ntrk)", n_pv, 0, n_pv, N_TERM, 0, N_TERM);
        for (Int_t k = 0; k < n_pv; ++k)
            for (Int_t t = 0; t < N_TERM; ++t)
                h.SetBinContent(k + 1, t + 1, e[k * N_TERM + t]);
        h.Write();
    };
    outfile->cd();
    for (auto &v : h_pv) for (auto *h : v) if (h) h->Write();
    for (Int_t i = 0; i < n_ip; ++i)
        for (Int_t c = 0; c < N_IP; ++c)
            if (h_d0[i][c])
            {
                h_d0[i][c]->Write();
                h_dz[i][c]->Write();
                write_sums(e_d0[i][c], Form("pverr_d0_%d_%s", i, IP_SUFFIX[c]));
                write_sums(e_dz[i][c], Form("pverr_dz_%d_%s", i, IP_SUFFIX[c]));
                for (Int_t v = 0; v < 2; ++v)
                {
                    const auto &edges = grp_edges[v][i * N_IP + c];
                    if (edges.empty())
                        continue;
                    // the edges are the same in every chunk: write them once, or hadd would add them up
                    if (chunk == 0)
                    {
                        TH1D he(Form("grpedges_%s_%d_%s", VAR[v], i, IP_SUFFIX[c]),
                                "PV error group edges [um] (no pull), lower and upper included", edges.size(), 0, edges.size());
                        for (size_t j = 0; j < edges.size(); ++j)
                            he.SetBinContent(j + 1, edges[j]);
                        he.Write();
                    }
                    for (Int_t g = 0; g < (Int_t)edges.size() - 1; ++g)
                    {
                        h_grp[v][i * N_IP + c][g]->Write();
                        write_sums(e_grp[v][i * N_IP + c][g], Form("pverr_%s_%d_%s_g%d", VAR[v], i, IP_SUFFIX[c], g));
                    }
                }
            }
    outfile->Close();
    std::cout << "[INFO] fill_hists: wrote " << outname << std::endl;
    return 0;
}
