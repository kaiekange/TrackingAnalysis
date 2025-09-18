void draw_something(TH1F *hist, TString figpath){
    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    hist->Draw("ep");
    canvas->SaveAs(figpath);
}

int check_PFHT(){

    TChain *mychain = new TChain("mytree");
    mychain->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_preEE_xsec.root");

    TH1F *h1 = new TH1F("h1", ";# triggered events;#it{H_{T}} [GeV]", 500, 0, 3000);
    mychain->Project("h1", "ev_PFHT", "xsecweight*(trig_PFHT1050_pass)");
    draw_something(h1, "trig_figs/trig1050.png");

    TH1F *h2 = new TH1F("h2", ";# triggered events;#it{H_{T}} [GeV]", 500, 0, 3000);
    mychain->Project("h2", "ev_PFHT", "xsecweight*((!trig_PFHT1050_pass) && (trig_PFHT890_pass))");
    draw_something(h2, "trig_figs/trig890.png");

    TH1F *h3 = new TH1F("h3", ";# triggered events;#it{H_{T}} [GeV]", 500, 0, 3000);
    mychain->Project("h3", "ev_PFHT", "xsecweight*((!trig_PFHT1050_pass) && (!trig_PFHT890_pass) && (trig_PFHT780_pass))");
    draw_something(h3, "trig_figs/trig780.png");

    TH1F *h4 = new TH1F("h4", ";# triggered events;#it{H_{T}} [GeV]", 500, 0, 3000);
    mychain->Project("h4", "ev_PFHT", "xsecweight*((!trig_PFHT1050_pass) && (!trig_PFHT890_pass) && (!trig_PFHT780_pass) && (trig_PFHT680_pass))");
    draw_something(h4, "trig_figs/trig680.png");

    TH1F *h5 = new TH1F("h5", ";# triggered events;#it{H_{T}} [GeV]", 500, 0, 3000);
    mychain->Project("h5", "ev_PFHT", "xsecweight*((!trig_PFHT1050_pass) && (!trig_PFHT890_pass) && (!trig_PFHT780_pass) && (!trig_PFHT680_pass) && (trig_PFHT590_pass))");
    draw_something(h5, "trig_figs/trig590.png");



    return 0;
}
