Summary: CM Tools for RCS
%define AppProgram cm_tools
%define AppLibrary td_lib
%define AppVersion 12.x
%define AppRelease 20191206
%define LibRelease 20191206
# $Id: cm_tools-12.0.spec,v 1.17 2019/12/07 01:17:25 tom Exp $
Name: %{AppProgram}
Version: %{AppVersion}
Release: %{AppRelease}
License: MIT-X11
Group: Development/Tools
URL: ftp://ftp.invisible-island.net/ded
Source0: %{AppLibrary}-%{LibRelease}.tgz
Source1: %{AppProgram}-%{AppRelease}.tgz
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify and extend the RCS programs
by preserving timestamps of archived files, as well as providing a
workaround for the long-broken setuid feature of RCS.  The latter is
used to support access-control lists, supported in a utility "permit".
Finally, there is an improved copy-file utility.

# no need for debugging symbols...
%define debug_package %{nil}

%prep

# -a N (unpack Nth source after cd'ing into build-root)
# -b N (unpack Nth source before cd'ing into build-root)
# -D (do not delete directory before unpacking)
# -q (quiet)
# -T (do not do default unpacking, is used with -a or -b)
rm -rf %{AppProgram}-%{AppVersion}
mkdir %{AppProgram}-%{AppVersion}
%setup -q -D -T -a 1
mv %{AppProgram}-%{AppRelease}/* .
%setup -q -D -T -a 0

%build

cd %{AppLibrary}-%{LibRelease}

./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir} \
		--disable-echo
make

cd ..
./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir}
make

%install

[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

make install DESTDIR=$RPM_BUILD_ROOT

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/baseline
%{_bindir}/checkin
%{_bindir}/checkout
%{_bindir}/checkup
%{_bindir}/copy
%{_bindir}/link2rcs
%{_bindir}/pci
%{_bindir}/pco
%{_bindir}/permit
%{_bindir}/rcsget
%{_bindir}/rcsput
%{_bindir}/vcs
%{_mandir}/man1/baseline.*
%{_mandir}/man1/checkin.*
%{_mandir}/man1/checkout.*
%{_mandir}/man1/checkup.*
%{_mandir}/man1/copy.*
%{_mandir}/man1/link2rcs.*
%{_mandir}/man1/permit.*
%{_mandir}/man1/rcsget.*
%{_mandir}/man1/rcsput.*
%{_mandir}/man1/vcs.*

%changelog
# each patch should add its ChangeLog entries here

* Sat Mar 24 2018 Thomas Dickey
- update url, disable debug-build

* Sat Jul 03 2010 Thomas Dickey
- code cleanup

* Wed Jun 30 2010 Thomas Dickey
- initial version
