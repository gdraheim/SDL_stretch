Summary: SDL_stretch - Stretch Functions For The Simple DirectMedia Layer
Name: SDL_stretch
Version: 0.2.2
Release: 1mdk
Source0: %{name}-%{version}.tar.bz2
License: LGPL
Group: Development/Libraries
BuildRoot: %{_tmppath}/%{name}-buildroot
Prefix: %{_prefix}
Requires: libSDL1.2
BuildRequires: libSDL1.2-devel

%package devel
Summary: SDL_stretch - Stretch Functions For SDL - Headers and Docs
Group: Development/Libraries
Requires: libSDL1.2-devel

%description
Providing stretch and blit routines for SDL surfaces. 
These are optimized for speed including lots of assembler parts 
in the general routines and dynamic cpu native code generation 
for applications to compile specialized stretch-and-blit routines 
at runtime.

%description devel
While hacking on UAE (the unix amiga emulator) I did develop a few 
stretching routines. I have been asking on the SDL mainling for any 
prior art but it seems that no one did wrap such routines into a 
library part that can be reused everywhere. Other projects are 
just game SDKs which tend to wrap such routines it into their 
own framework - instead of using vanilla SDL surface. Also, there 
are only rare pieces of assembler optimized routines. I took some 
of these as hints and created my own set of highly optimized routines 
pumped up with assembler - stretch-and-blit routines for SDL on steroids. 

%prep
%setup -q
CFLAGS="$RPM_OPT_CFLAGS" sh configure --prefix=%_prefix

%build
make
make docs

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=%buildroot
make install-docs DESTDIR=%buildroot

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
 %_libdir/*.so.*

%files devel
%defattr(-,root,root)
 %_libdir/*.so
 %_libdir/*.a
 %_libdir/*.la
 %_libdir/pkgconfig/*
 %_includedir/*
 %_datadir/man/*
 %_datadir/groups/*

%changelog

# end of file
