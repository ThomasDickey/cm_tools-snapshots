Summary: CM Tools for RCS
%define AppVersion 20100630
%define LibVersion 20100624
# $Header: /users/source/archives/cm_tools.vcs/package/RCS/cm_tools-12.0.spec,v 1.1 2010/06/30 09:07:26 tom Exp $
Name: cm_tools
Version: 12.x
# Base version is 12.x; rpm version corresponds to "Source1" directory name.
Release: %{AppVersion}
License: MIT-X11
Group: Applications/Editors
URL: ftp://invisible-island.net/ded
Source0: td_lib-%{LibVersion}.tgz
Source1: cm_tools-%{AppVersion}.tgz
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify and extend the RCS programs
by preserving timestamps of archived files, as well as providing a
workaround for the long-broken setuid feature of RCS.  The latter is
used to support access-control lists, supported in a utility "permit".
Finally, there is an improved copy-file utility.

%prep

# -a N (unpack Nth source after cd'ing into build-root)
# -b N (unpack Nth source before cd'ing into build-root)
# -D (do not delete directory before unpacking)
# -q (quiet)
# -T (do not do default unpacking, is used with -a or -b)
rm -rf cm_tools-12.x
mkdir cm_tools-12.x
%setup -q -D -T -a 1
mv cm_tools-%{AppVersion}/* .
%setup -q -D -T -a 0

%build

cd td_lib-%{LibVersion}

./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir}
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

make install                    DESTDIR=$RPM_BUILD_ROOT

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

* Wed Jun 30 2010 Thomas Dickey
- initial version
