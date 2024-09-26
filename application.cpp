
#include "application.h"

Application *app = nullptr;

Application::Application(int &argc, char **argv)
    : QApplication{argc, argv}
{    
    app = this;
    m_db = new Database(this);
}

