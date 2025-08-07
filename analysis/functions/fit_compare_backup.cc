std::pair<float, float> fit_compare(TH1F * hist_data, TH1F * hist_mc, TString figpath, float tolerance = 1e-4) {

    lumi_sqrtS = "13.6 TeV, 2022";

    hist_data->Scale(1./hist_data->Integral());
    hist_mc->Scale(1./hist_mc->Integral());

    TH1F * hist_ratio = (TH1F*)hist_data->Clone("hist_ratio");
    hist_ratio->Divide(hist_mc);

    RooRealVar pv_var("pv_var", "pv_var", hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());

    RooRealVar mu_data("mu_data", "mu_data", hist_data->GetMean(), hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());
    RooRealVar sigma1_data("sigma1_data", "sigma1_data", 0.5*hist_data->GetRMS(), 0., hist_data->GetRMS());
    RooRealVar sigma2_data("sigma2_data", "sigma2_data", hist_data->GetRMS(), hist_data->GetRMS()*0.5, hist_data->GetRMS()*2);
    RooRealVar sigma3_data("sigma3_data", "sigma3_data", 1.5*hist_data->GetRMS(), hist_data->GetRMS(), hist_data->GetRMS()*3);
    RooGaussian gauss1_data("gauss1_data", "gauss1_data", pv_var, mu_data, sigma1_data);
    RooGaussian gauss2_data("gauss2_data", "gauss2_data", pv_var, mu_data, sigma2_data);
    RooGaussian gauss3_data("gauss3_data", "gauss3_data", pv_var, mu_data, sigma3_data);
    RooRealVar f1_data("f1_data", "f1_data", 0.3, 0.0, 1.0);
    RooRealVar f2_data("f2_data", "f2_data", 0.6, 0.0, 1.0);
    RooAddPdf triGauss_data("triGauss_data", "triGauss_data", RooArgList(gauss1_data, gauss2_data, gauss3_data), RooArgList(f1_data, f2_data));

    RooDataHist hdatahist_data("hdatahist_data", "", pv_var, hist_data);
    RooFitResult *fitResult_data = triGauss_data.fitTo(hdatahist_data, RooFit::Save(true));
    fitResult_data->Print();

    RooRealVar mu_mc("mu_mc", "mu_mc", hist_mc->GetMean(), hist_mc->GetXaxis()->GetXmin(), hist_mc->GetXaxis()->GetXmax());
    RooRealVar sigma1_mc("sigma1_mc", "sigma1_mc", 0.5*hist_mc->GetRMS(), 0., hist_mc->GetRMS());
    RooRealVar sigma2_mc("sigma2_mc", "sigma2_mc", hist_mc->GetRMS(), hist_mc->GetRMS()*0.5, hist_mc->GetRMS()*2);
    RooRealVar sigma3_mc("sigma3_mc", "sigma3_mc", 1.5*hist_mc->GetRMS(), hist_mc->GetRMS(), hist_mc->GetRMS()*3);
    RooGaussian gauss1_mc("gauss1_mc", "gauss1_mc", pv_var, mu_mc, sigma1_mc);
    RooGaussian gauss2_mc("gauss2_mc", "gauss2_mc", pv_var, mu_mc, sigma2_mc);
    RooGaussian gauss3_mc("gauss3_mc", "gauss3_mc", pv_var, mu_mc, sigma3_mc);
    RooRealVar f1_mc("f1_mc", "f1_mc", 0.5, 0.0, 1.0);
    RooRealVar f2_mc("f2_mc", "f2_mc", 0.5, 0.0, 1.0);
    RooAddPdf triGauss_mc("triGauss_mc", "triGauss_mc", RooArgList(gauss1_mc, gauss2_mc, gauss3_mc), RooArgList(f1_mc, f2_mc));

    RooDataHist hdatahist_mc("hdatahist_mc", "", pv_var, hist_mc);
    RooFitResult *fitResult_mc = triGauss_mc.fitTo(hdatahist_mc, RooFit::Save(true));
    fitResult_mc->Print();

    float pv_var_max = pv_var.getMax();

    float mean_data = mu_data.getVal();
    float low_data = 0.0;
    float high_data = pv_var_max - mean_data;
    while (high_data - low_data > tolerance) {
        float mid_data = 0.5 * (low_data + high_data);
        pv_var.setRange("intRange", mean_data - mid_data, mean_data + mid_data);
        RooAbsReal* integral_data = triGauss_data.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
        float prob_data = integral_data->getVal();
        if (prob_data < 0.68) low_data = mid_data;
        else high_data = mid_data;
        delete integral_data;
    }
    float reso_data = 0.5 * (low_data + high_data);


    float mean_mc = mu_mc.getVal();
    float low_mc = 0.0;
    float high_mc =pv_var_max - mean_mc;
    while (high_mc - low_mc > tolerance) {
        float mid_mc = 0.5 * (low_mc + high_mc);
        pv_var.setRange("intRange", mean_mc - mid_mc, mean_mc + mid_mc);
        RooAbsReal* integral_mc = triGauss_mc.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
        float prob_mc = integral_mc->getVal();
        if (prob_mc < 0.68) low_mc = mid_mc;
        else high_mc = mid_mc;
        delete integral_mc;
    }
    float reso_mc = 0.5 * (low_mc + high_mc);

    /* float xmin = pv_var.getMin(); */
    /* float xmax = pv_var.getMax(); */
    /* float maxVal = 0.0; */
    /* float xAtMax = 0.0; */
    /* for (float val = xmin; val <= xmax; val += tolerance) { */
    /*     pv_var.setVal(val); */
    /*     float y = triGauss.getVal(pv_var); */
    /*     if (y > maxVal) { */
    /*         maxVal = y; */
    /*         xAtMax = val; */
    /*     } */
    /* } */
    /* float halfMax = maxVal / 2.0; */
    /* float left = xmin, right = xmax; */
    /* for (float val = xAtMax; val >= xmin; val -= tolerance) { */
    /*     pv_var.setVal(val); */
    /*     if (triGauss.getVal(pv_var) < halfMax) { */
    /*         left = val; */
    /*         break; */
    /*     } */
    /* } */
    /* for (float val = xAtMax; val <= xmax; val += tolerance) { */
    /*     pv_var.setVal(val); */
    /*     if (triGauss.getVal(pv_var) < halfMax) { */
    /*         right = val; */
    /*         break; */
    /*     } */
    /* } */

    /* float reso2 = (right - left) / 2.36; */

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 700);
    canvas->Divide(1,2);
    canvas->cd(1);
    TVirtualPad* c1 = canvas->GetPad(1);
    canvas_setup_sub(c1);
    c1->SetPad(0,0.3,1,1);
    c1->SetBottomMargin(0.0);
    c1->SetRightMargin(0.05);
    c1->SetLogy(0);
    c1->SetFillColor(0);
    c1->SetFrameFillColor(0);
    RooPlot *frame = pv_var.frame();

    hdatahist_mc.plotOn(frame, RooFit::Name("mc"), RooFit::FillColor(kOrange-9), RooFit::LineWidth(0), RooFit::Binning(100), RooFit::DrawOption("F"));
    triGauss_mc.plotOn(frame, RooFit::Name("triGauss_mc"), RooFit::Components("triGauss_mc"), RooFit::LineStyle(1), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    hdatahist_data.plotOn(frame, RooFit::Name("data"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(100), RooFit::DrawOption("ep"));
    triGauss_data.plotOn(frame, RooFit::Name("triGauss_data"), RooFit::Components("triGauss_data"), RooFit::LineStyle(9), RooFit::LineColor(kBlack), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame->Draw("");
    frame->GetYaxis()->SetTitle(hist_data->GetYaxis()->GetTitle());
    frame->GetYaxis()->SetNdivisions(810);
    frame->SetMinimum(0);
    frame->SetMaximum(frame->GetMaximum()*1.3);
    TLegend *legend = new TLegend(0.2, 0.5, 0.4, 0.7, NULL, "brNDC");
    legend->AddEntry(frame->findObject("data"), "data", "ep");
    legend->AddEntry(frame->findObject("triGauss_data"), "data fit", "l");
    legend->AddEntry(frame->findObject("mc"), "simulation", "f");
    legend->AddEntry(frame->findObject("triGauss_mc"), "simulation fit", "l");
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetLineWidth(0);
    legend->SetTextSize(0.03);
    legend->Draw();
    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8, hist_data->GetTitle());
    write_text(0.6, 0.7, Form("Fit Results:"));
    write_text(0.6, 0.65, Form("data reso = %.*f", std::max(0, 2-(int)floor(log10(reso_data))), reso_data));
    write_text(0.6, 0.6, Form("simulation reso = %.*f", std::max(0, 2-(int)floor(log10(reso_mc))), reso_mc));
    
    /* write_text(0.68, 0.65, Form("#mu = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getVal(), std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getError())); */
    /* write_text(0.68, 0.6, Form("#sigma_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma1.getVal()))), sigma1.getVal(), std::max(0, 2-(int)floor(log10(sigma1.getVal()))), sigma1.getError())); */
    /* write_text(0.68, 0.55, Form("#sigma_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma2.getVal()))), sigma2.getVal(), std::max(0, 2-(int)floor(log10(sigma2.getVal()))), sigma2.getError())); */
    /* write_text(0.68, 0.5, Form("#sigma_{3} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma3.getVal()))), sigma3.getVal(), std::max(0, 2-(int)floor(log10(sigma3.getVal()))), sigma3.getError())); */
    /* write_text(0.68, 0.45, Form("f_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(f1.getVal()))), f1.getVal(), std::max(0, 2-(int)floor(log10(f1.getVal()))), f1.getError())); */
    /* write_text(0.68, 0.4, Form("f_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(f2.getVal()))), f2.getVal(), std::max(0, 2-(int)floor(log10(f2.getVal()))), f2.getError())); */
    /* write_text(0.68, 0.35, Form("reso = %.*f", std::max(0, 2-(int)floor(log10(reso))), reso)); */
    /* write_text(0.68, 0.3, Form("FWHM/2.36 = %.*f", std::max(0, 2-(int)floor(log10(reso2))), reso2)); */
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad* c2 = canvas->GetPad(2);
    /* canvas_setup_sub(c2); */
    c2->SetPad(0,0,1,0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    hist_ratio->SetMarkerColor(kBlack);
    hist_ratio->SetMarkerSize(0.7);
    hist_ratio->GetXaxis()->SetTitle(hist_data->GetXaxis()->GetTitle());
    hist_ratio->GetXaxis()->SetNdivisions(810);
    hist_ratio->GetYaxis()->SetTitle("data / mc ratio");
    hist_ratio->SetMaximum(2);
    hist_ratio->SetMinimum(0.5);
    hist_ratio->Draw("ep");

    canvas->Update();
    canvas->SaveAs(figpath+".png");

    return {reso_data, reso_mc};
}
