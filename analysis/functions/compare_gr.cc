#include <TLine.h>
#include <TMath.h>
#include "plot_common.cc"

// Data vs MC overlay (top pad) with data/MC ratio (bottom pad).
// Both pads share the x range of data+MC; the ratio range grows beyond [0.9, 1.5] if needed.
void compare_gr(
    TGraph *gr1, TGraph *gr2, TGraph *div_gr,
    float height,
    TString period, TString given_text,
    TString xlabel, TString ylabel, TString figpath)
{
    lumi_sqrtS = lumi_label(period);
    period = period_title(period);

    // common x range of data and MC, with 5% padding
    Double_t xlo = 1e30, xhi = -1e30;
    for (TGraph *g : {gr1, gr2})
        if (g->GetN() > 0) {
            xlo = std::min(xlo, TMath::MinElement(g->GetN(), g->GetX()));
            xhi = std::max(xhi, TMath::MaxElement(g->GetN(), g->GetX()));
        }
    if (xlo > xhi) { xlo = 0; xhi = 1; }
    Double_t xpad = 0.05 * (xhi - xlo);
    xlo = (xlo >= 0 && xlo - xpad < 0) ? 0 : xlo - xpad;  // no negative pT / sqrt(sum pT^2)
    xhi += xpad;

    Double_t rlo = 0.9, rhi = 1.5;
    if (div_gr->GetN() > 0) {
        rlo = std::min(rlo, TMath::MinElement(div_gr->GetN(), div_gr->GetY()) - 0.05);
        rhi = std::max(rhi, TMath::MaxElement(div_gr->GetN(), div_gr->GetY()) + 0.05);
    }

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 700);
    canvas->Divide(1, 2);
    canvas->cd(1);
    TVirtualPad *c1 = canvas->GetPad(1);
    canvas_setup(c1);
    c1->SetPad(0, 0.3, 1, 1);
    c1->SetBottomMargin(0.02);
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
    gr1->Draw("P SAME");  // keep data on top of MC
    gr1->GetXaxis()->SetLimits(xlo, xhi);
    gr1->GetYaxis()->SetTitle(ylabel);
    gr1->GetXaxis()->SetLabelOffset(0.3);
    write_text(0.55, 0.85, given_text);
    gr1->SetMaximum(height * 1.5);
    gr1->SetMinimum(0.0);
    gr1->GetYaxis()->SetNdivisions(810);

    TLegend *leg = new TLegend(0.55, 0.65, 0.65, 0.75, NULL, "brNDC");
    leg->AddEntry(gr1, period + " data", "p");
    leg->AddEntry(gr2, period + " simulation", "p");
    leg->SetTextFont(42);
    leg->SetTextColor(kBlack);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->SetLineWidth(0);
    leg->SetTextSize(0.04);
    leg->Draw();
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad *c2 = canvas->GetPad(2);
    c2->SetPad(0, 0, 1, 0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    div_gr->Draw("AP");
    div_gr->SetMarkerColor(kBlack);
    div_gr->SetMarkerSize(1);
    div_gr->GetXaxis()->SetLimits(xlo, xhi);
    div_gr->GetXaxis()->SetTitleOffset(1.5);
    div_gr->GetYaxis()->SetTitleOffset(0.8);
    div_gr->GetXaxis()->SetTickLength(0.06);
    div_gr->GetYaxis()->SetNdivisions(504);
    div_gr->GetXaxis()->SetNdivisions(810);
    div_gr->GetXaxis()->SetTitle(xlabel);
    div_gr->GetYaxis()->SetTitle("Data / MC ratio");
    div_gr->SetMinimum(rlo);
    div_gr->SetMaximum(rhi);
    div_gr->GetXaxis()->SetTitleSize(0.1);
    div_gr->GetXaxis()->SetLabelSize(0.1);
    div_gr->GetYaxis()->SetTitleSize(0.1);
    div_gr->GetYaxis()->SetLabelSize(0.1);
    TLine *line = new TLine(xlo, 1.0, xhi, 1.0);
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->SetLineColor(kRed);
    line->Draw("same");
    canvas->Update();
    canvas->SaveAs(figpath + ".png");

    delete line;
    delete leg;
    delete canvas;
}
