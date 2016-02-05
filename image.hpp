#ifndef IMAGE_HPP__
#define IMAGE_HPP__

#include <vector>
#include <tuple>
#include <cairomm/cairomm.h>

namespace Stego
{
	struct Image
	{
		Image() = default;
		Image(const Cairo::RefPtr<Cairo::ImageSurface>& image);
		Image(size_t width,
			  size_t height,
			  const std::vector<double>& red,
			  const std::vector<double>& green,
			  const std::vector<double>& blue);
		Image(const Image&) = default;
		Image& operator=(const Image&) = default;

		std::tuple<double, double, double> psnr(const Image& copy);

		Cairo::RefPtr<Cairo::ImageSurface> to_cairo_image_surface() const;

		size_t m_width;
		size_t m_height;
		std::vector<double> m_red;
		std::vector<double> m_green;
		std::vector<double> m_blue;
	};
}

#endif

