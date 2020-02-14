# Copyright(c) 2018 FUJITSU LIMITED All Rights Reserved.
# @file        FishTech SPEC file
# @brief       Database RPM configuration file
# @author      anhnguyen
# @date        2018/12/07

%define name mdb-fdb
%define version ###Ver###
%define USERNAME mapdb
%define USERGRUP mapdb
%define debug_package %{nil}

Name:             %{name}
Version:          %{version}
Release:          ###Rev###%{?dist}
Summary:          Database configuration Service.
Group:            System Environment/Base
Source0:          %{name}-%{version}.tar.gz
BuildArch:        x86_64
License:          FUJITSU LIMITED
  
%description
Database configuration Service.

%prep
%setup -n famsdboperator

%build


%install
%{__mkdir_p} ${RPM_BUILD_ROOT}/opt/FJSVmapdb/db
%{__cp} -a opt/FJSVmapdb/db/* ${RPM_BUILD_ROOT}/opt/FJSVmapdb/db
%{__mkdir_p} ${RPM_BUILD_ROOT}/var/log/

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && %{__rm} -rf "${RPM_BUILD_ROOT}"

%post
%{__chown} %{USERNAME}:%{USERGRUP} /opt/
find /opt/ -type d | xargs %{__chown} %{USERNAME}:%{USERGRUP}
find /opt/ -type d | xargs %{__chmod} 770
touch /var/log/postgresql
%{__chown} %{USERNAME}:%{USERGRUP} /var/log/postgresql
%{__chmod} 660 /var/log/postgresql
%postun

%pre
%preun

%files
%defattr(660,%{USERNAME},%{USERGRUP},770)
/opt/FJSVmapdb/db/sql/fams_db.sql
