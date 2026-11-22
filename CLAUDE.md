# CLAUDE.md

Branch `CMSSW_13_0_13`: ntuple production (EDAnalyzer + crab) for the CMS IP/PV resolution analysis in release **CMSSW_13_0_13** (`el8_amd64_gcc11`). The analysis macros (`analysis/`) and the full documentation live on `master` (CMSSW_15_0_16); see `master:CLAUDE.md` for the EDAnalyzer description.

## Rules for this branch

- Keep `EDAnalyzers/` identical to `master` except for changes the release forces, so analyzer fixes on `master` can be cherry-picked. Release-specific changes so far:
  - `test/residuals.py`, `test/residuals_condor_group.py`: `options.SkipEvent` instead of `TryToContinue` (not known in 13_0).
- The offline PV configuration (`offlinePrimaryVertices`) of 13_0_13 is identical to 15_0_16, so `residuals_cfi.py` / `VertexReProducer` need no change.

## Setup and build (el8 host only: lxplus8 or `cmssw-el8`)

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_13_0_13/src
cmsenv
scram b -j8
objdump -T ../lib/el8_amd64_gcc11/*TrackingAnalysis*.so | grep -o 'GLIBC_2\.[0-9]*' | sort -uV | tail -1   # must be <= GLIBC_2.28
```

## Crab production

From `EDAnalyzers/crab/` after `cmsenv` in this area (crab ships the current release): set `SAMPLETYPE` and `YEAR` in `submitData.sh`, adjust `UNITSPERJOB` (~100 JetMET, ~150 ZeroBias), then `bash submitData.sh`.

- Dataset lists `DatasetList/data_<YEAR>_<SAMPLETYPE>.txt` (force-added): `<dataset> <globaltag>` per line; the global tag is per dataset.
- Version tag `Track-v20261122`, output `/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20261122/<PD>/<era>/` on T2_BE_IIHE.

| Period | Datasets (22Sep2023 MINIAOD; MINIAOD step was made in 13_0_13 with this GT) | Global tag |
|---|---|---|
| 2022 (preEE) | C, D | 130X_dataRun3_v2 |
| 2022EE (postEE) | E | 130X_dataRun3_v2 |
| 2022EE (postEE) | F, G | 130X_dataRun3_PromptAnalysis_v1 |
| 2023 (preBPix) | C (22Sep2023_v1..v4), JetMET0+1 / ZeroBias | 130X_dataRun3_PromptAnalysis_v1 |
| 2023BPix (postBPix) | D (22Sep2023_v1, v2), JetMET0+1 / ZeroBias | 130X_dataRun3_PromptAnalysis_v1 |

Lumi masks: official golden JSONs after the TrkML2026 review (`JSON/Cert_Collisions2022_355100_362760_Golden.json`, `JSON/Cert_Collisions2023_366442_370790_Golden.json`; the pre-review versions differ by < 0.5 % of the lumis).
