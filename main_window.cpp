#include "main_window.hpp"

#include <iostream>

namespace Stego
{
	MainWindow::MainWindow() :
		Gtk::Window(),
		m_box(Gtk::ORIENTATION_VERTICAL),
		m_toolbar(),
		m_create(Gtk::Stock::NEW),
		m_open(Gtk::Stock::OPEN),
		m_save(Gtk::Stock::SAVE),
		m_workspace(Gtk::ORIENTATION_HORIZONTAL),
		m_text_window(),
		m_text(),
		m_first(),
		m_statusbar()
	{
		set_title("Wavelet steganography example");
		set_has_resize_grip(false);
		set_size_request(200, 200);
		add(m_box);
		m_box.pack_start(m_toolbar, false, false, 0);
		m_box.pack_start(m_workspace);
		m_box.pack_end(m_statusbar, false, false, 0);

		m_toolbar.append(m_create, sigc::mem_fun(*this, &MainWindow::create_file));
		m_toolbar.append(m_open, sigc::mem_fun(*this, &MainWindow::open_file));
		m_toolbar.append(m_save, sigc::mem_fun(*this, &MainWindow::save_file));
		m_first.signal_draw().connect(sigc::mem_fun(*this, &MainWindow::first_draw));

		m_toolbar.set_toolbar_style(Gtk::TOOLBAR_BOTH);
		m_workspace.pack_start(m_first);
		m_workspace.pack_start(m_text_window);
		m_text_window.add(m_text);

		m_workspace.set_spacing(10);

		m_save.set_sensitive(false);

		show_all();

		m_current_directory = Glib::get_current_dir();
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::create_file()
	{
		std::string filename = run_open_file_dialog("Select png file to create stegocontener", Gtk::FILE_CHOOSER_ACTION_OPEN);

		if(filename != "")
		{
			m_png = Cairo::ImageSurface::create_from_png(filename);

			if(!m_png)
			{
				Gtk::MessageDialog err(*this, "Open file failed", false, Gtk::MESSAGE_ERROR);
				err.set_secondary_text("Open file failed");
				err.run();
			}
			else
			{
				switch(m_png->get_format())
				{
					case Cairo::FORMAT_ARGB32:
						std::cout << "Image format ARGB32\n";
						break;

					case Cairo::FORMAT_RGB24:
						std::cout << "Image format RGB24\n";
						break;

					case Cairo::FORMAT_A8:
						std::cout << "Image format A8\n";
						break;

					case Cairo::FORMAT_A1:
						std::cout << "Image format A1\n";
						break;

					case Cairo::FORMAT_RGB16_565:
						std::cout << "Image format RGB16_565\n";
						break;
				}

				m_wavelet = Wavelet(Image(m_png));

				std::ostringstream ostr;
				ostr << "Size: " << m_png->get_width() << "x" << m_png->get_height() << ", 'bits' to write: " << m_wavelet.count_bytes_to_write();

				m_statusbar.push(ostr.str());

				m_first.set_size_request(m_png->get_width(), m_png->get_height());
				m_first.show();
				m_text_window.set_size_request(m_png->get_width(), m_png->get_height());
				m_text_window.show();
				m_save.set_sensitive(true);
			}
		}
	}


	void MainWindow::open_file()
	{
		std::cout << "MainWindow::open_file()\n";

		std::string filename = run_open_file_dialog("Select stegocontener to read", Gtk::FILE_CHOOSER_ACTION_OPEN);

		if(filename != "")
		{
			m_png = Cairo::ImageSurface::create_from_png(filename);


			if(!m_png)
			{
				Gtk::MessageDialog err(*this, "Open file failed", false, Gtk::MESSAGE_ERROR);
				err.set_secondary_text("Open file failed");
				err.run();
			}
			else
			{

				Image image(m_png);
				Wavelet wavelet(image);

				std::string test_text = wavelet.read_text();
				// std::cout<<"    READED TEXT : '"<<test_text<<"'\n";

				std::ostringstream msg;
				msg << "Size: " << m_png->get_width() << "x" << m_png->get_height() << ", readed " << test_text.size() << " characters";
				m_statusbar.push(msg.str());

				m_text.get_buffer()->set_text(test_text);

				m_first.set_size_request(m_png->get_width(), m_png->get_height());
				m_first.show();
				m_text_window.set_size_request(m_png->get_width(), m_png->get_height());
				m_text_window.show();
				m_save.set_sensitive(true);
			}
		}
	}

	void MainWindow::save_file()
	{
		std::cout << "MainWindow::save_file()\n";

		if(m_png)
		{
			//dane są w obrazie teraz można je zapisać
			//więc dialog i te sprawy
			std::string filename = run_open_file_dialog("Select file to save stegocontener", Gtk::FILE_CHOOSER_ACTION_SAVE);

			std::string test_text = m_text.get_buffer()->get_text();
			m_wavelet.hide_text(test_text);

			if(filename != "")
			{
				std::cout << "Saving image\n";
				Cairo::RefPtr<Cairo::ImageSurface> surface = m_wavelet.to_image().to_cairo_image_surface();
				surface->write_to_png(filename);
			}
		}
	}

	bool MainWindow::first_draw(const Cairo::RefPtr<Cairo::Context>& cr)
	{
		Gtk::Allocation allocation = m_first.get_allocation();
		cr->set_source_rgba(0, 0, 0, 1);
		cr->rectangle(0, 0, allocation.get_width(), allocation.get_height());
		cr->fill();

		if(m_png)
		{
			cr->set_source(m_png, 0, 0);
			cr->paint();
		}

		return true;
	}

	std::string MainWindow::run_open_file_dialog(const std::string& title, Gtk::FileChooserAction action)
	{
		Gtk::FileChooserDialog dialog(title, action);

		dialog.set_transient_for(*this);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
		filter->set_name("PNG files");
		filter->add_mime_type("image/png");
		dialog.add_filter(filter);
		int result = dialog.run();

		if(m_current_directory != "")
			dialog.set_current_folder(m_current_directory);

		if(result == Gtk::RESPONSE_OK)
		{
			m_current_directory = dialog.get_current_folder();
			return dialog.get_filename();
		}

		return "";
	}
}

