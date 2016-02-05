#include "image.hpp"

#include <iostream>

namespace Stego
{
	Image::Image(const Cairo::RefPtr<Cairo::ImageSurface>& image) :
		m_width(image->get_width()),
		m_height(image->get_height())
	{
		unsigned char* data = image->get_data();
		size_t length = image->get_width() * image->get_height();

		for(size_t i = 0; i < length; ++i)
		{
			m_blue.push_back(static_cast<double>(data[i * 4]));
			m_green.push_back(static_cast<double>(data[i * 4 + 1]));
			m_red.push_back(static_cast<double>(data[i * 4 + 2]));
		}
	}

	Image::Image(size_t width,
				 size_t height,
				 const std::vector<double>& red,
				 const std::vector<double>& green,
				 const std::vector<double>& blue) :
		m_width(width),
		m_height(height),
		m_red(red),
		m_green(green),
		m_blue(blue)
	{
	}

	Cairo::RefPtr<Cairo::ImageSurface> Image::to_cairo_image_surface() const
	{
		Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, m_width, m_height);

		unsigned char* rgb = surface->get_data();

		for(size_t i = 0; i < m_width * m_height; ++i)
		{
			rgb[i * 4] = (m_blue[i]);
			rgb[i * 4 + 1] = (m_green[i]);
			rgb[i * 4 + 2] = (m_red[i]);
			rgb[i * 4 + 3] = 255;
		}

		return surface;
	}

	std::tuple<double, double, double> Image::psnr(const Image& copy)
	{
		double red = 0.0;
		double green = 0.0;
		double blue = 0.0;

		for(size_t i = 0; i < m_width * m_height; ++i)
		{
			red += (m_red[i] - copy.m_red[i]) * (m_red[i] - copy.m_red[i]);
			green += (m_green[i] - copy.m_green[i]) * (m_green[i] - copy.m_green[i]);
			blue += (m_blue[i] - copy.m_blue[i]) * (m_blue[i] - copy.m_blue[i]);
		}

		red /= (m_width * m_height);
		green /= (m_width * m_height);
		blue /= (m_width * m_height);

		red = 10.0 * log10((255 * 255) / red);
		green = 10.0 * log10((255 * 255) / green);
		blue = 10.0 * log10((255 * 255) / blue);

		return std::make_tuple(red, green, blue);
	}
}

