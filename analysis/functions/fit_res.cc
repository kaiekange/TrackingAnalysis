std::pair<float, float> fit_draw(TH1F* hist, TString figpath, float tolerance = 1e-4) {

    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", 0.5*hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), hist->GetRMS()*0.5, hist->GetRMS()*2);
    RooRealVar sigma3("sigma3", "sigma3", 1.5*hist->GetRMS(), hist->GetRMS(), hist->GetRMS()*3);
    RooGaussian gauss1("gauss1", "gauss1", pv_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", pv_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", pv_var, mu, sigma3);
    RooRealVar f1("f1", "f1", 0.3, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.3, 0.0, 1.0);
    RooFormulaVar f3("f3", "1 - f1 - f2", RooArgList(f1, f2));
    RooAddPdf triGauss("triGauss", "triGauss", RooArgList(gauss1, gauss2, gauss3), RooArgList(f1, f2, f3));

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();

    float mean = mu.getVal();
    float low = 0.0;
    float high = pv_var.getMax() - mean;
    while (high - low > tolerance) {
        float mid = 0.5 * (low + high);
        pv_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal* integral = triGauss.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
        float prob = integral->getVal();
        if (prob < 0.68) low = mid;
        else high = mid;
        delete integral;
    }
    float reso = 0.5 * (low + high);

    float xmin = pv_var.getMin();
    float xmax = pv_var.getMax();
    float maxVal = 0.0;
    float xAtMax = 0.0;
    for (float val = xmin; val <= xmax; val += tolerance) {
        pv_var.setVal(val);
        float y = triGauss.getVal(pv_var);
        if (y > maxVal) {
            maxVal = y;
            xAtMax = val;
        }
    }
    float halfMax = maxVal / 2.0;
    float left = xmin, right = xmax;
    for (float val = xAtMax; val >= xmin; val -= tolerance) {
        pv_var.setVal(val);
        if (triGauss.getVal(pv_var) < halfMax) {
            left = val;
            break;
        }
    }
    for (float val = xAtMax; val <= xmax; val += tolerance) {
        pv_var.setVal(val);
        if (triGauss.getVal(pv_var) < halfMax) {
            right = val;
            break;
        }
    }

    float reso2 = (right - left) / 2.36;

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
    hdatahist.plotOn(frame, RooFit::Name("data"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(100), RooFit::DrawOption("ep"));
    triGauss.plotOn(frame, RooFit::Name("triGauss"), RooFit::Components("triGauss"), RooFit::LineStyle(9), RooFit::LineColor(kBlue), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame->Draw("");
    frame->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    frame->GetYaxis()->SetNdivisions(810);
    frame->SetMinimum(0.01);
    frame->SetMaximum(frame->GetMaximum()*1.3);
    TLegend *legend = new TLegend(0.2, 0.5, 0.4, 0.7, NULL, "brNDC");
    legend->AddEntry(frame->findObject("data"), "data", "ep");
    legend->AddEntry(frame->findObject("triGauss"), "triple Gaussian", "l");
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetLineWidth(0);
    legend->SetTextSize(0.03);
    legend->Draw();
    write_text(0.68, 0.88, datatype_text);
    write_text(0.68, 0.8, hist->GetTitle());
    write_text(0.68, 0.7, Form("Fit Results:"));
    write_text(0.68, 0.65, Form("#mu = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getVal(), std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getError()));
    write_text(0.68, 0.6, Form("#sigma_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma1.getVal()))), sigma1.getVal(), std::max(0, 2-(int)floor(log10(sigma1.getVal()))), sigma1.getError()));
    write_text(0.68, 0.55, Form("#sigma_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma2.getVal()))), sigma2.getVal(), std::max(0, 2-(int)floor(log10(sigma2.getVal()))), sigma2.getError()));
    write_text(0.68, 0.5, Form("#sigma_{3} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(sigma3.getVal()))), sigma3.getVal(), std::max(0, 2-(int)floor(log10(sigma3.getVal()))), sigma3.getError()));
    write_text(0.68, 0.45, Form("f_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(f1.getVal()))), f1.getVal(), std::max(0, 2-(int)floor(log10(f1.getVal()))), f1.getError()));
    write_text(0.68, 0.4, Form("f_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(f2.getVal()))), f2.getVal(), std::max(0, 2-(int)floor(log10(f2.getVal()))), f2.getError()));
    write_text(0.68, 0.35, Form("f_{3} = %.*f", std::max(0, 2-(int)floor(log10(f3.getVal()))), f3.getVal()));
    write_text(0.68, 0.3, Form("reso = %.*f", std::max(0, 2-(int)floor(log10(reso))), reso));
    write_text(0.68, 0.25, Form("FWHM/2.36 = %.*f", std::max(0, 2-(int)floor(log10(reso2))), reso2));
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad* c2 = canvas->GetPad(2);
    c2->SetPad(0,0,1,0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    RooHist* hpull = frame->pullHist("data","triGauss");
    RooPlot* pull = pv_var.frame();
    hpull->SetFillColor(1);
    pull->addPlotable(hpull, "BX");
    pull->Draw();
    TAxis* xpull = pull->GetXaxis();
    TAxis* ypull = pull->GetYaxis();
    xpull->SetTitleOffset(3.2);
    xpull->SetTickLength(0.06);
    ypull->SetNdivisions(504);
    pull->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    pull->GetXaxis()->SetTitleOffset(1.);
    pull->SetYTitle("pull");
    pull->GetYaxis()->CenterTitle(true);
    pull->GetYaxis()->SetRangeUser(-5, 5);
    pull->GetYaxis()->SetTitleOffset(0.3);
    pull->GetXaxis()->SetTitleSize(0.1);
    pull->GetXaxis()->SetLabelSize(0.12);
    pull->GetYaxis()->SetTitleSize(0.12);
    pull->GetYaxis()->SetLabelSize(0.12);
    pull->GetXaxis()->SetNdivisions(810);
    pull->GetYaxis()->SetNdivisions(810);
    canvas->Update();
    canvas->SaveAs(figpath+".png");

    return {reso, reso2};
}
