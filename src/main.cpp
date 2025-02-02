#include <iostream>
#include <vector>
#include <cmath>
//#include <format>
#include <sndfile.hh>
#include "kiss_fftr.h"
#define PI 3.14159265358979323846
#define TAU 2*PI
#define kiss_fft_scalar double

int main(int argc, char ** argv) {
    SndfileHandle file;
    std::string filename = "audio/test1.wav";
    if(argc > 1) {
        filename = argv[1];
    }
    file = SndfileHandle(filename);
    size_t samples = file.frames();
    std::vector<float> input(samples);
    float in[samples];
    kiss_fft_cpx out[samples];
    file.read(input.data(), samples);
    if(file.error()) throw std::runtime_error(file.strError());
    // for(auto i : input) {
    //     std::cout << i << '\n';
    // }
    int N = 32768; //size of a chunk
    int overlap = N/2;
    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);
    
    int chunk = 0;
    std::vector<double> aggr_output(N/2);
    if(!cfg) { std::cout << "bad\n"; return -1;}
    for(size_t i = 0; i < samples; i += (N - overlap)) {
        for(size_t j = 0; j < N; ++j) {
            if(i + j < samples) {
                //https://stackoverflow.com/questions/3555318/implement-hann-window
                double hann = 0.5 * (1.0 - cos(TAU * j / (N - 1))); //helps reduce spectral leakage 
                in[j] = input[i + j] * hann;
            } else {
                //otherwise pad input
                in[j] = 0;
            }
        }
        kiss_fftr(cfg, in, out);
        for(size_t p = 0; p < N/2; ++p) {
            aggr_output[p] += std::sqrt( std::pow(out[p].r, 2) + std::pow(out[p].i, 2) );
        }
        chunk++;
    }
    free(cfg);
    std::vector<double> freq(N/2);
    for(size_t i = 0; i < aggr_output.size(); ++i) {
        aggr_output[i] /= (double)chunk;
        freq[i] = i * (double)file.samplerate()/N;
        // std::cout << freq[i] << " " << aggr_output[i] << '\n';
    }
    const int THRESHOLD = 100;
    for(size_t i = 1; i < aggr_output.size()-1; ++i) {
        if(aggr_output[i] > aggr_output[i-1] && aggr_output[i] > aggr_output[i + 1] && aggr_output[i] > THRESHOLD) {
            std::cout << freq[i] << " : " << aggr_output[i] << '\n';
        }
    }
    return 0;
}
