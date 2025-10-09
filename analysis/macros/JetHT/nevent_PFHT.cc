int nevent_PFHT(){

    /* TChain *datachain = new TChain("mytree"); */
    /* datachain->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_postEE.root"); */
  
    /* int num_1050_pur = datachain->GetEntries("trig_PFHT1050_pass"); */
    /* int num_890_pur = datachain->GetEntries("trig_PFHT890_pass"); */
    /* int num_780_pur = datachain->GetEntries("trig_PFHT780_pass"); */
    /* int num_680_pur = datachain->GetEntries("trig_PFHT680_pass"); */
    /* int num_590_pur = datachain->GetEntries("trig_PFHT590_pass"); */
    /* int num_510_pur = datachain->GetEntries("trig_PFHT510_pass"); */
    /* int num_430_pur = datachain->GetEntries("trig_PFHT430_pass"); */
    /* int num_370_pur = datachain->GetEntries("trig_PFHT370_pass"); */
    /* int num_250_pur = datachain->GetEntries("trig_PFHT250_pass"); */
    /* int num_180_pur = datachain->GetEntries("trig_PFHT180_pass"); */
    /* std::cout << "data pure" << std::endl; */
    /* std::cout << "1050: " << num_1050_pur << std::endl; */
    /* std::cout << "890: " << num_890_pur << std::endl; */
    /* std::cout << "780: " << num_780_pur << std::endl; */
    /* std::cout << "680: " << num_680_pur << std::endl; */
    /* std::cout << "590: " << num_590_pur << std::endl; */
    /* std::cout << "510: " << num_510_pur << std::endl; */
    /* std::cout << "430: " << num_430_pur << std::endl; */
    /* std::cout << "370: " << num_370_pur << std::endl; */
    /* std::cout << "250: " << num_250_pur << std::endl; */
    /* std::cout << "180: " << num_180_pur << std::endl; */

    /* int num_1050 = datachain->GetEntries("trig_PFHT1050_pass"); */
    /* int num_890 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(trig_PFHT890_pass)"); */
    /* int num_780 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(trig_PFHT780_pass)"); */
    /* int num_680 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(trig_PFHT680_pass)"); */
    /* int num_590 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(trig_PFHT590_pass)"); */
    /* int num_510 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(trig_PFHT510_pass)"); */
    /* int num_430 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(trig_PFHT430_pass)"); */
    /* int num_370 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(trig_PFHT370_pass)"); */
    /* int num_250 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(trig_PFHT250_pass)"); */
    /* int num_180 = datachain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&(trig_PFHT180_pass)"); */

    /* std::cout << "data accumulate" << std::endl; */
    /* std::cout << "1050: " << num_1050 << std::endl; */
    /* std::cout << "890: " << num_890 << std::endl; */
    /* std::cout << "780: " << num_780 << std::endl; */
    /* std::cout << "680: " << num_680 << std::endl; */
    /* std::cout << "590: " << num_590 << std::endl; */
    /* std::cout << "510: " << num_510 << std::endl; */
    /* std::cout << "430: " << num_430 << std::endl; */
    /* std::cout << "370: " << num_370 << std::endl; */
    /* std::cout << "250: " << num_250 << std::endl; */
    /* std::cout << "180: " << num_180 << "\n" << std::endl; */

    TString PTranges[] = {"50to80", "80to120", "120to170", "170to300", "300to470", "470to600", "600to800", "800to1000", "1000to1400", "1400to1800", "1800to2400", "2400to3200", "3200"};

    for(int i=0; i<13; i++){
        TChain *mcchain = new TChain("mytree");
        mcchain->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_XsecPS/all_skimmed_2022_mc_"+PTranges[i]+"_preEE.root");
        /* int mc_num_1050_pur = mcchain->GetEntries("trig_PFHT1050_pass"); */
        /* int mc_num_890_pur = mcchain->GetEntries("trig_PFHT890_pass"); */
        /* int mc_num_780_pur = mcchain->GetEntries("trig_PFHT780_pass"); */
        /* int mc_num_680_pur = mcchain->GetEntries("trig_PFHT680_pass"); */
        /* int mc_num_590_pur = mcchain->GetEntries("trig_PFHT590_pass"); */
        /* int mc_num_510_pur = mcchain->GetEntries("trig_PFHT510_pass"); */
        /* int mc_num_430_pur = mcchain->GetEntries("trig_PFHT430_pass"); */
        /* int mc_num_370_pur = mcchain->GetEntries("trig_PFHT370_pass"); */
        /* int mc_num_250_pur = mcchain->GetEntries("trig_PFHT250_pass"); */
        /* int mc_num_180_pur = mcchain->GetEntries("trig_PFHT180_pass"); */
        /* std::cout << "mc pure " << PTranges[i]  << std::endl; */
        /* std::cout << "1050: " << mc_num_1050_pur << std::endl; */
        /* std::cout << "890: " << mc_num_890_pur << std::endl; */
        /* std::cout << "780: " << mc_num_780_pur << std::endl; */
        /* std::cout << "680: " << mc_num_680_pur << std::endl; */
        /* std::cout << "590: " << mc_num_590_pur << std::endl; */
        /* std::cout << "510: " << mc_num_510_pur << std::endl; */
        /* std::cout << "430: " << mc_num_430_pur << std::endl; */
        /* std::cout << "370: " << mc_num_370_pur << std::endl; */
        /* std::cout << "250: " << mc_num_250_pur << std::endl; */
        /* std::cout << "180: " << mc_num_180_pur << std::endl; */

        int mc_num_1050 = mcchain->GetEntries("trig_PFHT1050_pass");
        int mc_num_890 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(trig_PFHT890_pass)");
        int mc_num_780 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(trig_PFHT780_pass)");
        int mc_num_680 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(trig_PFHT680_pass)");
        int mc_num_590 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(trig_PFHT590_pass)");
        int mc_num_510 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(trig_PFHT510_pass)");
        int mc_num_430 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(trig_PFHT430_pass)");
        int mc_num_370 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(trig_PFHT370_pass)");
        int mc_num_250 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(trig_PFHT250_pass)");
        int mc_num_180 = mcchain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&(trig_PFHT180_pass)");

        std::cout << "mc accumulate " << PTranges[i] << std::endl;
        std::cout << mc_num_1050 << std::endl;
        std::cout << mc_num_890 << std::endl;
        std::cout << mc_num_780 << std::endl;
        std::cout << mc_num_680 << std::endl;
        std::cout << mc_num_590 << std::endl;
        std::cout << mc_num_510 << std::endl;
        std::cout << mc_num_430 << std::endl;
        std::cout << mc_num_370 << std::endl;
        std::cout << mc_num_250 << std::endl;
        std::cout << mc_num_180 << "\n" << std::endl;

        /* std::cout << "1050: " << mc_num_1050 << std::endl; */
        /* std::cout << "890: " << mc_num_890 << std::endl; */
        /* std::cout << "780: " << mc_num_780 << std::endl; */
        /* std::cout << "680: " << mc_num_680 << std::endl; */
        /* std::cout << "590: " << mc_num_590 << std::endl; */
        /* std::cout << "510: " << mc_num_510 << std::endl; */
        /* std::cout << "430: " << mc_num_430 << std::endl; */
        /* std::cout << "370: " << mc_num_370 << std::endl; */
        /* std::cout << "250: " << mc_num_250 << std::endl; */
        /* std::cout << "180: " << mc_num_180 << "\n" << std::endl; */
    }

    return 0;
}
