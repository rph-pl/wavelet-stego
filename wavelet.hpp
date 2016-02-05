#ifndef WAVELET_HPP__
#define WAVELET_HPP__

#include <vector>
#include <functional>

#include "image.hpp"

#define TRESHOLD 10.0

namespace Stego
{
	class Wavelet
	{
		public:
			Wavelet();
			Wavelet(const Image& image);
			Wavelet(const Wavelet&) = default;
			Wavelet& operator=(const Wavelet&) = default;
			void hide_text(const std::string& text);
			std::string read_text() const;
			Image to_image() const;
			Image to_wavelet_image() const;
			size_t count_bytes_to_write() const;

		private:
			std::vector<double> merge_container_channels() const;
			void split_container_channels(const std::vector<double>& all);
			static char bits_to_char(const std::vector<char>& bits);
			static std::vector<char> char_to_bits(char c);

		private:
			size_t m_size;
			std::vector<double> m_red;
			std::vector<double> m_green;
			std::vector<double> m_blue;

			size_t m_container_size;
			std::vector<double> m_container_red;
			std::vector<double> m_container_green;
			std::vector<double> m_container_blue;

		private:
			std::function<bool(double)> m_checker = [](double v)
			{
				return fabs(v) < TRESHOLD;
			};
	};
}

#endif

