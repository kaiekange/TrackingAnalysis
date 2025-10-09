int unphys_event(){

    TString PTranges[] = {"50to80", "80to120", "120to170"};


    for(int i=0; i<3; i++){
        TChain *mcchain = new TChain("mytree");
        mcchain->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_XsecPS/all_skimmed_2022_mc_"+PTranges[i]+"_preEE.root");

        bool trig_PFHT1050_pass;
        bool trig_PFHT890_pass;
        bool trig_PFHT780_pass;
        bool trig_PFHT680_pass;
        bool trig_PFHT590_pass;
        bool trig_PFHT510_pass;
        bool trig_PFHT430_pass;
        float pv_SumTrackPt2;
        float pv_x_p1;
        float pv_x_p2;

        mcchain->SetBranchAddress("trig_PFHT1050_pass", &trig_PFHT1050_pass);
        mcchain->SetBranchAddress("trig_PFHT890_pass", &trig_PFHT890_pass);
        mcchain->SetBranchAddress("trig_PFHT780_pass", &trig_PFHT780_pass);
        mcchain->SetBranchAddress("trig_PFHT680_pass", &trig_PFHT680_pass);
        mcchain->SetBranchAddress("trig_PFHT590_pass", &trig_PFHT590_pass);
        mcchain->SetBranchAddress("trig_PFHT510_pass", &trig_PFHT510_pass);
        mcchain->SetBranchAddress("trig_PFHT430_pass", &trig_PFHT430_pass);
        mcchain->SetBranchAddress("pv_SumTrackPt2", &pv_SumTrackPt2);
        mcchain->SetBranchAddress("pv_x_p1", &pv_x_p1);
        mcchain->SetBranchAddress("pv_x_p2", &pv_x_p2);

        int nentries = mcchain->GetEntries();

        std::vector<float> sqrt_pv_SumTrackPt2_1050;
        std::vector<float> sqrt_pv_SumTrackPt2_890;
        std::vector<float> sqrt_pv_SumTrackPt2_780;
        std::vector<float> sqrt_pv_SumTrackPt2_680;
        std::vector<float> sqrt_pv_SumTrackPt2_590;
        std::vector<float> sqrt_pv_SumTrackPt2_510;
        std::vector<float> sqrt_pv_SumTrackPt2_430;
        std::vector<float> pv_x_diff_1050;
        std::vector<float> pv_x_diff_890;
        std::vector<float> pv_x_diff_780;
        std::vector<float> pv_x_diff_680;
        std::vector<float> pv_x_diff_590;
        std::vector<float> pv_x_diff_510;
        std::vector<float> pv_x_diff_430;

        for(int j=0; j<nentries; j++){

            mcchain->GetEntry(j);

            if(trig_PFHT1050_pass){
                sqrt_pv_SumTrackPt2_1050.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_1050.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT890_pass){
                sqrt_pv_SumTrackPt2_890.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_890.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT780_pass){
                sqrt_pv_SumTrackPt2_780.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_780.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT680_pass){
                sqrt_pv_SumTrackPt2_680.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_680.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT590_pass){
                sqrt_pv_SumTrackPt2_590.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_590.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT510_pass){
                sqrt_pv_SumTrackPt2_510.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_510.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
            else if(trig_PFHT430_pass){
                sqrt_pv_SumTrackPt2_430.push_back(std::sqrt(pv_SumTrackPt2));
                pv_x_diff_430.push_back((pv_x_p1 - pv_x_p2)/std::sqrt(2));
            }
        }

        std::cout << PTranges[i] << " 1050" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_1050.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_1050[j] << "," << pv_x_diff_1050[j] << std::endl;
        }
        std::cout << "\n" << std::endl;

        std::cout << PTranges[i] << " 890" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_890.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_890[j] << "," << pv_x_diff_890[j] << std::endl;
        }
        std::cout << "\n" << std::endl;

        std::cout << PTranges[i] << " 780" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_780.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_780[j] << "," << pv_x_diff_780[j] << std::endl;
        }
        std::cout << "\n" << std::endl;

        std::cout << PTranges[i] << " 680" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_680.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_680[j] << "," << pv_x_diff_680[j] << std::endl;
        }
        std::cout << "\n" << std::endl;

        std::cout << PTranges[i] << " 590" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_590.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_590[j] << "," << pv_x_diff_590[j] << std::endl;
        }
        std::cout << "\n" << std::endl;
        
        std::cout << PTranges[i] << " 510" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_510.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_510[j] << "," << pv_x_diff_510[j] << std::endl;
        }
        std::cout << "\n" << std::endl;
        
        std::cout << PTranges[i] << " 430" <<  std::endl;
        for(int j=0; j<sqrt_pv_SumTrackPt2_430.size(); j++){
            std::cout << sqrt_pv_SumTrackPt2_430[j] << "," << pv_x_diff_430[j] << std::endl;
        }
        std::cout << "\n\n" << std::endl;
    }

    return 0;
}
