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

    unsigned int reverseBits(unsigned int i);
    std::complex<float> calculateTwiddleFactor(unsigned int x, unsigned int _N);
    void fft(VecCompf& data, int stride, int offset);

private:
    unsigned int m_N{};
    const unsigned int m_log_2_N;

    std::vector<unsigned int> m_reversedIndices;
    GridT m_twiddleFactors;
    VecCompf m_temporaryBuffers[2];
    unsigned int m_bufferIndex;
};

#endif