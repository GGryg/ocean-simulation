#include "fft.h"

#include <cmath>

#include "constants.h"

FFT::FFT(unsigned int N_t)
    : m_N{N_t}
    , m_bufferIndex{}
    , m_log_2_N{static_cast<const unsigned int>(std::log(m_N))} // Precalculate size of indices
{
    // Reverse bits in the indices
    for(int i = 0; i < m_N; ++i)
    {
        m_reversedIndices.push_back(reverseBits(i));
    }


    // Precompute twiddle factors (e^(-2PIik/N))
    int helper = 1;
    for(int i = 0; i < m_log_2_N; ++i)
    {
        m_twiddleFactors.push_back(std::vector<std::complex<float>>(helper));
        for(int j = 0; j < helper; ++j)
        {
            m_twiddleFactors[i][j] = calculateTwiddleFactor(j, helper * 2);
        }
        helper *= 2;
    }

    m_temporaryBuffers[0] = VecCompf(m_N);
    m_temporaryBuffers[1] = VecCompf(m_N);
}

FFT::~FFT()
{

}

unsigned int FFT::reverseBits(unsigned int n)
{
    unsigned int result = 0;
    int numBits = log2(m_N);
    for(int i = 0; i < numBits; ++i)
    {
        result |= (n & 1) << (numBits - 1 - i);
        n >>= 1;
    }
    return result;
}

std::complex<float> FFT::calculateTwiddleFactor(unsigned int x, unsigned int N)
{
    return std::polar(1.0f, constants::pi2 * x / N);
}

void FFT::fft(VecCompf& data, int stride, int offset)
{
    // reorder data (data's length is N * N)
    for(int i = 0; i < m_N; ++i)
    {
        m_temporaryBuffers[m_bufferIndex][i] = data[m_reversedIndices[i] * stride * offset];
    }
    int loops = m_N / 2;
    int blockSize = 2;
    int halfBlockSize = 1;
    int twiddleIndex = 0;
    // "Butterfly" operations
    for(int i = 1; i <= m_log_2_N; ++i)
    {
        m_bufferIndex ^= 1;
        for(int j = 0; j < loops; ++j)
        {
            // Xk = Ek + e ^ ((-2PIi/N)*k) * Ok
            for(int k = 0; k < halfBlockSize; ++k)
            {
                m_temporaryBuffers[m_bufferIndex][blockSize * j + k] = m_temporaryBuffers[m_bufferIndex^1][blockSize * j + k] + 
                                             m_temporaryBuffers[m_bufferIndex^1][blockSize * j + k] * m_twiddleFactors[twiddleIndex][k];
            }

            // Xk+(N/2) = Ek - e ^ ((-2PIi/N)*k) * Ok
            for(int k = halfBlockSize; k < blockSize; ++k)
            {
                m_temporaryBuffers[m_bufferIndex][blockSize * j + k] = m_temporaryBuffers[m_bufferIndex^1][blockSize * j - halfBlockSize + k] -
                                             m_temporaryBuffers[m_bufferIndex^1][blockSize * j + k] * m_twiddleFactors[twiddleIndex][k - halfBlockSize];
            }
        }
        loops /= 2;
        blockSize *= 2;
        halfBlockSize *= 2;
        twiddleIndex++;
    }
    // save calculated data
    for(int i = 0; i < m_N; ++i)
    {
        data[i * stride + offset] = m_temporaryBuffers[m_bufferIndex][i];
    }
}