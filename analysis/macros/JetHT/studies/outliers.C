// Find probe tracks with huge projected unbiased-PV error (no pull: sigma_PV,d0 > 40 um or sigma_PV,dz > 200 um)
// and print their event's PV properties; summary by pv_NTracks.
// Run in cmsenv with X509_USER_PROXY and X509_CERT_DIR set (IIHE):
//   root -l -b -q 'outliers.C+("root://.../output_1.root,root://.../output_10.root")'
#include <TChain.h>
#include <TSystem.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <cmath>
#include <cstdio>
#include <map>
#include <vector>

void outliers(const char *files)
{
    TChain c("residuals/tree");
    TString fs(files);
    TObjArray *a = fs.Tokenize(",");
    for (int i = 0; i < a->GetEntries(); ++i)
        if (c.Add(((TObjString *)a->At(i))->GetString(), -1) != 1) { printf("ADD FAILED\n"); gSystem->Exit(1); }
    Int_t run, id, npv, ntrk, ndof; Float_t pvxe, pvye, pvze, spt2;   // pv_ndof is Int_t in the tree
    std::vector<Float_t> *pt = 0, *eta = 0, *phi = 0, *ex = 0, *ey = 0, *ez = 0, *cxy = 0, *cxz = 0, *cyz = 0, *d0 = 0, *dz = 0, *uspt2 = 0;
    std::vector<Bool_t> *uv = 0, *uf = 0;
    c.SetBranchStatus("*", 0);
    auto B = [&](const char *n, void *p) { c.SetBranchStatus(n, 1); c.SetBranchAddress(n, p); };
    B("ev_run", &run); B("ev_id", &id); B("ev_nPV", &npv); B("pv_NTracks", &ntrk); B("pv_ndof", &ndof);
    B("pv_xError", &pvxe); B("pv_yError", &pvye); B("pv_zError", &pvze); B("pv_SumTrackPt2", &spt2);
    B("pv_trk_pt", &pt); B("pv_trk_eta", &eta); B("pv_trk_phi", &phi);
    B("pv_trk_pvunbiased_xError", &ex); B("pv_trk_pvunbiased_yError", &ey); B("pv_trk_pvunbiased_zError", &ez);
    B("pv_trk_pvunbiased_covXY", &cxy); B("pv_trk_pvunbiased_covXZ", &cxz); B("pv_trk_pvunbiased_covYZ", &cyz);
    B("pv_trk_pvunbiased_IsValid", &uv); B("pv_trk_pvunbiased_IsFake", &uf);
    B("pv_trk_d0_pvunbiased", &d0); B("pv_trk_dz_pvunbiased", &dz); B("pv_trk_pvunbiased_SumTrackPt2", &uspt2);

    const Long64_t n = c.GetEntries();
    long ntot = 0, nout = 0, nev_out = 0;
    std::map<int, long> nt_all, nt_out;   // pv_NTracks (capped at 20) for all / outlier tracks
    int printed = 0;
    for (Long64_t ie = 0; ie < n; ++ie)
    {
        if (c.LoadTree(ie) < 0 || c.GetEntry(ie) <= 0) { printf("READ FAILED %lld\n", ie); gSystem->Exit(1); }
        int nout_ev = 0;
        for (size_t i = 0; i < pt->size(); ++i)
        {
            if (!(*uv)[i] || (*uf)[i] || (*d0)[i] == -777 || !std::isfinite((*ex)[i])) continue;
            const double s = std::sin((*phi)[i]), co = std::cos((*phi)[i]), sh = std::sinh((*eta)[i]);
            const double X = (*ex)[i] * (*ex)[i], Y = (*ey)[i] * (*ey)[i], Z = (*ez)[i] * (*ez)[i];
            const double sd0 = std::sqrt(s * s * X + co * co * Y - 2 * s * co * (*cxy)[i]);
            const double sdz = std::sqrt(co * co * sh * sh * X + s * s * sh * sh * Y + Z + 2 * co * s * sh * sh * (*cxy)[i]
                                         - 2 * co * sh * (*cxz)[i] - 2 * s * sh * (*cyz)[i]);
            const int k = std::min(ntrk, 20);
            ++ntot; ++nt_all[k];
            if (sd0 > 40 || sdz > 200)
            {
                ++nout; ++nt_out[k]; ++nout_ev;
                if (printed < 40)
                {
                    printf("run %d ev %d nPV %d | PV: ntrk %d ndof %d err(x,y,z) %.1f %.1f %.1f sqrtSpt2 %.1f | probe pt %.2f eta %.2f phi %.2f"
                           " | unbiased err(x,y,z) %.0f %.0f %.0f  sig_d0 %.0f sig_dz %.0f  d0 %.0f dz %.0f\n",
                           run, id, npv, ntrk, ndof, pvxe, pvye, pvze, std::sqrt(spt2), (*pt)[i], (*eta)[i], (*phi)[i],
                           (*ex)[i], (*ey)[i], (*ez)[i], sd0, sdz, (*d0)[i], (*dz)[i]);
                    ++printed;
                }
            }
        }
        if (nout_ev) ++nev_out;
    }
    printf("\nevents %lld, tracks %ld, outlier tracks %ld (%.2e), events with outliers %ld\n", n, ntot, nout, (double)nout / ntot, nev_out);
    printf("pv_NTracks : frac of all tracks | frac of outlier tracks | outlier rate\n");
    for (auto &[k, v] : nt_all)
        printf("%3d%s : %.4f | %.3f | %.1e\n", k, k == 20 ? "+" : " ", (double)v / ntot, (double)nt_out[k] / std::max(nout, 1L), (double)nt_out[k] / v);
}
