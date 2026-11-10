// |eta| composition of the sigma_PV,dz groups (no pull) in pT ranges, for all eta and |eta|<1.3,
// and the fraction of tracks from PVs with < 20 tracks. Run like outliers.C (cmsenv + IIHE proxy):
//   root -l -b -q 'groups_eta.C+("root://.../output_1.root,root://.../output_10.root")'
#include <TChain.h>
#include <TSystem.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

void groups_eta(const char *files)
{
    TChain c("residuals/tree");
    TObjArray *a = TString(files).Tokenize(",");
    for (int i = 0; i < a->GetEntries(); ++i)
        if (c.Add(((TObjString *)a->At(i))->GetString(), -1) != 1) { printf("ADD FAILED\n"); gSystem->Exit(1); }
    Int_t ntrk;
    std::vector<Float_t> *pt = 0, *eta = 0, *phi = 0, *ex = 0, *ey = 0, *ez = 0, *cxy = 0, *cxz = 0, *cyz = 0, *d0 = 0;
    std::vector<Bool_t> *uv = 0, *uf = 0;
    c.SetBranchStatus("*", 0);
    auto B = [&](const char *n, void *p) { c.SetBranchStatus(n, 1); c.SetBranchAddress(n, p); };
    B("pv_NTracks", &ntrk); B("pv_trk_pt", &pt); B("pv_trk_eta", &eta); B("pv_trk_phi", &phi);
    B("pv_trk_pvunbiased_xError", &ex); B("pv_trk_pvunbiased_yError", &ey); B("pv_trk_pvunbiased_zError", &ez);
    B("pv_trk_pvunbiased_covXY", &cxy); B("pv_trk_pvunbiased_covXZ", &cxz); B("pv_trk_pvunbiased_covYZ", &cyz);
    B("pv_trk_pvunbiased_IsValid", &uv); B("pv_trk_pvunbiased_IsFake", &uf); B("pv_trk_d0_pvunbiased", &d0);

    const double E[8] = {0, 5.0, 6.5, 8.0, 10, 13, 18, 27};   // group g = [E[g], E[g+1]), g=1..6 as in fill_hists
    const double PT[4] = {0.1, 1, 3, 10};
    // [etaSel][pt][group] -> |eta| values, small-PV count
    std::vector<float> v[2][3][8]; long small[2][3][8] = {};
    const Long64_t n = c.GetEntries();
    for (Long64_t ie = 0; ie < n; ++ie)
    {
        if (c.LoadTree(ie) < 0 || c.GetEntry(ie) <= 0) { printf("READ FAILED\n"); gSystem->Exit(1); }
        for (size_t i = 0; i < pt->size(); ++i)
        {
            if (!(*uv)[i] || (*uf)[i] || (*d0)[i] == -777 || !std::isfinite((*ez)[i])) continue;
            int ip = -1;
            for (int k = 0; k < 3; ++k) if ((*pt)[i] > PT[k] && (*pt)[i] < PT[k + 1]) ip = k;
            const double ae = std::fabs((*eta)[i]);
            if (ip < 0 || ae > 3.0) continue;
            const double s = std::sin((*phi)[i]), co = std::cos((*phi)[i]), sh = std::sinh((*eta)[i]);
            const double sdz = std::sqrt(co * co * sh * sh * (*ex)[i] * (*ex)[i] + s * s * sh * sh * (*ey)[i] * (*ey)[i] + (*ez)[i] * (*ez)[i]
                                         + 2 * co * s * sh * sh * (*cxy)[i] - 2 * co * sh * (*cxz)[i] - 2 * s * sh * (*cyz)[i]);
            int g = -1;
            for (int k = 1; k < 7; ++k) if (sdz >= E[k] && sdz < E[k + 1]) g = k;
            if (g < 0) continue;
            for (int es = 0; es < 2; ++es)
            {
                if (es == 1 && ae > 1.3) continue;
                v[es][ip][g].push_back(ae);
                if (ntrk < 20) ++small[es][ip][g];
            }
        }
    }
    const char *ES[2] = {"|eta|<3.0", "|eta|<1.3"}, *PS[3] = {"0.1-1 GeV", "1-3 GeV", "3-10 GeV"};
    for (int es = 0; es < 2; ++es)
        for (int ip = 0; ip < 3; ++ip)
        {
            printf("\n%s, pT %s: group(sigma_PV,dz um)  ntrk   |eta| q16 / median / q84   frac from PV with <20 tracks\n", ES[es], PS[ip]);
            for (int g = 1; g < 7; ++g)
            {
                auto &x = v[es][ip][g];
                if (x.empty()) { printf("  %4.1f-%4.1f  empty\n", E[g], E[g + 1]); continue; }
                std::sort(x.begin(), x.end());
                auto q = [&](double f) { return x[std::min<size_t>(x.size() - 1, f * x.size())]; };
                printf("  %4.1f-%4.1f  %8zu   %.2f / %.2f / %.2f   %.1e\n", E[g], E[g + 1], x.size(), q(0.16), q(0.5), q(0.84),
                       (double)small[es][ip][g] / x.size());
            }
        }
}
