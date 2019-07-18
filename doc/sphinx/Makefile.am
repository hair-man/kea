EXTRA_DIST=

if GENERATE_DOCS

sphinxbuilddir=$(builddir)/_build

sphinxopts=
sphinxopts+=-v
sphinxopts+=-E
sphinxopts+=-a
sphinxopts+=-j auto
sphinxopts+=-c "${abs_srcdir}"
sphinxopts+=-D release="@PACKAGE_VERSION@"
sphinxopts+=-D version="@PACKAGE_VERSION@"


static_sources=
static_sources+=static/kea-imageonly-100bw.png
static_sources+=static/kea-logo-200.png
static_sources+=static/kea.css

# ARM
rst_arm_sources=
rst_arm_sources+=arm/admin.rst
rst_arm_sources+=arm/agent.rst
rst_arm_sources+=arm/classify.rst
rst_arm_sources+=arm/config-backend.rst
rst_arm_sources+=arm/config.rst
rst_arm_sources+=arm/congestion-handling.rst
rst_arm_sources+=arm/ctrl-channel.rst
rst_arm_sources+=arm/ddns.rst
rst_arm_sources+=arm/dhcp4-srv.rst
rst_arm_sources+=arm/dhcp6-srv.rst
rst_arm_sources+=arm/hammer.rst
rst_arm_sources+=arm/hooks-cb-cmds.rst
rst_arm_sources+=arm/hooks-class-cmds.rst
rst_arm_sources+=arm/hooks-ha.rst
rst_arm_sources+=arm/hooks-host-cache.rst
rst_arm_sources+=arm/hooks-lease-cmds.rst
rst_arm_sources+=arm/hooks-radius.rst
rst_arm_sources+=arm/hooks.rst
rst_arm_sources+=arm/hooks-stat-cmds.rst
rst_arm_sources+=arm/install.rst
rst_arm_sources+=index.rst
rst_arm_sources+=arm/intro.rst
rst_arm_sources+=arm/keactrl.rst
rst_arm_sources+=arm/lease-expiration.rst
rst_arm_sources+=arm/lfc.rst
rst_arm_sources+=arm/logging.rst
rst_arm_sources+=arm/netconf.rst
rst_arm_sources+=arm/quickstart.rst
rst_arm_sources+=arm/shell.rst
rst_arm_sources+=arm/stats.rst

main_sources=$(rst_arm_sources) conf.py $(static_sources)

# mans
rst_man_sources=
rst_man_sources+=man/kea-admin.8.rst
rst_man_sources+=man/kea-ctrl-agent.8.rst
rst_man_sources+=man/kea-dhcp4.8.rst
rst_man_sources+=man/kea-dhcp6.8.rst
rst_man_sources+=man/kea-dhcp-ddns.8.rst
rst_man_sources+=man/kea-lfc.8.rst
if HAVE_SYSREPO
rst_man_sources+=man/kea-netconf.8.rst
endif
rst_man_sources+=man/kea-shell.8.rst
rst_man_sources+=man/keactrl.8.rst
if PERFDHCP
rst_man_sources+=man/perfdhcp.8.rst
endif
man_sources=$(rst_man_sources) conf.py
man8s=$(foreach rst,$(rst_man_sources), $(sphinxbuilddir)/$(basename $(rst)))

EXTRA_DIST += $(main_sources) $(man_sources) mes2doc.py


# list of messages files that are used to generate kea-messages.rst and then kea-messages.pdf
mes_files=
mes_files+=$(top_srcdir)/src/hooks/dhcp/mysql_cb/mysql_cb_messages.mes
mes_files+=$(top_srcdir)/src/hooks/dhcp/lease_cmds/lease_cmds_messages.mes
mes_files+=$(top_srcdir)/src/hooks/dhcp/high_availability/ha_messages.mes
mes_files+=$(top_srcdir)/src/hooks/dhcp/stat_cmds/stat_cmds_messages.mes
mes_files+=$(top_srcdir)/src/hooks/dhcp/user_chk/user_chk_messages.mes
mes_files+=$(top_srcdir)/src/lib/config/config_messages.mes
mes_files+=$(top_srcdir)/src/lib/hooks/hooks_messages.mes
mes_files+=$(top_srcdir)/src/lib/dhcpsrv/dhcpsrv_messages.mes
mes_files+=$(top_srcdir)/src/lib/dhcpsrv/alloc_engine_messages.mes
mes_files+=$(top_srcdir)/src/lib/dhcpsrv/hosts_messages.mes
mes_files+=$(top_srcdir)/src/lib/http/http_messages.mes
mes_files+=$(top_srcdir)/src/lib/dhcp_ddns/dhcp_ddns_messages.mes
mes_files+=$(top_srcdir)/src/lib/database/db_messages.mes
mes_files+=$(top_srcdir)/src/lib/log/log_messages.mes
mes_files+=$(top_srcdir)/src/lib/log/logimpl_messages.mes
mes_files+=$(top_srcdir)/src/lib/log/tests/log_test_messages.mes
mes_files+=$(top_srcdir)/src/lib/process/process_messages.mes
mes_files+=$(top_srcdir)/src/lib/asiodns/asiodns_messages.mes
mes_files+=$(top_srcdir)/src/lib/eval/eval_messages.mes
mes_files+=$(top_srcdir)/src/bin/dhcp4/dhcp4_messages.mes
mes_files+=$(top_srcdir)/src/bin/agent/ca_messages.mes
mes_files+=$(top_srcdir)/src/bin/d2/d2_messages.mes
mes_files+=$(top_srcdir)/src/bin/dhcp6/dhcp6_messages.mes
mes_files+=$(top_srcdir)/src/bin/lfc/lfc_messages.mes
if HAVE_SYSREPO
mes_files+=$(top_srcdir)/src/bin/netconf/netconf_messages.mes
endif

# list of api files that are used to generate api.rst
api_files=
api_files+=api/build-report.json
api_files+=api/cache-clear.json
api_files+=api/cache-get.json
api_files+=api/cache-get-by-id.json
api_files+=api/cache-insert.json
api_files+=api/cache-load.json
api_files+=api/cache-remove.json
api_files+=api/cache-size.json
api_files+=api/cache-write.json
api_files+=api/class-add.json
api_files+=api/class-del.json
api_files+=api/class-get.json
api_files+=api/class-list.json
api_files+=api/class-update.json
api_files+=api/config-get.json
api_files+=api/config-reload.json
api_files+=api/config-set.json
api_files+=api/config-test.json
api_files+=api/config-write.json
api_files+=api/dhcp-disable.json
api_files+=api/dhcp-enable.json
api_files+=api/ha-continue.json
api_files+=api/ha-heartbeat.json
api_files+=api/ha-scopes.json
api_files+=api/ha-sync.json
api_files+=api/lease4-add.json
api_files+=api/lease4-del.json
api_files+=api/lease4-get-all.json
api_files+=api/lease4-get.json
api_files+=api/lease4-update.json
api_files+=api/lease4-wipe.json
api_files+=api/lease6-add.json
api_files+=api/lease6-del.json
api_files+=api/lease6-get-all.json
api_files+=api/lease6-get.json
api_files+=api/lease6-update.json
api_files+=api/lease6-wipe.json
api_files+=api/leases-reclaim.json
api_files+=api/libreload.json
api_files+=api/list-commands.json
api_files+=api/network4-add.json
api_files+=api/network4-del.json
api_files+=api/network4-get.json
api_files+=api/network4-list.json
api_files+=api/network4-subnet-add.json
api_files+=api/network4-subnet-del.json
api_files+=api/network6-add.json
api_files+=api/network6-del.json
api_files+=api/network6-get.json
api_files+=api/network6-list.json
api_files+=api/network6-subnet-add.json
api_files+=api/network6-subnet-del.json
api_files+=api/remote-global-parameter4-del.json
api_files+=api/remote-global-parameter4-get-all.json
api_files+=api/remote-global-parameter4-get.json
api_files+=api/remote-global-parameter4-set.json
api_files+=api/remote-global-parameter6-del.json
api_files+=api/remote-global-parameter6-get-all.json
api_files+=api/remote-global-parameter6-get.json
api_files+=api/remote-global-parameter6-set.json
api_files+=api/remote-network4-del.json
api_files+=api/remote-network4-get.json
api_files+=api/remote-network4-list.json
api_files+=api/remote-network4-set.json
api_files+=api/remote-network6-del.json
api_files+=api/remote-network6-get.json
api_files+=api/remote-network6-list.json
api_files+=api/remote-network6-set.json
api_files+=api/remote-option-def4-del.json
api_files+=api/remote-option-def4-get-all.json
api_files+=api/remote-option-def4-get.json
api_files+=api/remote-option-def4-set.json
api_files+=api/remote-option-def6-del.json
api_files+=api/remote-option-def6-get-all.json
api_files+=api/remote-option-def6-get.json
api_files+=api/remote-option-def6-set.json
api_files+=api/remote-option4-global-del.json
api_files+=api/remote-option4-global-get-all.json
api_files+=api/remote-option4-global-get.json
api_files+=api/remote-option4-global-set.json
api_files+=api/remote-option6-global-del.json
api_files+=api/remote-option6-global-get-all.json
api_files+=api/remote-option6-global-get.json
api_files+=api/remote-option6-global-set.json
api_files+=api/remote-subnet4-del-by-id.json
api_files+=api/remote-subnet4-del-by-prefix.json
api_files+=api/remote-subnet4-get-by-id.json
api_files+=api/remote-subnet4-get-by-prefix.json
api_files+=api/remote-subnet4-list.json
api_files+=api/remote-subnet4-set.json
api_files+=api/remote-subnet6-del-by-id.json
api_files+=api/remote-subnet6-del-by-prefix.json
api_files+=api/remote-subnet6-get-by-id.json
api_files+=api/remote-subnet6-get-by-prefix.json
api_files+=api/remote-subnet6-list.json
api_files+=api/remote-subnet6-set.json
api_files+=api/reservation-add.json
api_files+=api/reservation-del.json
api_files+=api/reservation-get.json
api_files+=api/reservation-get-all.json
api_files+=api/reservation-get-page.json
api_files+=api/shutdown.json
api_files+=api/statistic-get-all.json
api_files+=api/statistic-get.json
api_files+=api/statistic-remove-all.json
api_files+=api/statistic-remove.json
api_files+=api/statistic-reset-all.json
api_files+=api/statistic-reset.json
api_files+=api/stat-lease4-get.json
api_files+=api/stat-lease6-get.json
api_files+=api/subnet4-add.json
api_files+=api/subnet4-del.json
api_files+=api/subnet4-get.json
api_files+=api/subnet4-list.json
api_files+=api/subnet4-update.json
api_files+=api/subnet6-add.json
api_files+=api/subnet6-del.json
api_files+=api/subnet6-get.json
api_files+=api/subnet6-list.json
api_files+=api/subnet6-update.json
api_files+=api/version-get.json

EXTRA_DIST += $(api_files)


all: html mans pdf

kea-messages.rst: $(mes_files) mes2doc.py
	$(srcdir)/mes2doc.py -o $@ $(mes_files)


api.rst: $(api_files) api2doc.py
	$(srcdir)/api2doc.py -o $@ $(api_files)


pdf: $(main_sources) api.rst kea-messages.rst
	$(SPHINXBUILD) -M latexpdf $(srcdir) $(sphinxbuilddir) $(sphinxopts)

html: $(main_sources) api.rst kea-messages.rst
	$(SPHINXBUILD) -M html $(srcdir) $(sphinxbuilddir) $(sphinxopts)

$(man8s): mans

mans: $(man_sources)
	$(SPHINXBUILD) -M man $(srcdir) $(sphinxbuilddir) $(sphinxopts)

install-data-local:
	mkdir -p $(DESTDIR)$(docdir)
	cp -r $(sphinxbuilddir)/html $(DESTDIR)$(docdir)
	${INSTALL_DATA} $(sphinxbuilddir)/latex/kea-arm.pdf $(DESTDIR)$(docdir)
	${INSTALL_DATA} $(sphinxbuilddir)/latex/kea-messages.pdf $(DESTDIR)$(docdir)
	${MKDIR_P} ${DESTDIR}${mandir}/man8
	${INSTALL_DATA} $(man8s) ${DESTDIR}${mandir}/man8/

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)

clean-local:
	rm -rf $(sphinxbuilddir)
	rm -f api.rst kea-messages.rst

.PHONY: all pdf html mans


endif

# There are several steps needed to document new API command:
#
# 1. edit docgen/cmds-list and add the new command
# 2. ./configure --enable-generate-docs
# 3. make - you need to build the sources first, am afraid. The reason why you
#    need to do this is that the tool kea-docgen depends on libkea-cc as it
#    loads JSON files. This means that the libs need to be built first.
# 4. (optional) run: make templates
#    This will go through the list of commands listed in cmds-list
#    and will check if there are corresponding JSON files in api/name.json
#    If the file is missing, a new JSON will be created using template.
#    If you dislike this generator, you can always use api/_template.json
#    and copy it over under the name of a new command.
# 5. Edit api/command-name.json. If the command is provided by the daemon
#    out of its own (and not via hook), simply delete the hook entry.
#    If you don't want to provide command syntax (cmd-syntax key),
#    any comments about the syntax (cmd-comment key) or response syntax
#    (resp-syntax) or any comment about response (resp-comment), simply
#    remove those unused keys. The generator will attempt to generate
#    boilerplates for it.
# 6. Generate api.xml: make api
# 7. Rebuild User's Guide as usual: make guide

# This target will generate templates. There's no need to run it, unless
# new commands have been added or there are existing commands that are
# still not documented.
templates: docgen
	docgen/generate-templates docgen/cmds-list

# This will generate the api.xml file using docgen generator. It will
# read the JSON files from api/ directory. Make sure they're up to date.
api: docgen
	docgen/kea-docgen api/*.json

# This convenience target makes sure the docgen tool is built properly
docgen:
	$(MAKE) -C docgen