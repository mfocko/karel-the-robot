Name:           libkarel
Version:        2019.1
Release:        1%{?dist}
Summary:        C library that provides Karel the Robot interface.
License:        GPLv3
URL:            https://git.kpi.fei.tuke.sk/kpi/karel-the-robot
Source0:        karel-%{version}.tar.gz
BuildArch:      x86_64
BuildRequires:  gcc
BuildRequires:  cmake
BuildRequires:  make
BuildRequires:  ncurses-devel
BuildRequires:  gettext-devel
BuildRequires:  check-devel
Requires:       ncurses-devel

%undefine _hardened_build
%define _debugsource_template %{nil}

%description
%{summary}

%prep
%autosetup -n %{name}-%{version}

%build
%cmake
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%{_prefix}/lib/libkarel.a
%{_prefix}/lib/libsuperkarel.a
%{_includedir}/karel.h
%{_includedir}/superkarel.h
%{_docdir}/libkarel/*

%doc README.md

%changelog
* Mon Oct 18 2021 Matej Focko <mfocko@redhat.com> - 2019.1-1
- Initial release
