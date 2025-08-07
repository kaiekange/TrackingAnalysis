void compare_gr(TGraph * gr1, TGraph * gr2, TGraph * div_gr, float height, float ground, float xmin, float xmax, float ymin, float ymax, TString leg1, TString leg2, TString given_text, TString xlabel, TString ylabel, TString figpath) {

    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

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

    gr1->SetMarkerStyle(20);
    gr1->SetMarkerColor(kBlack);
    gr2->SetMarkerStyle(20);
    gr2->SetMarkerColor(kRed);
    gr1->Draw("AP");
    gr2->Draw("P SAME");
    gr1->GetYaxis()->SetTitle(ylabel);
    write_text(0.55, 0.85, given_text);
    gr1->SetMaximum(height*1.5);
    gr1->SetMinimum(0.01);
    gr1->GetYaxis()->SetNdivisions(810);
    /* gr1->GetXaxis()->SetTitleSize(0.035); */
    /* gr1->GetXaxis()->SetTitleOffset(1.5); */
    TLegend * leg = new TLegend(0.55, 0.65, 0.65, 0.75, NULL, "brNDC");
    leg->AddEntry(gr1, leg1, "p");
    leg->AddEntry(gr2, leg2, "p");
    leg->SetTextFont(42);
    leg->SetTextColor(kBlack);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->SetLineWidth(0);
    leg->SetTextSize(0.04);
    leg->Draw();
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad* c2 = canvas->GetPad(2);
    c2->SetPad(0,0,1,0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    div_gr->Draw("AP");
	div_gr->SetMarkerColor(kBlack);
	div_gr->SetMarkerSize(1);
    div_gr->GetXaxis()->SetTitleOffset(1.0);
	div_gr->GetYaxis()->SetTitleOffset(0.8);
    div_gr->GetXaxis()->SetTickLength(0.06);
    div_gr->GetYaxis()->SetNdivisions(504);
    div_gr->GetXaxis()->SetNdivisions(810);
    div_gr->GetXaxis()->SetTitle(xlabel);
    div_gr->GetYaxis()->SetTitle("Data / MC ratio");
    div_gr->GetYaxis()->SetRangeUser(ymin, ymax);
    div_gr->GetXaxis()->SetTitleSize(0.1);
    div_gr->GetXaxis()->SetLabelSize(0.1); 
    div_gr->GetYaxis()->SetTitleSize(0.1);
    div_gr->GetYaxis()->SetLabelSize(0.1);
	/* TF1 *line = new TF1("line1", "1", c2->GetUxmin(), c2->GetUxmax()); */
    TLine* line = new TLine(xmin, 1.0, xmax, 1.0);
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->SetLineColor(kRed);
    line->Draw("same");
    canvas->Update();
    canvas->SaveAs(figpath+".png");

    gr1->SetMaximum(height*100.);
    gr1->SetMinimum(ground/100.);
    c1->SetLogy(1);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + "_log.png");
}
