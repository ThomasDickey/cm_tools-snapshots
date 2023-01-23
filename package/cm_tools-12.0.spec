Summary: CM Tools for RCS
%define AppProgram cm_tools
%define AppLibrary td_lib
%define AppVersion 12.x
%define AppRelease 20230122
%define LibRelease 20230122
# $Id: cm_tools-12.0.spec,v 1.25 2023/01/23 00:29:59 tom Exp $
Name: %{AppProgram}
Version: %{AppVersion}
Release: %{AppRelease}
License: MIT-X11
Group: Development/Tools
URL: https://invisible-island.net/ded
Source0: https://invisible-island.net/archives/ded/%{AppProgram}-%{AppRelease}.tgz
BuildRequires: td_lib <= %{AppRelease}
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify and extend the RCS programs
by preserving timestamps of archived files, as well as providing a
workaround for the long-broken setuid feature of RCS.  The latter is
used to support access-control lists, supported in a utility "permit".
Finally, there is an improved copy-file utility.

%prep

# no need for debugging symbols...
%define debug_package %{nil}

%setup -q -n %{AppProgram}-%{AppRelease}

%build

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

* Sun Jan 22 2023 Thomas Dickey
- build against td_lib package rather than side-by-side configuration

* Sat Mar 24 2018 Thomas Dickey
- update url, disable debug-build

* Sat Jul 03 2010 Thomas Dickey
- code cleanup

* Wed Jun 30 2010 Thomas Dickey
- initial version
