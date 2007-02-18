%define my_build yjl.0.testing.1

Summary: Plugins for use with libvisual
Name: libvisual-plugins
Epoch: 0
Version: 0.2.0
Release: 0.%{epoch}.%my_build
License: GPL
Group: Applications/Multimedia
URL: http://libvisual.sourceforge.net/v2/
Source0: %{name}-%{version}.tar.bz2
Packager: Michael A. Peters <mpeters@mac.com>
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: libvisual-devel
BuildRequires: xorg-x11-devel
BuildRequires: alsa-lib-devel
BuildRequires: esound-devel
BuildRequires: pango-devel
Requires: libvisual
Requires: alsa-lib
Requires: esound
Requires: pango


%description
This package contains several plugins for libvisual

%prep
%setup -q

%build
%configure 
%__make

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall

# fix plugins
if [ ! -d %buildroot%_libdir/libvisual ]; then
	mkdir -p %buildroot%_libdir/libvisual
	#actor
	mkdir -p %buildroot%_libdir/libvisual/actor
	mv %buildroot%_libdir/actor* %buildroot%_libdir/libvisual/actor/
	mkdir -p %buildroot%_libdir/libvisual/input
	mv %buildroot%_libdir/input* %buildroot%_libdir/libvisual/input/
	mkdir -p %buildroot%_libdir/libvisual/morph
	mv %buildroot%_libdir/morph* %buildroot%_libdir/libvisual/morph/
fi

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README TODO
%_libdir/libvisual/actor
%_libdir/libvisual/input
%_libdir/libvisual/morph
%_datadir/libvisual/actor
%_datadir/libvisual-plugins

%changelog
* Mon Jan 31 2005 Michael A. Peters <mpeters@mac.com>
- testing pre 0.2.0 release

* Mon Nov 15 2004  Michael A. Peters <mpeters@mac.com>
- Initial build.


