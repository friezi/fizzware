#ifndef MAIN_HPP
#define MAIN_HPP

#include <syslog.h>
#include <netdb.h>
#include <stdlib.h>
#include <libgen.h>
#include <fztooltempl/exception.hpp>
#include <fztooltempl/cmdlparser.hpp>
#include "msgqueueregdaemon.hpp"

#define L_OPT (1L<<0)
#define QU_OPT (1L<<1)
#define STREAM_PATH "/var/run/msgqrd_socket"
#define VERSION "1.6"

void help_exit(cmdl::CmdlParser& parser);
void version_exit();
std::string setCmdlOptions(cmdl::CmdlParser& parser);

#endif
