# CLAUDE.md

Branch `CMSSW_15_0_15_patch4`: ntuple production (EDAnalyzer + crab) for the CMS IP/PV resolution analysis in release **CMSSW_15_0_15_patch4** (`el8_amd64_gcc12`). The analysis macros (`analysis/`) and the full documentation live on `master` (CMSSW_15_0_16); see `master:CLAUDE.md` for the EDAnalyzer description.

## Rules for this branch

- Keep `EDAnalyzers/` identical to `master` except for changes the release forces, so analyzer fixes on `master` can be cherry-picked. Release-specific changes so far: none.
- The offline PV configuration (`offlinePrimaryVertices`) of this release is identical to 13_0_13 / 15_0_16, so `residuals_cfi.py` / `VertexReProducer` need no change.

## Setup and build (el8 host only: lxplus8 or `cmssw-el8`)

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_15_patch4/src
cmsenv
scram b -j8
objdump -T ../lib/el8_amd64_gcc12/*TrackingAnalysis*.so | grep -o 'GLIBC_2\.[0-9]*' | sort -uV | tail -1   # must be <= GLIBC_2.28
```

## Crab production

From `EDAnalyzers/crab/` after `cmsenv` in this area (crab ships the current release): set `SAMPLETYPE` and `YEAR` in `submitData.sh`, adjust `UNITSPERJOB` (~100 JetMET, ~150 ZeroBias), then `bash submitData.sh`.

- Dataset lists `DatasetList/data_<YEAR>_<SAMPLETYPE>.txt` (force-added): `<dataset> <globaltag>` per line; the global tag is per dataset.
- Version tag `Track-v20261122`, output `/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20261122/<PD>/<era>/` on T2_BE_IIHE.

| Period | Datasets (PromptReco MINIAOD, reco releases 15_0_6_patch1 … 15_0_15_patch4) | Global tag |
|---|---|---|
| 2025 | C–G (C, F: v1 + v2), JetMET0+1 / ZeroBias | 150X_dataRun3_Prompt_v1 |

Lumi mask: `JSON/Cert_Collisions2025_391658_398903_Golden.json`.

The newest prompt-reco release of 2025 is used, so every 2025 MINIAOD file can be read (an older release is not guaranteed to read files written by a newer one).
