#include <QString>
#include <helper/logger/dlogger.h>
#include <helper/manager/dmanager.h>

/**
 * @brief usage usage function
 */
void usage();

/**
 * @brief handle_args
 * @param manager
 * @param argc
 * @param argv
 * @return
 */
bool handle_args(DManager &manager, int argc, char **argv);

/**
 * @brief main Program entry point
 * @param argc number of arguments
 * @param argv array of arguments
 * @return status
 */
int main(int argc, char **argv) {
  DLogger::registerCallback({DLogger::Type::Error, DLogger::Type::Warning, DLogger::Type::Message},
                            [](QString msg)-> void { printf("%s\n", msg.toStdString().c_str()); fflush(0); });

  LOG_MSG("dDeflect");

  DManager manager;

  if (!handle_args(manager, argc, argv))
    return EXIT_FAILURE;




  return EXIT_SUCCESS;
}

void usage() {
  LOG_MSG("usage: dDeflect [option] [value]");
  LOG_MSG("Options: ");
  LOG_MSG("\t--in:\t\tinput file name");
  LOG_MSG("\t--out:\t\toutput optional filename");
  LOG_MSG("\t--adding-method:\tadding method type");
  LOG_MSG("\t--secure-method:\tsecure method type");
  LOG_MSG("\t--ddhandler:\tdebugger detection handler");
  LOG_MSG("\t--change-x-segment:\texecutable segment of code could be changed");
  LOG_MSG("\t--obfuscate:\tobfuscate binary after secure");
  LOG_MSG("\t--pack:\t\tpack input file with UPX");
  LOG_MSG("\t--show-ddmethods:\tlist all debugger detection methods for specified platform");
  LOG_MSG("\t--show-ddhandlers:\tlist all debugger detection handler for specified platform");
  LOG_MSG("\t--show-adding-methods:\tlist all adding methods for specified platform");
  LOG_MSG("\t--help:\t\thelp");
}

bool handle_args(DManager &manager, int argc, char **argv) {
  int i;
  QString help("--help");
  if (argc == 1) {
      usage();
      return false;
    }

  for (i = 0; i < argc; ++i) {
      if (help == argv[i]) {
          usage();
          return false;
        }
    }

  return true;
}
