%{!?python_sitelib: %define python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib(0)")}

Name:    zorba
Version: 0.9.21
Release: 4%{?dist}
Summary: General purpose XQuery processor implemented in C++

Group: System Environment/Libraries
License: ASL 2.0
URL: http://www.zorba-xquery.com/
Source0: ftp://ftp.heanet.ie/pub/download.sourceforge/z/zo/zorba/%{name}-%{version}.tar.gz

%{!?ruby_sitelib: %define ruby_sitelib %( ruby -r rbconfig -e "print Config::CONFIG['sitearchdir']")}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: cmake >= 2.4 libxml2-devel >= 2.2.16 icu >= 2.6 libicu-devel
BuildRequires: boost-devel >= 1.32 xerces-c-devel >= 2.7 libcurl-devel
BuildRequires: ruby-devel >= 1.8 graphviz


%description
Zorba is a general purpose XQuery processor implementing in C++ the
W3C family of specifications. It is not an XML database. The query
processor has been designed to be embeddable in a variety of
environments such as other programming languages extended with XML
processing capabilities, browsers, database servers, XML message
dispatchers, or smart phones. Its architecture employs a modular
design, which allows customizing the Zorba query processor to the
environment's needs. In particular the architecture of the query
processor allows a pluggable XML store (e.g. main memory, DOM stores,
persistent disk-based large stores, S3 stores).


%package devel
Summary: Header files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}


%description devel
The %{name}-devel package contains headers for building applications
that use %{zorba}.


%package python
Summary: %{name} Python module for %{name}
Group: Development/Languages
Requires: %{name} = %{version}-%{release}
BuildRequires: swig


%description python
Provides Python module to use %{name} API


%package ruby
Summary: %{name} Ruby module
Group: Development/Languages
BuildRequires: swig
Requires: %{name} = %{version}-%{release}
Requires: ruby >= 1.8
%description ruby
Provides ruby module to use %{name} API

 
%prep
%setup -q

%build
mkdir -p build
pushd build
cmake -D CMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug --debug-output -DCMAKE_SKIP_BUILD_RPATH=1  ..
make %{?_smp_mflags}
make doc
popd


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="install -p" -C build


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_libdir}/*.so.%{version}

%dir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/doc/%{name}-%{version}


%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/%{name}/%{name}/*.h
%{_includedir}/%{name}/simplestore/*.h
%{_datadir}/doc/%{name}-%{version}/python/examples/*.py
%{_datadir}/doc/%{name}-%{version}/python/examples/*.pyc
%{_datadir}/doc/%{name}-%{version}/python/examples/*.pyo
%{_datadir}/doc/%{name}-%{version}/python/html/*.gif
%{_datadir}/doc/%{name}-%{version}/ruby/examples/*.rb
%{_datadir}/doc/%{name}-%{version}/ruby/html/*.gif


%files python
%defattr(-,root,root,-)
%dir %{python_sitelib}/_zorba_api.so
%dir %{python_sitelib}/zorba_api.*


%files ruby
%defattr(-,root,root,-)
%dir %{ruby_sitelib}/zorba_api.so


%changelog
* Mon Aug 25 2008 Paul Kunz <PaulfKunz@gmail.com> - 0.9.21-4
- Added requirement of graphviz and libcurl-devel

* Fri Aug 22 2008 Paul Kunz <PaulfKunz@gmail.com> - 0.9.21-3
- remove devel-doc subpackage

* Sun Aug 17 2008 Paul Kunz <PaulfKunz@gmail.com> - 0.9.21-3
- Fixed Source tag with ftp: and site that has 0.9.21 version
- added Ruby requirements
- Added requirement of boost-devel
- fixed file attributes
- move python_sitelib to top
- Fixed spelling

* Sun Aug 17 2008 Paul Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.1-2
- Use ftp: instead of file: for source

* Tue Jul 29 2008 Paul Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.21-2
- Fixed grammar of summary

* Mon Jul 28 2008 Paul Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.21-2
- remove rpath in build
- added documentation
- added devel package
- remove non utf8 character
- fix license entry
- fix description line


* Sun Jun 29 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.21-1
- Update to 0.9.21

* Fri Jun 27 2008 Paul Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.2-2
- Added required -devel packages for build


* Sun Jun 22 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.2-2
- Added correct install of ruby and python modules

* Fri Jun 20 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.2-1
- update to version 0.9.2

* Mon Jun 16 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.2541-1
- added python and ruby subpackages

* Fri Jun 13 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.2-1
- removed patch for next release

* Thu May 29 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.1-1
- Add patch for gcc 4.3 under Fedora 9


* Mon May 19 2008 Paul F. Kunz <Paul_Kunz@slac.stanford.edu> - 0.9.1-1
- Initial 
