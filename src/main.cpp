#include <matplot/matplot.h>
#include <iostream>
#include <vector>
#include <complex>
#include <numeric>
#include <cmath>
#include <thread>

#include <sndfile.hh>
#include <fstream>
#define PI 3.14159265358979323846
#define TAU 2*PI


/* this is disgustingly unoptimized, i'm only doing this to showcase the properties of the discrete fourier transform.
 * in any other program, i would use the fft and be done.
*/
void dft_range(std::vector<double> input, size_t pos, size_t range,std::vector<std::complex<double>>& res) {
    for(size_t i = pos; i < range || i < input.size(); ++i) {
        for(size_t j = 0; j < input.size(); ++j) {
            std::complex<double> exponent = std::complex<double>(0.0, (-TAU * pos * j)/input.size());
            res[pos] += input[j] * std::exp(exponent);
        }
    }
}
std::vector<std::complex<double>> dft_para(std::vector<double> input) {
    const int THREAD_COUNT = 8;
    int THREAD_SPACE = ceil(input.size()/THREAD_COUNT);
    std::array<THREAD_COUNT> threads;
    std::vector<std::complex<double>> out(input.size());
    for(size_t k = 0; k < THREAD_COUNT; k += THREAD_SPACE) {
        std::cout << k << " of " << input.size() << '\n';
        threads[k] = std::thread(dft_range, k, (THREAD_SPACE), out);
    }
    for(auto& i : threads) {
        i.join();
    }
    return out;
}

int main() {
    SndfileHandle file;
    file = SndfileHandle("audio.wav");
    size_t samples = file.frames();
    std::vector<double> input(samples);
    std::vector<float> buf(samples);
    sf_readf_float(file, buf.data(), samples);
    for(size_t i = 0; i < samples; ++i) {
        input[i] = static_cast<double>(buf[i]);
    }
    sf_close(file);
    std::cout << "finished generating\n";
    int it = 0;

    //cleans up data using hann window https://stackoverflow.com/questions/3555318/implement-hann-window
    std::transform(input.begin(), input.end(), [&](double x){ return x * 0.5(1-std::cos((TAU * it++)/(input.size()-1)))})
    auto x = dft_para(input);
    for(size_t i = 0; i < x.size(); ++i) {
        if(abs(x[i].imag()) > 1 || abs(x[i].real()) > 1)
            std::cout << input[i] << " : " << ((double)i * sf_info.samplerate() / samples)<< " : " << x[i] << '\n';
    }
    for(auto i : input) {std::cout << i << ", ";}
}