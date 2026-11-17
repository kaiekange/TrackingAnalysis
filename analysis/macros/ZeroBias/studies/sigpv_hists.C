// sigma_PV (no pull) of the probe tracks vs pT (per |eta| slice) and vs eta (per pT range), d0 and dz.
// Same track selection and Jacobians as JetHT/fill_hists.cc, wider range: 0.1 < pT < 15 GeV, |eta| < 3.
// Usage (cmsenv + proxy): root -b -q 'sigpv_hists.C+("<file list>", "<out.root>")'
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <TSystem.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

void sigpv_hists(const char *list, const char *out)
{
    TChain *chain = new TChain("residuals/tree");
    std::ifstream in(list);
    std::string f;
    while (in >> f)
        chain->Add(f.c_str());
    // fail loud: every file must be readable and non-empty
    chain->GetEntries();
    for (Int_t i = 0; i < chain->GetNtrees(); ++i)
    {
        const Long64_t n = (i + 1 < chain->GetNtrees() ? chain->GetTreeOffset()[i + 1] : chain->GetEntries()) - chain->GetTreeOffset()[i];
        if (n <= 0)
        {
            std::cerr << "[ERROR] tree " << i << " has no entries" << std::endl;
            gSystem->Exit(1);
        }
    }

    std::vector<Float_t> *pt = 0, *eta = 0, *phi = 0, *d0 = 0, *dz = 0, *ex = 0, *ey = 0, *ez = 0, *cxy = 0, *cxz = 0, *cyz = 0;
    std::vector<Bool_t> *valid = 0, *fake = 0;
    chain->SetBranchStatus("*", 0);
    auto bind = [&](const char *b, auto p) { chain->SetBranchStatus(b, 1); chain->SetBranchAddress(b, p); };
    bind("pv_trk_pt", &pt); bind("pv_trk_eta", &eta); bind("pv_trk_phi", &phi);
    bind("pv_trk_d0_pvunbiased", &d0); bind("pv_trk_dz_pvunbiased", &dz);
    bind("pv_trk_pvunbiased_IsValid", &valid); bind("pv_trk_pvunbiased_IsFake", &fake);
    bind("pv_trk_pvunbiased_xError", &ex); bind("pv_trk_pvunbiased_yError", &ey); bind("pv_trk_pvunbiased_zError", &ez);
    bind("pv_trk_pvunbiased_covXY", &cxy); bind("pv_trk_pvunbiased_covXZ", &cxz); bind("pv_trk_pvunbiased_covYZ", &cyz);
    const Float_t null = -777;

    // sigma_PV: log bins 0.1 um - 10 cm (as the range step of fill_hists.cc)
    const Int_t NS = 1500;
    std::vector<Double_t> sb(NS + 1);
    for (Int_t i = 0; i <= NS; ++i)
        sb[i] = 0.1 * std::pow(1e6, Double_t(i) / NS);
    const std::vector<Double_t> slices = {0.0, 0.8, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5, 3.0};
    const std::vector<Double_t> ptr = {0.1, 1.0, 3.0, 10.0, 15.0};
    const char *V[2] = {"d0", "dz"};
    std::vector<TH2D *> h_pt[2], h_eta[2];
    for (Int_t v = 0; v < 2; ++v)
    {
        for (size_t s = 0; s + 1 < slices.size(); ++s)
            h_pt[v].push_back(new TH2D(Form("sig_%s_pt_s%zu", V[v], s), Form("%.1f<|eta|<%.1f", slices[s], slices[s + 1]),
                                       149, 0.1, 15.0, NS, sb.data()));
        for (size_t p = 0; p + 1 < ptr.size(); ++p)
            h_eta[v].push_back(new TH2D(Form("sig_%s_eta_p%zu", V[v], p), Form("%.1f<pT<%.1f", ptr[p], ptr[p + 1]),
                                        120, -3.0, 3.0, NS, sb.data()));
    }

    Long64_t n_trk = 0, n_bad = 0;
    const Long64_t n = chain->GetEntries();
    for (Long64_t ie = 0; ie < n; ++ie)
    {
        if (chain->LoadTree(ie) < 0 || chain->GetEntry(ie) <= 0)
        {
            std::cerr << "[ERROR] cannot read entry " << ie << std::endl;
            gSystem->Exit(1);
        }
        for (size_t it = 0; it < pt->size(); ++it)
        {
            if (!(*valid)[it] || (*fake)[it] || (*d0)[it] == null || (*dz)[it] == null ||
                !std::isfinite((*ex)[it]) || !std::isfinite((*ey)[it]) || !std::isfinite((*ez)[it]))
                continue;
            const Double_t tpt = (*pt)[it], teta = (*eta)[it], aeta = std::fabs(teta);
            if (!(tpt > 0.1 && tpt < 15.0 && aeta < 3.0))
                continue;
            const Double_t s = std::sin((*phi)[it]), c = std::cos((*phi)[it]), sh = std::sinh(teta);
            const Double_t xx = (*ex)[it] * (*ex)[it], yy = (*ey)[it] * (*ey)[it], zz = (*ez)[it] * (*ez)[it];
            const Double_t a = c * sh, b = s * sh;
            const Double_t var[2] = {s * s * xx + c * c * yy - 2 * s * c * (*cxy)[it],
                                     a * a * xx + b * b * yy + zz + 2 * a * b * (*cxy)[it] - 2 * a * (*cxz)[it] - 2 * b * (*cyz)[it]};
            if (!(var[0] > 0) || !(var[1] > 0) || !std::isfinite(var[0]) || !std::isfinite(var[1]))
            {
                n_bad++;
                continue;
            }
            n_trk++;
            const Int_t is = Int_t(std::upper_bound(slices.begin(), slices.end(), aeta) - slices.begin()) - 1;
            const Int_t ip = Int_t(std::upper_bound(ptr.begin(), ptr.end(), tpt) - ptr.begin()) - 1;
            for (Int_t v = 0; v < 2; ++v)
            {
                const Double_t sig = std::sqrt(var[v]);
                h_pt[v][is]->Fill(tpt, sig);
                h_eta[v][ip]->Fill(teta, sig);
            }
        }
    }
    std::cout << "[INFO] " << chain->GetNtrees() << " files, " << n << " events, " << n_trk << " tracks, "
              << n_bad << " dropped (non-positive covariance)" << std::endl;

    TFile fo(out, "RECREATE");
    for (Int_t v = 0; v < 2; ++v)
    {
        for (auto h : h_pt[v]) h->Write();
        for (auto h : h_eta[v]) h->Write();
    }
    fo.Close();
}
