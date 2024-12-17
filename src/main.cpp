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
    std::cout << "Thread starting from pos " << pos << '\n';
    size_t i = pos;
    for(; i < range || i < input.size(); ++i) {
        if(i == (range + pos)/4) std::cout << "25% done\n";
        if(i == (range + pos)/2) std::cout << "50% done\n";
        if(i == (3 * (range + pos))/2) std::cout << "75% done\n";
        for(size_t j = 0; j < input.size(); ++j) {
            std::complex<double> exponent = std::complex<double>(0.0, (-TAU * pos * j)/input.size());
            res[pos] += input[j] * std::exp(exponent);
        }
    }
    std::cout << "Thread finished at " << i <<  '\n';
}
std::vector<std::complex<double>> dft_para(std::vector<double> input) {
    const size_t THREAD_COUNT = 8;
    size_t THREAD_SPACE = ceil(input.size()/THREAD_COUNT);
    std::array<std::thread, THREAD_COUNT> threads;
    std::vector<std::complex<double>> out(input.size());
    for(size_t k = 0; k < THREAD_COUNT; k += 1) {
        std::cout << k+1 << " of " << THREAD_COUNT << '\n';
        threads[k] = std::thread(dft_range, std::ref(input), k * THREAD_SPACE, THREAD_SPACE, std::ref(out));
    }
    for(auto& i : threads) {
        i.join();
    }
    return out;
}

//no
std::vector<std::complex<double>> dft_seq(std::vector<double> input) {
    std::vector<std::complex<double>> res;
    dft_range(input, 0, input.size(), res);
    return res;
}

int main() {
    SndfileHandle file;
    file = SndfileHandle("audio.wav");
    size_t samples = file.frames();
    std::vector<double> input(samples);
    file.write(input.data(), samples);
    std::cout << "finished generating\n";
    int it = 0;

    //cleans up data using hann window https://stackoverflow.com/questions/3555318/implement-hann-window
    // std::transform(input.begin(), input.end(), [&](double x){ return x * 0.5(1-std::cos((TAU * it++)/(input.size()-1)))});
    auto x = dft_para(input);
    for(size_t i = 0; i < x.size(); ++i) {
        if(abs(x[i].imag()) > 1 || abs(x[i].real()) > 1)
            std::cout << input[i] << " : " << ((double)i * file.samplerate() / samples)<< " : " << x[i] << '\n';
    }
    for(auto i : input) {std::cout << i << ", ";}
}