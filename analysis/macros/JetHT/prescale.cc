#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <algorithm>
#include <nlohmann/json.hpp>

std::pair<int,int> getPS(TString trigname, std::string run_number, int lumisec){

    std::cout << "./trigjson/"+trigname+"prescales.json" << std::endl;
    std::ifstream PSfile("./trigjson/"+trigname+"_prescales.json");
    nlohmann::json PSjson;
    PSfile >> PSjson;
  
    int hltprescale = -1;
    int l1prescale = -1;


    if (PSjson["prescales"].contains(run_number)) {
        auto PSarr = PSjson["prescales"][run_number];

        for (auto& entry : PSarr) {

            int lumisec_start = entry["lumisec_start"];
            int lumisec_end   = entry["lumisec_end"];
            if (lumisec >= lumisec_start && lumisec <= lumisec_end) {
                hltprescale = entry["hltprescale"];
                l1prescale  = entry["l1prescale"];
            }
        }
    }

    return {l1prescale, hltprescale};
}


int prescale(){

    int ev_run;
    int ev_lumi;

    bool trig_PFHT180_pass;
    bool trig_PFHT250_pass;
    bool trig_PFHT350_pass;
    bool trig_PFHT370_pass;
    bool trig_PFHT430_pass;
    bool trig_PFHT510_pass;
    bool trig_PFHT590_pass;
    bool trig_PFHT680_pass;
    bool trig_PFHT780_pass;
    bool trig_PFHT890_pass;
    bool trig_PFHT1050_pass;

    mytree->SetBranchAddress("ev_run", &ev_run);
    mytree->SetBranchAddress("ev_lumi", &ev_lumi);
    mytree->SetBranchAddress("trig_PFHT180_pass", &trig_PFHT180_pass);
    mytree->SetBranchAddress("trig_PFHT250_pass", &trig_PFHT250_pass);
    mytree->SetBranchAddress("trig_PFHT350_pass", &trig_PFHT350_pass);
    mytree->SetBranchAddress("trig_PFHT370_pass", &trig_PFHT370_pass);
    mytree->SetBranchAddress("trig_PFHT430_pass", &trig_PFHT430_pass);
    mytree->SetBranchAddress("trig_PFHT510_pass", &trig_PFHT510_pass);
    mytree->SetBranchAddress("trig_PFHT590_pass", &trig_PFHT590_pass);
    mytree->SetBranchAddress("trig_PFHT680_pass", &trig_PFHT680_pass);
    mytree->SetBranchAddress("trig_PFHT780_pass", &trig_PFHT780_pass);
    mytree->SetBranchAddress("trig_PFHT890_pass", &trig_PFHT890_pass);
    mytree->SetBranchAddress("trig_PFHT1050_pass", &trig_PFHT1050_pass);

    int nentries = mytree->GetEntries();

    std::vector<std::pair<int, int>> fired_1050;
    std::vector<std::pair<int, int>> fired_890;
    std::vector<std::pair<int, int>> fired_780;
    std::vector<std::pair<int, int>> fired_680;
    std::vector<std::pair<int, int>> fired_590;
    std::vector<std::pair<int, int>> fired_510;
    std::vector<std::pair<int, int>> fired_430;
    std::vector<std::pair<int, int>> fired_370;
    std::vector<std::pair<int, int>> fired_350;
    std::vector<std::pair<int, int>> fired_250;
    std::vector<std::pair<int, int>> fired_180;

    for(int i=0; i<nentries; i++){
        mytree->GetEntry(i);

        std::pair<int,int> results;

        if(trig_PFHT1050_pass) fired_1050.push_back(getPS("HLT_PFHT1050_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT890_pass) fired_890.push_back(getPS("HLT_PFHT890_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT780_pass) fired_780.push_back(getPS("HLT_PFHT780_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT680_pass) fired_680.push_back(getPS("HLT_PFHT680_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT590_pass) fired_590.push_back(getPS("HLT_PFHT590_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT510_pass) fired_510.push_back(getPS("HLT_PFHT510_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT430_pass) fired_430.push_back(getPS("HLT_PFHT430_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT370_pass) fired_370.push_back(getPS("HLT_PFHT370_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT350_pass) fired_350.push_back(getPS("HLT_PFHT350_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT250_pass) fired_250.push_back(getPS("HLT_PFHT250_v", std::to_string(ev_run), ev_lumi));
        else if(trig_PFHT180_pass) fired_180.push_back(getPS("HLT_PFHT180_v", std::to_string(ev_run), ev_lumi));
    }

    return 0;
}
