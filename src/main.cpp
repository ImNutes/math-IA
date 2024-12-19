#include <matplot/matplot.h>
#include <iostream>
#include <vector>
#include <complex>
#include <numeric>
#include <cmath>
#include <thread>

#include <sndfile.hh>
#include <fstream>
#include "kiss_fftr.h"
#define PI 3.14159265358979323846
#define TAU 2*PI


int main() {
    SndfileHandle file;
    file = SndfileHandle("audio.wav");
    size_t samples = file.frames();
    std::vector<float> input(samples);
    float in[samples];
    kiss_fft_cpx out[samples];
    file.read(input.data(), samples);
    std::cout << "writing samples\n";
    int N = 4096; //size of a chunk
    int overlap = N/2;
    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);
    
    int chunk = 0;
    std::vector<double> aggr_output(N/2);
    if(!cfg) { std::cout << "bad\n"; return -1;}
    for(size_t i = 0; i < samples; i += (N - overlap)) {
        for(size_t j = 0; j < N; ++j) {
            if(i + j < samples) {
                double hann = 0.5 * (1.0 - cos(2 * M_PI * j / (N - 1))); //helps reduce spectral leakage 
                in[j] = input[i + j] * hann;
            } else {
                //otherwise pad input
                in[j] = 0;
            }
        }
        kiss_fftr(cfg, in, out);
        for(size_t p = 0; p < N/2; ++p) {
            aggr_output[p] += std::sqrt( std::pow(out[p].r, 2) + std::pow(out[p].i, 2) );
            if(abs(out[p].r) > 100 || abs(out[p].i) > 100)
                std::cout << "Bin "<< i << " : " << p << " of " << samples << " freq: " << (p * ((double)file.samplerate()/N)) << " : (" << out[p].r << ", " << out[p].i << ")" << '\n';
        }
        chunk++;
    }
    std::vector<double> freq(N/2);
    for(size_t i = 0; i < N/2; ++i) {
        aggr_output[i] /= (double)chunk;
        freq[i] = i * (double)file.samplerate()/N;
        std::cout << freq[i] << " : " << aggr_output[i] << '\n';
    }
    matplot::plot(freq, aggr_output);
    std::cout << "was stepping by " << (double)file.samplerate()/N << '\n';
    free(cfg);
    return 0;
}
