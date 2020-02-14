# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        FishTech SPEC file
# @brief       Data Management RPM Configuration file
# @author      anhnguyen
# @date        2018/12/07

%define name mdb-fdm
%define version ###Ver###
%define USERNAME mapdb
%define USERGRUP mapdb
%define debug_package %{nil}
%define deploy_path /opt/FJSVmapdb

Name:             %{name}
Version:          %{version}
Release:          ###Rev###%{?dist}
Summary:          Data Management Service.
Group:            System Environment/Base
Source0:          %{name}-%{version}.tar.gz
BuildArch:        x86_64
License:          FUJITSU LIMITED

%description
Data Management Server.

%prep
%setup -n famsdatamanagement

%build
cd ../mdbcommon/opt/FJSVmapdb
%{__make}
cd ../../../famsdatamanagement/opt/FJSVmapdb/
%{__make}


%install
%{__cp} -a usr ${RPM_BUILD_ROOT}
%{__mkdir_p} ${RPM_BUILD_ROOT}/opt/FJSVmapdb/{bin,daemon,etc,script}
%{__cp} -a opt/FJSVmapdb/daemon/build/bin/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/daemon
%{__cp} -a opt/FJSVmapdb/etc/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/etc
%{__cp} -a opt/FJSVmapdb/script/build/bin/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/script
%{__ln_s} /opt/FJSVmapdb/script/mdbdatamanagementadm  ${RPM_BUILD_ROOT}/opt/FJSVmapdb/bin/mdbdatamanagementadm 
%{__mkdir_p} ${RPM_BUILD_ROOT}/var/log/

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && %{__rm} -rf "${RPM_BUILD_ROOT}"

%post
%{__chown} %{USERNAME}:%{USERGRUP} /opt/
%{__chown} %{USERNAME}:%{USERGRUP} -R %{deploy_path}
%{__chmod} 770 -R %{deploy_path}
%{__chmod} 550 /opt/FJSVmapdb/bin
touch /var/log/postgresql
%{__chown} %{USERNAME}:%{USERGRUP} /var/log/postgresql
%{__chmod} 660 /var/log/postgresql
%postun

%pre
%preun

%files
%defattr(660,%{USERNAME},%{USERGRUP},770)
/opt/FJSVmapdb/bin/mdbdatamanagementadm 
%attr(770, %{USERNAME}, %{USERGRUP}) /opt/FJSVmapdb/daemon/famsdtrcv
%attr(770, %{USERNAME}, %{USERGRUP}) /opt/FJSVmapdb/daemon/famsdtul
%attr(770, %{USERNAME}, %{USERGRUP}) /opt/FJSVmapdb/daemon/famsevtps
/opt/FJSVmapdb/etc/fams_data_util.conf
/opt/FJSVmapdb/etc/fams_data_util_api.yaml
/opt/FJSVmapdb/etc/fams_receiver.conf
/opt/FJSVmapdb/etc/fams_receiver_api.yaml
/opt/FJSVmapdb/etc/fams_event_processing.conf
/opt/FJSVmapdb/etc/fams_error_messages.yaml
/opt/FJSVmapdb/etc/reception_processing_shm.conf
%attr(770, %{USERNAME}, %{USERGRUP}) /opt/FJSVmapdb/etc/duration_limit
%attr(770, %{USERNAME}, %{USERGRUP}) /opt/FJSVmapdb/script/mdbdatamanagementadm 
%attr(644, root, root) /usr/lib/systemd/system/mdbdatamanagementadm.service
