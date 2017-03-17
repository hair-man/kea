// Copyright (C) 2012-2017 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef CTRL_DHCPV4_SRV_H
#define CTRL_DHCPV4_SRV_H

#include <asiolink/asio_wrapper.h>
#include <asiolink/asiolink.h>
#include <cc/data.h>
#include <cc/command_interpreter.h>
#include <dhcpsrv/timer_mgr.h>
#include <dhcp4/dhcp4_srv.h>

namespace isc {
namespace dhcp {

/// @brief Controlled version of the DHCPv4 server
///
/// This is a class that is responsible for DHCPv4 server being controllable,
/// by reading configuration file from disk.
class ControlledDhcpv4Srv : public isc::dhcp::Dhcpv4Srv {
public:

    /// @brief Constructor
    ///
    /// @param port UDP port to be opened for DHCP traffic
    ControlledDhcpv4Srv(uint16_t port = DHCP4_SERVER_PORT);

    /// @brief Destructor.
    ~ControlledDhcpv4Srv();

    /// @brief Initializes the server.
    ///
    /// Depending on the configuration backend, it establishes msgq session,
    /// reads the JSON file from disk or may perform any other setup
    /// operation. For specific details, see actual implementation in
    /// *_backend.cc
    ///
    /// This method may throw if initialization fails. Exception types may be
    /// specific to used configuration backend.
    void init(const std::string& config_file);

    /// @brief Performs cleanup, immediately before termination
    ///
    /// This method performs final clean up, just before the Dhcpv4Srv object
    /// is destroyed. Currently it is a no-op.
    void cleanup();

    /// @brief Initiates shutdown procedure for the whole DHCPv4 server.
    void shutdown();

    /// @brief Command processor
    ///
    /// This method is uniform for all config backends. It processes received
    /// command (as a string + JSON arguments). Internally, it's just a
    /// wrapper that calls process*Command() methods and catches exceptions
    /// in them.
    ///
    /// Currently supported commands are:
    /// - shutdown
    /// - libreload
    /// - config-reload
    /// - leases-reclaim
    /// ...
    ///
    /// @note It never throws.
    ///
    /// @param command Text representation of the command (e.g. "shutdown")
    /// @param args Optional parameters
    ///
    /// @return status of the command
    static isc::data::ConstElementPtr
    processCommand(const std::string& command, isc::data::ConstElementPtr args);

    /// @brief Configuration processor
    ///
    /// This is a method for handling incoming configuration updates.
    /// This method should be called by all configuration backends when the
    /// server is starting up or when configuration has changed.
    ///
    /// As pointer to this method is used a callback in ASIO used in
    /// ModuleCCSession, it has to be static.
    ///
    /// @param new_config textual representation of the new configuration
    ///
    /// @return status of the config update
    static isc::data::ConstElementPtr
    processConfig(isc::data::ConstElementPtr new_config);

    /// @brief Configuration checker
    ///
    /// This is a method for checking incoming configuration.
    ///
    /// @param new_config textual representation of the new configuration
    ///
    /// @return status of the config check
    isc::data::ConstElementPtr
    checkConfig(isc::data::ConstElementPtr new_config);

    /// @brief Returns pointer to the sole instance of Dhcpv4Srv
    ///
    /// @return server instance (may return NULL, if called before server is spawned)
    static ControlledDhcpv4Srv* getInstance() {
        return (server_);
    }


private:
    /// @brief Callback that will be called from iface_mgr when data
    /// is received over control socket.
    ///
    /// This static callback method is called from IfaceMgr::receive6() method,
    /// when there is a new command or configuration sent over control socket
    /// (that was sent from some yet unspecified sender).
    static void sessionReader(void);

    /// @brief Handler for processing 'shutdown' command
    ///
    /// This handler processes shutdown command, which initializes shutdown
    /// procedure.
    /// @param command (parameter ignored)
    /// @param args (parameter ignored)
    ///
    /// @return status of the command
    isc::data::ConstElementPtr
    commandShutdownHandler(const std::string& command,
                           isc::data::ConstElementPtr args);

    /// @brief Handler for processing 'libreload' command
    ///
    /// This handler processes libreload command, which unloads all hook
    /// libraries and reloads them.
    ///
    /// @param command (parameter ignored)
    /// @param args (parameter ignored)
    ///
    /// @return status of the command
    isc::data::ConstElementPtr
    commandLibReloadHandler(const std::string& command,
                            isc::data::ConstElementPtr args);

    /// @brief Handler for processing 'config-reload' command
    ///
    /// This handler processes config-reload command, which processes
    /// configuration specified in args parameter.
    ///
    /// @param command (parameter ignored)
    /// @param args configuration to be processed
    ///
    /// @return status of the command
    isc::data::ConstElementPtr
    commandConfigReloadHandler(const std::string& command,
                               isc::data::ConstElementPtr args);

    /// @brief handler for processing 'get-config' command
    ///
    /// This handler processes get-config command, which retrieves
    /// the current configuration and returns it in response.
    ///
    /// @param command (ignored)
    /// @param args (ignored)
    /// @return current configuration wrapped in a response
    isc::data::ConstElementPtr
    commandConfigGetHandler(const std::string& command,
                            isc::data::ConstElementPtr args);

    /// @brief handler for processing 'write-config' command
    ///
    /// This handle processes write-config comamnd, which writes the
    /// current configuration to disk. This command takes one optional
    /// parameter called filename. If specified, the current configuration
    /// will be written to that file. If not specified, the file used during
    /// Kea start-up will be used. To avoid any exploits, the path is
    /// always relative and .. is not allowed in the filename. This is
    /// a security measure against exploiting file writes remotely.
    ///
    /// @param command (ignored)
    /// @param args may contain optional string argument filename
    /// @return status of the configuration file write
    isc::data::ConstElementPtr
    commandConfigWriteHandler(const std::string& command,
                              isc::data::ConstElementPtr args);

    /// @brief handler for processing 'set-config' command
    ///
    /// This handler processes set-config command, which processes
    /// configuration specified in args parameter.
    /// @param command (parameter ignored)
    /// @param args configuration to be processed. Expected format:
    /// map containing Dhcp4 map that contains DHCPv4 server configuration.
    /// May also contain Logging map that specifies logging configuration.
    ///
    /// @return status of the command
    isc::data::ConstElementPtr
    commandSetConfigHandler(const std::string& command,
                            isc::data::ConstElementPtr args);

    /// @brief handler for processing 'config-test' command
    ///
    /// This handler processes config-test command, which checks
    /// configuration specified in args parameter.
    /// @param command (parameter ignored)
    /// @param args configuration to be checked. Expected format:
    /// map containing Dhcp4 map that contains DHCPv4 server configuration.
    /// May also contain Logging map that specifies logging configuration.
    ///
    /// @return status of the command
    isc::data::ConstElementPtr
    commandConfigTestHandler(const std::string& command,
                             isc::data::ConstElementPtr args);

    /// @Brief handler for processing 'version-get' command
    ///
    /// This handler processes version-get command, which returns
    /// over the control channel the -v and -V command line arguments.
    /// @param command (parameter ignored)
    /// @param args (parameter ignored) 
    ///
    /// @return status of the command with the version in text and
    /// the extended version in arguments.
    isc::data::ConstElementPtr
    commandVersionGetHandler(const std::string& command,
                             isc::data::ConstElementPtr args);

    /// @brief handler for processing 'build-report' command
    ///
    /// This handler processes build-report command, which returns
    /// over the control channel the -W command line argument.
    /// @param command (parameter ignored)
    /// @param args (parameter ignored) 
    ///
    /// @return status of the command with the config report
    isc::data::ConstElementPtr
    commandBuildReportHandler(const std::string& command,
                              isc::data::ConstElementPtr args);

    /// @brief Handler for processing 'leases-reclaim' command
    ///
    /// This handler processes leases-reclaim command, which triggers
    /// the leases reclamation immediately.
    /// No limit for processing time or number of processed leases applies.
    ///
    /// @param command (parameter ignored)
    /// @param args arguments map { "remove": <bool> }
    ///        if true a lease is removed when it is reclaimed,
    ///        if false its state is changed to "expired-reclaimed".
    ///
    /// @return status of the command (should be success unless args
    ///         was not a Bool Element).
    isc::data::ConstElementPtr
    commandLeasesReclaimHandler(const std::string& command,
                                isc::data::ConstElementPtr args);

    /// @brief Reclaims expired IPv4 leases and reschedules timer.
    ///
    /// This is a wrapper method for @c AllocEngine::reclaimExpiredLeases4.
    /// It reschedules the timer for leases reclamation upon completion of
    /// this method.
    ///
    /// @param max_leases Maximum number of leases to be reclaimed.
    /// @param timeout Maximum amount of time that the reclamation routine
    /// may be processing expired leases, expressed in milliseconds.
    /// @param remove_lease A boolean value indicating if the lease should
    /// be removed when it is reclaimed (if true) or it should be left in the
    /// database in the "expired-reclaimed" state (if false).
    /// @param max_unwarned_cycles A number of consecutive processing cycles
    /// of expired leases, after which the system issues a warning if there
    /// are still expired leases in the database. If this value is 0, the
    /// warning is never issued.
    void reclaimExpiredLeases(const size_t max_leases, const uint16_t timeout,
                              const bool remove_lease,
                              const uint16_t max_unwarned_cycles);

    /// @brief Deletes reclaimed leases and reschedules the timer.
    ///
    /// This is a wrapper method for @c AllocEngine::deleteExpiredReclaimed4.
    /// It reschedules the timer for leases reclamation upon completion of
    /// this method.
    ///
    /// @param secs Minimum number of seconds after which a lease can be
    /// deleted.
    void deleteExpiredReclaimedLeases(const uint32_t secs);

    /// @brief Static pointer to the sole instance of the DHCP server.
    ///
    /// This is required for config and command handlers to gain access to
    /// the server
    static ControlledDhcpv4Srv* server_;

    /// @brief IOService object, used for all ASIO operations.
    isc::asiolink::IOService io_service_;

    /// @brief Instance of the @c TimerMgr.
    ///
    /// Shared pointer to the instance of timer @c TimerMgr is held here to
    /// make sure that the @c TimerMgr outlives instance of this class.
    TimerMgrPtr timer_mgr_;
};

}; // namespace isc::dhcp
}; // namespace isc

#endif
