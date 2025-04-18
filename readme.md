# napfft

NOTE: `napfft` is a NAP core module as of [version 0.7.7](https://github.com/napframework/nap/releases/tag/v0.7.7). It is no longer maintained here!

Module that wraps [Kiss FFT](https://github.com/mborgerding/kissfft) functionality for NAP 0.6. Contributions are welcome!

- Implements real-optimized FFTs (positive half-spectrum: `nfft/2+1` complex frequency bins)
- Currently supports Windows only

## Example
```
// ~~~~~{.h}
std::unique_ptr<FFTBuffer> mFFTBuffer;

// ~~~~~{.cpp} 
// ::init()
uint buffer_size = 1024;
mFFTBuffer = std::make_unique<FFTBuffer>(buffer_size);

// ::update()
mFFTBuffer->supply(my_audio_sample_buffer);
const auto& amps = mFFTBuffer->getAmplitudes();

// ::gui()
const auto& amps = fft_comp->getFFTBuffer().getAmplitudes();
ImGui::PlotLines("FFT", amps.data(), amps.size());
```
