#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

class HLTanalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
    public:
        explicit HLTanalyzer(const edm::ParameterSet&);
        ~HLTanalyzer() override;

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken_;
        void beginJob() override;
        void analyze(const edm::Event&, const edm::EventSetup&) override;
        void endJob() override;
};

HLTanalyzer::HLTanalyzer(const edm::ParameterSet& iConfig) 
    : triggerResultsToken_(consumes<edm::TriggerResults>(
                iConfig.getParameter<edm::InputTag>("triggerResults"))) {}

HLTanalyzer::~HLTanalyzer() {}

void HLTanalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    edm::Handle<edm::TriggerResults> triggerResults;
    iEvent.getByToken(triggerResultsToken_, triggerResults);

    if (!triggerResults.isValid()) {
        std::cout << "No TriggerResults found in this event!" << std::endl;
        return;
    }

    const edm::TriggerNames &names = iEvent.triggerNames(*triggerResults);

    std::cout << "HLT paths in this event:" << std::endl;
    for (unsigned int i = 0; i < triggerResults->size(); ++i) {
        std::string pathName = names.triggerName(i);
        bool fired = triggerResults->accept(i);
        std::cout << "  " << pathName << " -> " << (fired ? "PASSED" : "FAILED") << std::endl;
    }
}

void HLTanalyzer::beginJob() {}

void HLTanalyzer::endJob() {}

void HLTanalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HLTanalyzer);
