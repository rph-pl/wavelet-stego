#include <gtkmm.h>

#include "main_window.hpp"

int main(int argc, char* argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv);
	Stego::MainWindow win;
	return app->run(win);
}

