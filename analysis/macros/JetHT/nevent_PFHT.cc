int nevent_PFHT(){

    /* ROOT::RDataFrame myDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_postEE_xsec.root"); */

    /* auto num_1050 = myDF.Filter("trig_PFHT1050_pass").Sum<double>("xsecweight"); */
    /* auto num_890 = myDF.Filter("(!trig_PFHT1050_pass)&&(trig_PFHT890_pass)").Sum<double>("xsecweight"); */
    /* auto num_780 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(trig_PFHT780_pass)").Sum<double>("xsecweight"); */
    /* auto num_680 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(trig_PFHT680_pass)").Sum<double>("xsecweight"); */
    /* auto num_590 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(trig_PFHT590_pass)").Sum<double>("xsecweight"); */
    /* auto num_510 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(trig_PFHT510_pass)").Sum<double>("xsecweight"); */
    /* auto num_430 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(trig_PFHT430_pass)").Sum<double>("xsecweight"); */
    /* auto num_370 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(trig_PFHT370_pass)").Sum<double>("xsecweight"); */
    /* auto num_250 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(trig_PFHT250_pass)").Sum<double>("xsecweight"); */
    /* auto num_180 = myDF.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&(trig_PFHT180_pass)").Sum<double>("xsecweight"); */

    /* std::cout << "1050: " << *num_1050 << std::endl; */
    /* std::cout << "890: " << *num_890 << std::endl; */
    /* std::cout << "780: " << *num_780 << std::endl; */
    /* std::cout << "680: " << *num_680 << std::endl; */
    /* std::cout << "590: " << *num_590 << std::endl; */
    /* std::cout << "510: " << *num_510 << std::endl; */
    /* std::cout << "430: " << *num_430 << std::endl; */
    /* std::cout << "370: " << *num_370 << std::endl; */
    /* std::cout << "250: " << *num_250 << std::endl; */
    /* std::cout << "180: " << *num_180 << std::endl; */

    TChain *mychain = new TChain("mytree");
    mychain->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_postEE.root");
   

    int num_1050 = mychain->GetEntries("trig_PFHT1050_pass");
    int num_890 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(trig_PFHT890_pass)");
    int num_780 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(trig_PFHT780_pass)");
    int num_680 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(trig_PFHT680_pass)");
    int num_590 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(trig_PFHT590_pass)");
    int num_510 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(trig_PFHT510_pass)");
    int num_430 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(trig_PFHT430_pass)");
    int num_370 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(trig_PFHT370_pass)");
    int num_250 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(trig_PFHT250_pass)");
    int num_180 = mychain->GetEntries("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&(trig_PFHT180_pass)");

    std::cout << "1050: " << num_1050 << std::endl;
    std::cout << "890: " << num_890 << std::endl;
    std::cout << "780: " << num_780 << std::endl;
    std::cout << "680: " << num_680 << std::endl;
    std::cout << "590: " << num_590 << std::endl;
    std::cout << "510: " << num_510 << std::endl;
    std::cout << "430: " << num_430 << std::endl;
    std::cout << "370: " << num_370 << std::endl;
    std::cout << "250: " << num_250 << std::endl;
    std::cout << "180: " << num_180 << std::endl;

    return 0;
}
