#include <matplot/matplot.h>
#include <iostream>
#include <vector>
#include <complex>
#include <numeric>
#include <cmath>
#include <thread>

#include <sndfile.h>
#include <fstream>
#define PI 3.14159265358979323846
#define TAU 2*PI

std::complex<double> expr(double in, double freq, size_t size, int place) {
    using namespace std::complex_literals;
    return in * (std::exp(-1.0i * (TAU/size) * (double)place));
}
void dft(std::vector<double> input, size_t pos, std::vector<std::complex<double>>& res) {
        for(size_t j = 0; j < input.size(); ++j) {
            std::complex<double> exponent = std::complex<double>(0.0, (-TAU * pos * j)/input.size());
            res[pos] += input[j] * std::exp(exponent);
        }
}
std::vector<std::complex<double>> dft_para(std::vector<double> input) {
    std::vector<std::thread> threads;
    std::vector<std::complex<double>> out(input.size());
    for(size_t k = 0; k < input.size(); ++k) {
        std::cout << k << " of " << input.size() << '\n';
        threads.push_back(std::thread(dft, std::ref(input), k,std::ref(out)));
    }
    for(auto& i : threads) {
        i.join();
    }
    return out;
}

int main() {
    SF_INFO sf_info;
    SNDFILE* file = sf_open("audio.wav", SFM_READ, &sf_info);
    if(file == nullptr) {
        std::cout << "bad\n";
        return -1;
    } 
    size_t samples = sf_info.frames;
    std::vector<double> input(samples);
    std::vector<float> buf(samples);
    sf_readf_float(file, buf.data(), samples);
    for(size_t i = 0; i < samples; ++i) {
        input[i] = static_cast<double>(buf[i]);
    }
    sf_close(file);
    std::cout << "finished generating\n";
    std::vector<double> y;
    auto x = dft_para(input);
    for(size_t i = 0; i < x.size(); ++i) {
        if(abs(x[i].imag()) > 1 || abs(x[i].real()) > 1)
            std::cout << input[i] << " : " << ((double)i * sf_info.samplerate / samples)<< " : " << x[i] << '\n';
    }
    for(auto i : input) {std::cout << i << ", ";}
}