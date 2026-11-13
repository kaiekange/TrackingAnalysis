// Steps 1 and 2 of the PV/IP resolution measurement (step 3 = pv_res.cc, then ip_res.cc).
// Each condor job handles files_per_job files of <storage_dir>/hists/<period>/files.txt
// (written by fill_hists.py):
//
//   fill_hists.cc+("2022_22Sep", "range", <chunk>, 10)  -> hists/<period>/range/range_<chunk>.root
//       n / sum / sum2 of every histogram variable inside its initial window, and the projected PV error
//       (no pull) of every IP histogram's tracks (log bins); hadd -> range.root
//   fill_hists.cc+("2022_22Sep", "fill", <chunk>, 10)   -> hists/<period>/fill/fill_<chunk>.root
//       histograms with ranges mean +- 8 RMS from range.root; hadd -> fill.root
//
// Probe tracks: 1 < pT < 15 GeV, |eta| < 2.5 (pt/eta/phi binnings of binning.json). In every IP histogram,
// separately for d0 and dz, the 1 % of tracks with the largest PV error (no pull; 99 % quantile of the
// histogram's distribution in range.root) are dropped from the histogram, the PV error sums and the groups.
// This removes the rare outliers from poorly constrained, low-multiplicity PVs (sigma_PV up to mm; < 0.1 % per
// histogram in 2022C) with a robust cut. A track dropped in dz is still used in d0.
//
// fill.root contains
//   pv_<k>_<suffix>             PV split histograms, k = sqrt(SumPt2) bin
//   ip_<d0|dz>_<idx>_<suffix>   IP histograms, idx = pt/eta/phi bin
//   pverr_<d0|dz>_<idx>_<suffix> per IP histogram: sum over its tracks of the unbiased-PV covariance
//                               projected on d0/dz, split by the sqrt(SumPt2) bin k of each track's
//                               unbiased PV (x axis; the pull lookup) and
//                               covariance term (y bins: xx yy zz xy xz yz, 7 = track count).
//                               ip_res.cc applies the PV pulls to these sums.
//   sigclip_<d0|dz>             PV-error cut per IP histogram (x: idx * N_IP + comb) [um], written by chunk 0
//   nclip_<d0|dz>, nall_<d0|dz> tracks dropped by that cut / all tracks, per IP histogram (summed by hadd)

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

    // vs pT: one category per |eta| slice (pv_trk_abseta_slices of binning.json, N_SLICE slices) + all |eta| < 2.5;
    // vs eta and vs phi: 1 < pT < 3 GeV (hipt) and 3 < pT < 10 GeV (ulpt)
    const Int_t N_SLICE = 9;
    enum IpComb { PT_SLICE0 = 0, PT_ALLETA = N_SLICE, ETA_HIPT, ETA_ULPT, PHI_HIPT, PHI_ULPT, N_IP };
    const char *IP_SUFFIX[N_IP] = {"pt_eta0", "pt_eta1", "pt_eta2", "pt_eta3", "pt_eta4", "pt_eta5", "pt_eta6",
                                   "pt_eta7", "pt_eta8", "pt_alleta", "eta_hipt", "eta_ulpt", "phi_hipt", "phi_ulpt"};
    const Double_t init_d0_min[N_IP] = {-2000, -2000, -2000, -2000, -2000, -2000, -2000, -2000, -2000, -2000, -1000, -400, -500, -400};
    const Double_t init_d0_max[N_IP] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000, 1000, 400, 500, 400};
    const Double_t init_dz_min[N_IP] = {-4000, -4000, -4000, -4000, -4000, -4000, -4000, -4000, -4000, -4000, -4000, -2000, -1500, -600};
    const Double_t init_dz_max[N_IP] = {4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 2000, 1500, 600};

    enum CovTerm { XX = 0, YY, ZZ, XY, XZ, YZ, NTRK, N_TERM };

    // Groups in the track's own PV error projected on d0/dz, with the PV pulls (pv_res jsons of the period,
    // looked up in sqrt(SumPt2) of the track's unbiased PV, as in ip_res.cc). Each IP histogram is also split
    // into groups [edge_j, edge_j+1) with edge_j+1 / edge_j = GRP_RATIO (sigma_PV nearly the same inside a group,
    // as for the |eta| slices), the same edges in every histogram. They span the 0.1 % - 99.99 % quantiles of
    // the 2022C distribution (1-10 GeV, no pull: d0 2.2-20 um, dz 3.7-86 um); tracks outside are in no group.
    // Groups with too few tracks are not fitted (ip_res.cc).
    const Double_t GRP_RATIO = 1.15;
    const Double_t GRP_FIRST[2] = {2.2, 3.6};  // d0, dz [um]
    const Int_t GRP_N[2] = {16, 24};           // up to 20.2 / 103 um
    std::vector<Double_t> grp_edges_of(Int_t v)
    {
        std::vector<Double_t> e(1, GRP_FIRST[v]);
        for (Int_t g = 0; g < GRP_N[v]; ++g)
            e.push_back(e.back() * GRP_RATIO);
        return e;
    }

    // PV-error distribution per IP histogram (range step): log bins from SIG_MIN to SIG_MAX um;
    // overflow (> 10 cm) is always cut
    const Int_t SIG_NBINS = 1500;
    const Double_t SIG_MIN = 0.1, SIG_MAX = 1e5;
    const Double_t CLIP_QUANTILE = 0.99;  // tracks above this quantile of sigma_PV are dropped

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

    // PV-error cut of one IP histogram: the CLIP_QUANTILE quantile of its sigma_PV distribution (column xb of
    // the range-step TH2D, under- and overflow counted; log-interpolated inside the log bin).
    // SIG_MAX if the histogram has no tracks.
    Double_t clip_threshold(const TH2D *h, Int_t xb)
    {
        const Int_t ny = h->GetNbinsY();
        Double_t n = 0;
        for (Int_t j = 0; j <= ny + 1; ++j)
            n += h->GetBinContent(xb, j);
        if (n <= 0)
            return SIG_MAX;
        Double_t cum = h->GetBinContent(xb, 0);
        for (Int_t j = 1; j <= ny; ++j)
        {
            const Double_t w = h->GetBinContent(xb, j);
            if (w > 0 && cum + w >= CLIP_QUANTILE * n)
            {
                const Double_t lo = h->GetYaxis()->GetBinLowEdge(j), hi = h->GetYaxis()->GetBinUpEdge(j);
                return lo * std::pow(hi / lo, (CLIP_QUANTILE * n - cum) / w);
            }
            cum += w;
        }
        return SIG_MAX;
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
    const auto eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    const auto phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();
    const auto slice_edges = binning["pv_trk_abseta_slices"].get<std::vector<Float_t>>();
    const Int_t n_pv = pv_edges.size() - 1;
    const Int_t n_ip = pt_edges.size() - 1;
    if ((Int_t)eta_edges.size() - 1 != n_ip || (Int_t)phi_edges.size() - 1 != n_ip)
    {
        std::cerr << "[ERROR] fill_hists: pt/eta/phi binnings must have the same number of bins" << std::endl;
        gSystem->Exit(1);
    }
    if ((Int_t)slice_edges.size() - 1 != N_SLICE)
    {
        std::cerr << "[ERROR] fill_hists: binning.json has " << slice_edges.size() - 1 << " |eta| slices, the code " << N_SLICE << std::endl;
        gSystem->Exit(1);
    }
    const Float_t eta_max = slice_edges.back();

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
                // probe tracks: |eta| < 2.5 (pT: the pt binning, 1-15 GeV, and the hipt/ulpt ranges)
                if (!(aeta < eta_max))
                    continue;
                const Int_t i_pt = find_bin(pt_edges, tpt);
                const Int_t i_eta = find_bin(eta_edges, teta);
                const Int_t i_phi = find_bin(phi_edges, (*phi)[it]);
                // |eta| slice: [edge_s, edge_s+1)
                const Int_t i_slice = Int_t(std::upper_bound(slice_edges.begin(), slice_edges.end(), aeta) - slice_edges.begin()) - 1;

                const Bool_t hipt = (tpt > 1.0 && tpt < 3.0);
                const Bool_t ulpt = (tpt > 3.0 && tpt < 10.0);

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

                if (i_pt >= 0 && i_slice >= 0 && i_slice < N_SLICE) ip_action(i_pt, PT_SLICE0 + i_slice, it);
                if (i_pt >= 0) ip_action(i_pt, PT_ALLETA, it);

                if (i_eta >= 0 && hipt) ip_action(i_eta, ETA_HIPT, it);
                if (i_eta >= 0 && ulpt) ip_action(i_eta, ETA_ULPT, it);

                if (i_phi >= 0 && hipt) ip_action(i_phi, PHI_HIPT, it);
                if (i_phi >= 0 && ulpt) ip_action(i_phi, PHI_ULPT, it);
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
        // projected PV error distribution (no pull) of every IP histogram (x: idx * N_IP + comb), log bins,
        // for the PV-error cut of the fill step
        TH1::AddDirectory(kFALSE);
        std::vector<Double_t> sig_bins(SIG_NBINS + 1);
        for (Int_t j = 0; j <= SIG_NBINS; ++j)
            sig_bins[j] = SIG_MIN * std::pow(SIG_MAX / SIG_MIN, Double_t(j) / SIG_NBINS);
        std::vector<Double_t> x_bins(n_ip * N_IP + 1);
        for (Int_t j = 0; j <= n_ip * N_IP; ++j)
            x_bins[j] = j;
        TH2D *h_sig[2];
        for (Int_t v = 0; v < 2; ++v)
            h_sig[v] = new TH2D(Form("sig_%s", VAR[v]), ";IP histogram (idx*N_IP+comb);PV error (no pull) [um]",
                                n_ip * N_IP, x_bins.data(), SIG_NBINS, sig_bins.data());
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
    // [var][idx * N_IP + comb] -> PV-error cut [um] (no pull); tracks dropped by it / all tracks
    std::vector<Double_t> sig_clip[2];
    std::vector<Double_t> n_clip[2], n_all[2];

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
        // group edges: the same geometric edges everywhere (booked only for histograms with entries)
        for (Int_t v = 0; v < 2; ++v)
            grp_edges[v].assign(n_ip * N_IP, grp_edges_of(v));
        // PV-error cut per IP histogram
        for (Int_t v = 0; v < 2; ++v)
        {
            TH2D *hs = (TH2D *)rf->Get(Form("sig_%s", VAR[v]));
            if (!hs || hs->GetNbinsX() != n_ip * N_IP || hs->GetNbinsY() != SIG_NBINS)
            {
                std::cerr << "[ERROR] fill_hists: sig_" << VAR[v] << " of range.root does not match" << std::endl;
                gSystem->Exit(1);
            }
            sig_clip[v].resize(n_ip * N_IP);
            for (Int_t i = 0; i < n_ip * N_IP; ++i)
                sig_clip[v][i] = clip_threshold(hs, i + 1);
        }
        rf->Close();
    }

    // PV pulls per sqrt(SumPt2) bin, for the grouping (pv_res.cc output of this period, as in ip_res.cc)
    std::vector<Double_t> pull_x(n_pv), pull_y(n_pv), pull_z(n_pv);
    for (Int_t k = 0; k < n_pv; ++k)
    {
        std::ifstream pvfile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", k));
        if (!pvfile)
        {
            std::cerr << "[ERROR] fill_hists: missing PV result " << storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", k)
                      << " (the grouping needs the pulls: run pv_res first)" << std::endl;
            gSystem->Exit(1);
        }
        nlohmann::json pv;
        pvfile >> pv;
        pull_x[k] = pv["reso_data_pullx"];
        pull_y[k] = pv["reso_data_pully"];
        pull_z[k] = pv["reso_data_pullz"];
        for (Double_t p : {pull_x[k], pull_y[k], pull_z[k]})
            if (p < 0.5 || p > 2.0)
            {
                std::cerr << "[ERROR] fill_hists: implausible PV pull " << p << " in sqrt(SumPt2) bin " << k << std::endl;
                gSystem->Exit(1);
            }
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
        TString etacut = Form("%.2f<#it{#eta}<%.2f", eta_edges[i], eta_edges[i + 1]);
        TString phicut = Form("%.2f<#it{#phi}<%.2f", phi_edges[i], phi_edges[i + 1]);
        auto two = [](TString a, TString b) { return "#splitline{" + a + "}{" + b + "}"; };
        const TString hipt = "1<#it{p_{T}}<3 GeV", ulpt = "3<#it{p_{T}}<10 GeV";
        TString title[N_IP];
        for (Int_t s = 0; s < N_SLICE; ++s)
            title[PT_SLICE0 + s] = two(ptcut, Form("%.1f<|#it{#eta}|<%.1f", slice_edges[s], slice_edges[s + 1]));
        title[PT_ALLETA] = two(ptcut, Form("|#it{#eta}|<%.1f", eta_max));
        title[ETA_HIPT] = two(etacut, hipt);
        title[ETA_ULPT] = two(etacut, ulpt);
        title[PHI_HIPT] = two(phicut, two(hipt, Form("|#it{#eta}|<%.1f", eta_max)));
        title[PHI_ULPT] = two(phicut, two(ulpt, Form("|#it{#eta}|<%.1f", eta_max)));
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
                    TString gcut = Form("%.1f<#sigma_{PV}<%.1f #mum", glo, ghi);
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
    auto pv_bin = [&](size_t it)
    {
        const Float_t r = std::sqrt((*u_spt2)[it]);
        return r <= pv_edges.front() ? 0
             : r >= pv_edges.back() ? n_pv - 1
             : Int_t(std::upper_bound(pv_edges.begin(), pv_edges.end(), r) - pv_edges.begin()) - 1;
    };
    auto add_cov = [&](std::vector<Double_t> &e, Double_t jx, Double_t jy, Double_t jz, size_t it)
    {
        const Double_t cxx = (*u_ex)[it] * (*u_ex)[it], cyy = (*u_ey)[it] * (*u_ey)[it], czz = (*u_ez)[it] * (*u_ez)[it];
        const Int_t k = pv_bin(it);
        Double_t *s = &e[k * N_TERM];
        s[XX] += jx * jx * cxx;
        s[YY] += jy * jy * cyy;
        s[ZZ] += jz * jz * czz;
        s[XY] += 2 * jx * jy * (*u_cxy)[it];
        s[XZ] += 2 * jx * jz * (*u_cxz)[it];
        s[YZ] += 2 * jy * jz * (*u_cyz)[it];
        s[NTRK] += 1.0;
    };
    // the track's PV error projected with Jacobian J, with the pulls of its unbiased PV (C_ij -> p_i p_j C_ij);
    // -1 if not positive (then in no group)
    auto sig_pull = [&](Double_t jx, Double_t jy, Double_t jz, size_t it)
    {
        const Int_t k = pv_bin(it);
        const Double_t ax = jx * pull_x[k], ay = jy * pull_y[k], az = jz * pull_z[k];
        const Double_t w = ax * ax * (*u_ex)[it] * (*u_ex)[it] + ay * ay * (*u_ey)[it] * (*u_ey)[it] +
                           az * az * (*u_ez)[it] * (*u_ez)[it] +
                           2 * ax * ay * (*u_cxy)[it] + 2 * ax * az * (*u_cxz)[it] + 2 * ay * az * (*u_cyz)[it];
        return w > 0 ? std::sqrt(w) : -1.0;
    };

    for (Int_t v = 0; v < 2; ++v)
    {
        n_clip[v].assign(n_ip * N_IP, 0.0);
        n_all[v].assign(n_ip * N_IP, 0.0);
    }
    loop([&](Int_t k, Int_t c, Double_t v)
         { if (h_pv[k][c]) h_pv[k][c]->Fill(v); },
         [&](Int_t i, Int_t c, size_t it)
         {
             if (!h_d0[i][c])
                 return;
             TH1F *hist[2] = {h_d0[i][c], h_dz[i][c]};
             std::vector<Double_t> *esum[2] = {&e_d0[i][c], &e_dz[i][c]};
             const Float_t val[2] = {(*d0)[it], (*dz)[it]};
             const Double_t jac[2][3] = {{s_phi, -c_phi, 0.0}, {c_phi * sh_eta, s_phi * sh_eta, -1.0}};
             for (Int_t v = 0; v < 2; ++v)
             {
                 // outlying PV error (no pull): dropped from this variable's histogram, sums and groups
                 n_all[v][i * N_IP + c] += 1;
                 if (!(sig_pv[v] <= sig_clip[v][i * N_IP + c]))
                 {
                     n_clip[v][i * N_IP + c] += 1;
                     continue;
                 }
                 hist[v]->Fill(val[v]);
                 // PV error sums only over the tracks inside the fitted range
                 const Bool_t in = val[v] >= hist[v]->GetXaxis()->GetXmin() && val[v] < hist[v]->GetXaxis()->GetXmax();
                 if (in)
                     add_cov(*esum[v], jac[v][0], jac[v][1], jac[v][2], it);
                 // the same in the track's PV-error group
                 const auto &edges = grp_edges[v][i * N_IP + c];
                 if (edges.empty())
                     continue;
                 // group [edge_g, edge_g+1) in the PV error with pull; none outside the edges
                 const Double_t sp = sig_pull(jac[v][0], jac[v][1], jac[v][2], it);
                 const Int_t g = Int_t(std::upper_bound(edges.begin(), edges.end(), sp) - edges.begin()) - 1;
                 if (g < 0 || g >= (Int_t)edges.size() - 1)
                     continue;
                 h_grp[v][i * N_IP + c][g]->Fill(val[v]);
                 if (in)
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
    for (Int_t v = 0; v < 2; ++v)
    {
        Double_t nc = 0, na = 0;
        TH1D hc(Form("nclip_%s", VAR[v]), "tracks dropped by the PV-error cut;idx*N_IP+comb", n_ip * N_IP, 0, n_ip * N_IP);
        TH1D ha(Form("nall_%s", VAR[v]), "all tracks;idx*N_IP+comb", n_ip * N_IP, 0, n_ip * N_IP);
        for (Int_t j = 0; j < n_ip * N_IP; ++j)
        {
            hc.SetBinContent(j + 1, n_clip[v][j]);
            ha.SetBinContent(j + 1, n_all[v][j]);
            nc += n_clip[v][j];
            na += n_all[v][j];
        }
        hc.Write();
        ha.Write();
        std::cout << "[INFO] fill_hists: PV-error cut " << VAR[v] << ": dropped " << nc << " of " << na << " track entries" << std::endl;
        // the cuts are the same in every chunk: write them once, or hadd would add them up
        if (chunk == 0)
        {
            TH1D ht(Form("sigclip_%s", VAR[v]), "PV-error cut (no pull) [um];idx*N_IP+comb", n_ip * N_IP, 0, n_ip * N_IP);
            for (Int_t j = 0; j < n_ip * N_IP; ++j)
                ht.SetBinContent(j + 1, sig_clip[v][j]);
            ht.Write();
        }
    }
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
                                "PV error group edges [um] (with pull), lower and upper included", edges.size(), 0, edges.size());
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
