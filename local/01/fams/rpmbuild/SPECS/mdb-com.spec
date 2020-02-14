# COPYRIGHT 2017 FUJITSU LIMITED
# @file        MAPDB SPEC file
# @brief       MAPDB rpm Configuration file
# @author      wada
# @date        2016/12/15

%define name mdb-com
%define version ###Ver###
%define USERNAME mapdb
%define USERGRUP mapdb
%define debug_package %{nil}
%define deploy_path /opt/FJSVmapdb

Name:             %{name}
Version:          %{version}
Release:          ###Rev###%{?dist}
Summary:          dynamicDB Service.
Group:            System Environment/Base
URL:              http://www.fujitsu.com/
Source0:          %{name}-%{version}.tar.gz
BuildArch:        x86_64
License:          FUJITSU LIMITED

%description
MapDB CommonLibrary.

%prep
%setup -n mdbcommon

%build
cd opt/FJSVmapdb
%{__make}
pwd
%install
%{__cp} -a {dev,etc,usr,var} ${RPM_BUILD_ROOT}
%{__mkdir_p} ${RPM_BUILD_ROOT}/opt/FJSVmapdb/{etc,lib,script}
%{__cp} -a opt/FJSVmapdb/etc/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/etc
%{__cp} -a opt/FJSVmapdb/lib/build/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/lib
%{__cp} -a opt/FJSVmapdb/script/build/bin/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/script
%{__cp} -a opt/FJSVmapdb/script/script/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/script/

%{__mkdir_p} ${RPM_BUILD_ROOT}%{_sysconfdir}/cron.d
pwd
cd ${RPM_BUILD_ROOT}
pwd
cd ${RPM_BUILD_ROOT}%{_sysconfdir}
pwd

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && %{__rm} -rf "${RPM_BUILD_ROOT}"
pwd
%post
/sbin/ldconfig
%{__chown} %{USERNAME}:%{USERGRUP} /opt/
%{__chown} %{USERNAME}:%{USERGRUP} -R %{deploy_path}
%{__chmod} 770 -R %{deploy_path}
touch /var/log/application
%{__chown} %{USERNAME}:%{USERGRUP} /var/log/application
%{__chmod} 660 /var/log/application
%{__mkdir_p} /var/log/mapdb
%{__mkdir_p} /var/log/mapdbtmp
%{__chown} %{USERNAME}:%{USERGRUP} /var/log/mapdb
%{__chown} %{USERNAME}:%{USERGRUP} /var/log/mapdbtmp
%{__chmod} 770 /var/log/mapdb
%{__chmod} 770 /var/log/mapdbtmp
pwd
%postun
/sbin/ldconfig
pwd
%pre
%preun

%files
%defattr(660,mapdb,mapdb,770)
/dev/mapdb
/etc/httpd/conf.d/deflate.conf
/etc/httpd/conf.d/mpm_event.conf
/etc/ld.so.conf.d/mapdb.conf
%attr(644, root, root) /etc/logrotate.d/mapdb
/etc/profile.d/mapdb.sh
/etc/rsyslog.d/mapdb.conf
/etc/security/limits.d/91-core.conf
/etc/security/limits.d/92-sigpending.conf
/etc/security/limits.d/99-mapdb.conf
/opt/FJSVmapdb/etc/dynamic_management_db.conf
/opt/FJSVmapdb/etc/debug_level.conf
/opt/FJSVmapdb/etc/debug_log_rotate.conf
/opt/FJSVmapdb/etc/mdbcomcron
#/opt/FJSVmapdb/etc/delivery_management_db.conf
#/opt/FJSVmapdb/etc/history_management_db.conf
/opt/FJSVmapdb/etc/syslog.conf
/opt/FJSVmapdb/lib/libmdbproc.so
/opt/FJSVmapdb/lib/libmdbconf.so
/opt/FJSVmapdb/lib/libmdbutil.so
/opt/FJSVmapdb/lib/libmdbhttp.so
/opt/FJSVmapdb/lib/libmdbsig.so
/opt/FJSVmapdb/lib/libmdbcrypto.so
/opt/FJSVmapdb/lib/libmdblog.so
/opt/FJSVmapdb/lib/libmdbshm.so
/opt/FJSVmapdb/lib/libmdbthread.so
/opt/FJSVmapdb/lib/libmdbjson.so
/opt/FJSVmapdb/lib/libmdbdatabase.so
/opt/FJSVmapdb/lib/libmdbipc.so
/opt/FJSVmapdb/lib/libmdbinout.so
/opt/FJSVmapdb/lib/libmdbyaml.so
%attr(770, mapdb, mapdb) /opt/FJSVmapdb/script/mdbdbgloginit
%attr(770, mapdb, mapdb) /opt/FJSVmapdb/script/mdbdbglog
%attr(770, mapdb, mapdb) /opt/FJSVmapdb/script/mdbencryptor
/opt/FJSVmapdb/script/mapdb_functions
%attr(770, mapdb, mapdb) /opt/FJSVmapdb/script/mdbpasswd
%attr(770, root, root) /opt/FJSVmapdb/script/mdbutil
%attr(644, root, root) /usr/lib/systemd/system/mdbutil.service
/usr/lib/tmpfiles.d/mapdb.conf
%attr(600, root, root) /var/spool/cron/root

