// Copyright (C) 2013-2014 Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.


#include <d2/d2_log.h>
#include <config/ccsession.h>
#include <d2/d_controller.h>
#include <exceptions/exceptions.h>
#include <log/logger_support.h>

#include <sstream>

namespace isc {
namespace d2 {

DControllerBasePtr DControllerBase::controller_;

// Note that the constructor instantiates the controller's primary IOService.
DControllerBase::DControllerBase(const char* app_name, const char* bin_name)
    : app_name_(app_name), bin_name_(bin_name),
      verbose_(false), spec_file_name_(""),
      io_service_(new isc::asiolink::IOService()){
}

void
DControllerBase::setController(const DControllerBasePtr& controller) {
    if (controller_) {
        // This shouldn't happen, but let's make sure it can't be done.
        // It represents a programmatic error.
        isc_throw (DControllerBaseError,
                "Multiple controller instances attempted.");
    }

    controller_ = controller;
}

void
DControllerBase::launch(int argc, char* argv[], const bool test_mode) {

    // Step 1 is to parse the command line arguments.
    try {
        parseArgs(argc, argv);
    } catch (const InvalidUsage& ex) {
        usage(ex.what());
        throw; // rethrow it
    }

    // Do not initialize logger here if we are running unit tests. It would
    // replace an instance of unit test specific logger.
    if (!test_mode) {
        // Now that we know what the mode flags are, we can init logging.
        Daemon::loggerInit(bin_name_.c_str(), verbose_);
    }

    LOG_DEBUG(dctl_logger, DBGLVL_START_SHUT, DCTL_STARTING)
              .arg(app_name_).arg(getpid());
    try {
        // Step 2 is to create and initialize the application process object.
        initProcess();
    } catch (const std::exception& ex) {
        LOG_FATAL(dctl_logger, DCTL_INIT_PROCESS_FAIL)
                  .arg(app_name_).arg(ex.what());
        isc_throw (ProcessInitError,
                   "Application Process initialization failed: " << ex.what());
    }

    LOG_DEBUG(dctl_logger, DBGLVL_START_SHUT, DCTL_STANDALONE).arg(app_name_);

    // Step 3 is to load configuration from file.
    int rcode;
    isc::data::ConstElementPtr comment
        = isc::config::parseAnswer(rcode, configFromFile());
    if (rcode != 0) {
        LOG_FATAL(dctl_logger, DCTL_CONFIG_FILE_LOAD_FAIL)
                  .arg(app_name_).arg(comment->stringValue());
        isc_throw (ProcessInitError, "Could Not load configration file: "
                   << comment->stringValue());
    }

    // Everything is clear for launch, so start the application's
    // event loop.
    try {
        runProcess();
    } catch (const std::exception& ex) {
        LOG_FATAL(dctl_logger, DCTL_PROCESS_FAILED)
                  .arg(app_name_).arg(ex.what());
        isc_throw (ProcessRunError,
                   "Application process event loop failed: " << ex.what());
    }

    // All done, so bail out.
    LOG_INFO(dctl_logger, DCTL_STOPPING).arg(app_name_);
}

void
DControllerBase::parseArgs(int argc, char* argv[])
{
    // Iterate over the given command line options. If its a stock option
    // ("s" or "v") handle it here.  If its a valid custom option, then
    // invoke customOption.
    int ch;
    opterr = 0;
    optind = 1;
    std::string opts("vc:" + getCustomOpts());
    while ((ch = getopt(argc, argv, opts.c_str())) != -1) {
        switch (ch) {
        case 'v':
            // Enables verbose logging.
            verbose_ = true;
            break;

        case 'c':
            // config file name
            if (optarg == NULL) {
                isc_throw(InvalidUsage, "configuration file name missing");
            }

            Daemon::init(optarg);
            break;

        case '?': {
            // We hit an invalid option.
            isc_throw(InvalidUsage, "unsupported option: ["
                      << static_cast<char>(optopt) << "] "
                      << (!optarg ? "" : optarg));

            break;
            }

        default:
            // We hit a valid custom option
            if (!customOption(ch, optarg)) {
                // This would be a programmatic error.
                isc_throw(InvalidUsage, " Option listed but implemented?: ["
                          << static_cast<char>(ch) << "] "
                          << (!optarg ? "" : optarg));
            }
            break;
        }
    }

    // There was too much information on the command line.
    if (argc > optind) {
        isc_throw(InvalidUsage, "extraneous command line information");
    }
}

isc::data::ConstElementPtr
DControllerBase::configHandler(isc::data::ConstElementPtr new_config) {
    LOG_DEBUG(dctl_logger, DBGLVL_COMMAND, DCTL_CONFIG_UPDATE)
              .arg(controller_->getAppName()).arg(new_config->str());

    // Invoke the instance method on the controller singleton.
    return (controller_->updateConfig(new_config));
}

// Static callback which invokes non-static handler on singleton
isc::data::ConstElementPtr
DControllerBase::commandHandler(const std::string& command,
                                isc::data::ConstElementPtr args) {

    LOG_DEBUG(dctl_logger, DBGLVL_COMMAND, DCTL_COMMAND_RECEIVED)
        .arg(controller_->getAppName()).arg(command)
        .arg(args ? args->str() : "(no args)");

    // Invoke the instance method on the controller singleton.
    return (controller_->executeCommand(command, args));
}

bool
DControllerBase::customOption(int /* option */, char* /*optarg*/)
{
    // Default implementation returns false.
    return (false);
}

void
DControllerBase::initProcess() {
    LOG_DEBUG(dctl_logger, DBGLVL_START_SHUT, DCTL_INIT_PROCESS).arg(app_name_);

    // Invoke virtual method to instantiate the application process.
    try {
        process_.reset(createProcess());
    } catch (const std::exception& ex) {
        isc_throw(DControllerBaseError, std::string("createProcess failed: ")
                  + ex.what());
    }

    // This is pretty unlikely, but will test for it just to be safe..
    if (!process_) {
        isc_throw(DControllerBaseError, "createProcess returned NULL");
    }

    // Invoke application's init method (Note this call should throw
    // DProcessBaseError if it fails).
    process_->init();
}

isc::data::ConstElementPtr
DControllerBase::configFromFile() {
    isc::data::ConstElementPtr module_config;

    try {
        std::string config_file = getConfigFileName();
        if (config_file.empty()) {
            // Basic sanity check: file name must not be empty.
            isc_throw(BadValue, "JSON configuration file not specified. Please "
                                "use -c command line option.");
        }

        // Read contents of the file and parse it as JSON
        isc::data::ConstElementPtr whole_config =
            isc::data::Element::fromJSONFile(config_file, true);

        // Extract derivation-specific portion of the configuration.
        module_config = whole_config->get(getAppName());
        if (!module_config) {
            isc_throw(BadValue, "Config file " << config_file <<
                                " does not include '" <<
                                 getAppName() << "' entry.");
        }
    } catch (const std::exception& ex) {
        // build an error result
        isc::data::ConstElementPtr error =
            isc::config::createAnswer(1,
                std::string("Configuration parsing failed: ") + ex.what());
        return (error);
    }

    return (updateConfig(module_config));
}


void
DControllerBase::runProcess() {
    LOG_DEBUG(dctl_logger, DBGLVL_START_SHUT, DCTL_RUN_PROCESS).arg(app_name_);
    if (!process_) {
        // This should not be possible.
        isc_throw(DControllerBaseError, "Process not initialized");
    }

    // Invoke the application process's run method. This may throw
    // DProcessBaseError
    process_->run();
}

// Instance method for handling new config
isc::data::ConstElementPtr
DControllerBase::updateConfig(isc::data::ConstElementPtr new_config) {
    return (process_->configure(new_config));
}


// Instance method for executing commands
isc::data::ConstElementPtr
DControllerBase::executeCommand(const std::string& command,
                            isc::data::ConstElementPtr args) {
    // Shutdown is universal.  If its not that, then try it as
    // an custom command supported by the derivation.  If that
    // doesn't pan out either, than send to it the application
    // as it may be supported there.
    isc::data::ConstElementPtr answer;
    if (command.compare(SHUT_DOWN_COMMAND) == 0) {
        answer = shutdownProcess(args);
    } else {
        // It wasn't shutdown, so may be a custom controller command.
        int rcode = 0;
        answer = customControllerCommand(command, args);
        isc::config::parseAnswer(rcode, answer);
        if (rcode == COMMAND_INVALID)
        {
            // It wasn't controller command, so may be an application command.
            answer = process_->command(command, args);
        }
    }

    return (answer);
}

isc::data::ConstElementPtr
DControllerBase::customControllerCommand(const std::string& command,
                                     isc::data::ConstElementPtr /* args */) {

    // Default implementation always returns invalid command.
    return (isc::config::createAnswer(COMMAND_INVALID,
                                      "Unrecognized command: " + command));
}

isc::data::ConstElementPtr
DControllerBase::shutdownProcess(isc::data::ConstElementPtr args) {
    if (process_) {
        return (process_->shutdown(args));
    }

    // Not really a failure, but this condition is worth noting. In reality
    // it should be pretty hard to cause this.
    LOG_WARN(dctl_logger, DCTL_NOT_RUNNING).arg(app_name_);
    return (isc::config::createAnswer(0, "Process has not been initialzed."));
}

void
DControllerBase::usage(const std::string & text)
{
    if (text != "") {
        std::cerr << "Usage error: " << text << std::endl;
    }

    std::cerr << "Usage: " << bin_name_ <<  std::endl
              << "  -c <config file name> : mandatory,"
              <<   " specifies name of configuration file " << std::endl
              << "  -v: optional, verbose output " << std::endl;

    // add any derivation specific usage
    std::cerr << getUsageText() << std::endl;
}

DControllerBase::~DControllerBase() {
}

std::string
DControllerBase::getConfigFileName() {
    return (Daemon::getConfigFile());
}

}; // namespace isc::d2

// Provide an implementation until we figure out a better way to do this.
void
dhcp::Daemon::loggerInit(const char* log_name, bool verbose) {
    setenv("B10_LOCKFILE_DIR_FROM_BUILD", "/tmp", 1);
    setenv("B10_LOGGER_ROOT", log_name, 0);
    setenv("B10_LOGGER_SEVERITY", (verbose ? "DEBUG":"INFO"), 0);
    setenv("B10_LOGGER_DBGLEVEL", "99", 0);
    setenv("B10_LOGGER_DESTINATION",  "stdout", 0);
    isc::log::initLogger();
}

}; // namespace isc
