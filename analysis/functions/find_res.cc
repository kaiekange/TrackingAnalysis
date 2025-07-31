std::pair<float, float> find_res(RooAddPdf* model, RooRealVar& x) {

    float tolerance = 1e-4;
    float low = 0.0;
    float high = x.getMax() - mu;

    while (high - low > tolerance) {
        float mid = 0.5 * (low + high);
        x.setRange("intRange", mu - mid, mu + mid);
        RooAbsReal* integral = model.createIntegral(x, NormSet(x), Range("intRange"));
        float prob = integral->getVal();

        if (prob < 0.68)
            low = mid;
        else
            high = mid;

        delete integral;
    }
    float reso = 0.5 * (low + high);

    float xmin = x.getMin();
    float xmax = x.getMax();
    float maxVal = 0.0;
    float xAtMax = 0.0;
    for (float val = xmin; val <= xmax; val += step) {
        x.setVal(val);
        float y = pdf.getVal(x);
        if (y > maxVal) {
            maxVal = y;
            xAtMax = val;
        }
    }
    float halfMax = maxVal / 2.0;
    float left = xmin, right = xmax;
    for (float val = xAtMax; val >= xmin; val -= step) {
        x.setVal(val);
        if (pdf.getVal(x) < halfMax) {
            left = val;
            break;
        }
    }

    for (float val = xAtMax; val <= xmax; val += step) {
        x.setVal(val);
        if (pdf.getVal(x) < halfMax) {
            right = val;
            break;
        }
    }

    float reso2 = right - left;
    return {reso, reso2};
}
