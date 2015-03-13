#include <helper/logger/dlogger.h>

int main(int argc, char *argv[]) {
    DLogger::registerCallback({DLogger::Type::Error, DLogger::Type::Warning, DLogger::Type::Message},
                              [](QString msg)-> void { printf("%s\n", msg.toStdString().c_str()); fflush(0); });

    LOG_MSG("dDeflect");

    return 0;
}
