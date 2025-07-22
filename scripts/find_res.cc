#include <TH1F.h>

float find_res(TH1F* hist, float mu, float fraction=0.68) {

    int bin_mu = hist->FindBin(mu);
    float totalEntries = hist->Integral();
    float targetEntries = fraction * totalEntries;

    int bin_left = bin_mu, bin_right = bin_mu;
    float sum = hist->GetBinContent(bin_mu);
    
    while (sum < targetEntries) {
        if (bin_left > 1) bin_left--;
        if (bin_right < hist->GetNbinsX()) bin_right++;
        
        sum = hist->Integral(bin_left, bin_right);
        
        if (bin_left == 1 && bin_right == hist->GetNbinsX()) break;
    }
    
    float x_low = hist->GetBinLowEdge(bin_left);
    float x_high = hist->GetBinLowEdge(bin_right) + hist->GetBinWidth(bin_right);
    
    return (x_high - x_low) / 2.0;
}
