#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>

using VecCompf = std::vector<std::complex<float>>;
using GridT = std::vector<VecCompf>;

class FFT
{
public:
    FFT(unsigned int N_t);
    ~FFT();

    unsigned int reverse(unsigned int i);
    std::complex<float> t(unsigned int x, unsigned int _N);
    void fft(VecCompf& input, VecCompf& output, int stride, int offset);

private:
    unsigned int m_N{};
    unsigned int m_index;
    unsigned int m_log_2_N;

    std::vector<unsigned int> m_reversed;
    GridT m_T;
    VecCompf m_c[2];
};

#endif