#include "wavelet.hpp"

#include <algorithm>
#include <cmath>

#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_wavelet2d.h>

namespace Stego
{
	Wavelet::Wavelet() :
		m_size(0)
	{
	}

	Wavelet::Wavelet(const Image& image) :
		m_size(image.m_width),
		m_red(image.m_red),
		m_green(image.m_green),
		m_blue(image.m_blue),
		m_container_size(m_size / 2)
	{
		gsl_wavelet* wavelet = gsl_wavelet_alloc(gsl_wavelet_haar, 2);
		gsl_wavelet_workspace* wavelet_workspace = gsl_wavelet_workspace_alloc(m_size);

		gsl_wavelet2d_transform_forward(wavelet, m_red.data(), m_size, m_size, m_size, wavelet_workspace);
		gsl_wavelet2d_transform_forward(wavelet, m_green.data(), m_size, m_size, m_size, wavelet_workspace);
		gsl_wavelet2d_transform_forward(wavelet, m_blue.data(), m_size, m_size, m_size, wavelet_workspace);

		for(size_t i = image.m_width / 2; i < image.m_width; ++i)
		{
			for(size_t j = image.m_height / 2; j < image.m_height; ++j)
			{
				m_container_red.push_back(m_red[i * image.m_width + j]);
				m_container_green.push_back(m_green[i * image.m_width + j]);
				m_container_blue.push_back(m_blue[i * image.m_width + j]);
			}
		}

		gsl_wavelet_free(wavelet);
		gsl_wavelet_workspace_free(wavelet_workspace);
	}

	size_t Wavelet::count_bytes_to_write() const
	{
		return std::count_if(m_container_red.begin(), m_container_red.end(), m_checker)
			   + std::count_if(m_container_green.begin(), m_container_green.end(), m_checker)
			   + std::count_if(m_container_blue.begin(), m_container_blue.end(), m_checker);
	}

	Image Wavelet::to_image() const
	{
		gsl_wavelet* wavelet = gsl_wavelet_alloc(gsl_wavelet_haar, 2);
		gsl_wavelet_workspace* wavelet_workspace = gsl_wavelet_workspace_alloc(m_size);

		std::vector<double> red = m_red;
		std::vector<double> green = m_green;
		std::vector<double> blue = m_blue;

		gsl_wavelet2d_transform_inverse(wavelet, red.data(), m_size, m_size, m_size, wavelet_workspace);
		gsl_wavelet2d_transform_inverse(wavelet, green.data(), m_size, m_size, m_size, wavelet_workspace);
		gsl_wavelet2d_transform_inverse(wavelet, blue.data(), m_size, m_size, m_size, wavelet_workspace);

		gsl_wavelet_free(wavelet);
		gsl_wavelet_workspace_free(wavelet_workspace);

		return Image(m_size, m_size, red, green, blue);
	}

	Image Wavelet::to_wavelet_image() const
	{
		std::vector<double> red = m_red;
		std::vector<double> green = m_green;
		std::vector<double> blue = m_blue;

		auto minmax = std::minmax_element(red.begin(), red.end());

		double min = *(minmax.first);
		double max = *(minmax.second);

		minmax = std::minmax_element(green.begin(), green.end());
		min = std::min(min, *(minmax.first));
		max = std::max(max, *(minmax.second));

		minmax = std::minmax_element(blue.begin(), blue.end());
		min = std::min(min, *(minmax.first));
		max = std::max(max, *(minmax.second));

		auto scaler = [min, max](double v)
		{
			return (v - min) / (max - min);
		};

		std::transform(red.begin(), red.end(), red.begin(), scaler);
		std::transform(green.begin(), green.end(), green.begin(), scaler);
		std::transform(blue.begin(), blue.end(), blue.begin(), scaler);

		return Image(m_size, m_size, m_red, m_green, m_blue);
	}

	std::vector<char> Wavelet::char_to_bits(char c)
	{
		std::vector<char> result;
		char mask = 1;

		for(size_t i = 0; i < 8; ++i)
		{
			result.push_back((c & mask) >> i);
			mask <<= 1;
		}

		return result;
	}

	char Wavelet::bits_to_char(const std::vector<char>& bits)
	{
		char result = '\0';
		char mask = 1;

		for(int i = 0; i < 8; ++i)
		{
			result |= mask & (bits[i] == 0 ? 0 : 255);
			mask <<= 1;
		}

		return result;
	}

	void Wavelet::hide_text(const std::string& text)
	{
		std::vector<double> all = merge_container_channels();

		//transform text int sequence of `bits`
		std::vector<char> bits;

		for(size_t i = 0; i < text.length(); ++i)
		{
			std::vector<char> b = char_to_bits(text[i]);
			bits.insert(bits.end(), b.begin(), b.end());
		}

		//add text terminator - zero byte
		std::vector<char> zero = char_to_bits('\0');
		bits.insert(bits.end(), zero.begin(), zero.end());

		//for all bits find a place in all channels to write and write
		auto all_iter = all.begin();

		for(auto bit : bits)
		{
			all_iter = std::find_if(all_iter, all.end(), m_checker);

			if(all_iter != all.end())
				*all_iter = bit == 0 ? -TRESHOLD / 2.0 : TRESHOLD / 2.0;

			++all_iter;
		}

		split_container_channels(all);

		//move data from container channels to image channels
		for(size_t i = m_container_size; i < m_size; ++i)
		{
			for(size_t j = m_container_size; j < m_size; ++j)
			{
				auto index = (i - m_container_size) * m_container_size + (j - m_container_size);
				m_red[i * m_size + j] = m_container_red[index];
				m_green[i * m_size + j] = m_container_green[index];
				m_blue[i * m_size + j] = m_container_blue[index];
			}
		}
	}

	std::string Wavelet::read_text() const
	{
		std::vector<double> all = merge_container_channels();

		//collect values thats contain information
		std::vector<double> infos;
		std::copy_if(all.begin(), all.end(), std::back_inserter(infos), m_checker);
		size_t number_of_chars = (infos.size() / 8);
		//transform values int bit sequence
		std::vector<char> bits;
		std::transform(infos.begin(), infos.end(), std::back_inserter(bits), [](double v)
		{
			return v < 0.0 ? 0 : 1;
		});

		//transform bit sequence int character sequence
		std::vector<char> chars;
		auto bits_iterator = bits.begin();
		std::generate_n(std::back_inserter(chars), number_of_chars, [&bits, &bits_iterator]()
		{
			std::vector<char> bits_of_char(bits_iterator, bits_iterator + 8);
			bits_iterator += 8;
			return bits_to_char(bits_of_char);
		});

		auto text_end_iter = std::find(chars.begin(), chars.end(), '\0');

		if(text_end_iter == chars.end())
			return std::string(chars.begin(), chars.end());
		else
			return std::string(chars.begin(), text_end_iter);
	}

	std::vector<double> Wavelet::merge_container_channels() const
	{
		//merge container color channels into one
		std::vector<double> all;

		for(size_t i = 0; i < m_container_size * m_container_size; ++i)
			all.insert(all.end(), {m_container_red[i], m_container_green[i], m_container_blue[i]});
		return all;
	}

	void Wavelet::split_container_channels(const std::vector<double>& all)
	{
		//split all channels into container channels
		for(size_t i = 0; i < m_container_size * m_container_size; ++i)
		{
			m_container_red[i] = all[i * 3];
			m_container_green[i] = all[i * 3 + 1];
			m_container_blue[i] = all[i * 3 + 2];
		}
	}
}

