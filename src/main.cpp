#include <matplot/matplot.h>
#include <iostream>
#include <vector>
#include <complex>
#include <numeric>
#include <cmath>
#include <thread>

#include <sndfile.hh>
#include <fstream>
#include "kiss_fft.h"
#define PI 3.14159265358979323846
#define TAU 2*PI


int main() {
    SndfileHandle file;
    file = SndfileHandle("audio.wav");
    size_t samples = file.frames();
    std::vector<double> input(samples);
    kiss_fft_cpx in[samples];
    kiss_fft_cpx out[samples];
    file.write(in.r, samples);
    kiss_fft_cfg = kiss_fft_alloc(samples, 0, nullptr, nullptr);
    if(!cfg) { std::cout << "bad\n"; return -1;}
    kiss_fft(cfg, in, out);

    std::cout << "finished copying\n";
    int it = 0;

    //cleans up data using hann window https://stackoverflow.com/questions/3555318/implement-hann-window
    // std::transform(input.begin(), input.end(), [&](double x){ return x * 0.5(1-std::cos((TAU * it++)/(input.size()-1)))});
    for(size_t i = 0; i < x.size(); ++i) {
        std::cout << "Bin "<< i << "freq: " <<i * (file.samplerate()/samples) << " : (" << out[i].r << out[i].i << ")" << '\n';
    }
    free(cfg);
}