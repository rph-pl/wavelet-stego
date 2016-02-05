#ifndef MAIN_WINDOW_HPP__
#define MAIN_WINDOW_HPP__

#include <string>
#include <gtkmm.h>

#include "image.hpp"
#include "wavelet.hpp"

namespace Stego
{
	class MainWindow : public Gtk::Window
	{
		public:
			MainWindow();
			virtual ~MainWindow();

		private:
			void open_file();
			void create_file();
			void save_file();
			bool first_draw(const Cairo::RefPtr<Cairo::Context>& cr);

			std::string run_open_file_dialog(const std::string& title, Gtk::FileChooserAction action);
		private:
			Gtk::Box m_box;
			Gtk::Toolbar m_toolbar;
			Gtk::ToolButton m_create;
			Gtk::ToolButton m_open;
			Gtk::ToolButton m_save;
			Gtk::Box m_workspace;
			Gtk::ScrolledWindow m_text_window;
			Gtk::TextView m_text;
			Gtk::DrawingArea m_first;
			Gtk::Statusbar m_statusbar;
			Cairo::RefPtr<Cairo::ImageSurface> m_png;
			std::string m_current_directory;

			Wavelet m_wavelet;
	};
}

#endif

