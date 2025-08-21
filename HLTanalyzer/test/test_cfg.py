import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTanalyzer")

# Input file (replace with a valid CMS data file)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2022C/JetMET/MINIAOD/27Jun2023-v1/2820000/008d45a8-9f4d-4680-b4bb-3aad9b674a25.root' #jetmet
        # "/store/data/Run2022C/JetHT/MINIAOD/27Jun2023-v2/2530000/01b16473-4079-458e-a341-fdf194f38315.root" #jetht
        # "/store/data/Run2022B/ZeroBias/MINIAOD/27Jun2023-v1/2520000/0262c661-ecd4-49ba-aa14-f497aeec04f1.root" #zerobias
        # "/store/mc/Run3Summer22EEMiniAODv4/QCD_PT-80to120_TuneCP5_13p6TeV_pythia8/MINIAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/2520000/17fdedfc-9434-4c15-99fd-51d20df3cdbd.root" #qcd mc
        # "/store/mc/Run3Summer22EEMiniAODv4/SingleNeutrino_E-10_gun/MINIAODSIM/130X_mcRun3_2022_realistic_postEE_v6_ext1-v2/2540000/05418a44-547a-4d47-a0c7-1b1baed41e74.root" #singleneutrino mc
        )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)  # Only process 10 events for the test
)

# Analyzer
process.hltAnalyzer = cms.EDAnalyzer("HLTanalyzer",
        triggerResults = cms.InputTag("TriggerResults","","HLT")
        )

# Path
process.p = cms.Path(process.hltAnalyzer)
