%{!?php_inidir:  %global php_inidir  %{_sysconfdir}/php.d}
%global ini_name    clearsilver.ini

Name: php56w-clearsilver
Version:	0.10.5
Release:	1%{?dist}
Summary:	ClearSilver templating for PHP		

Group: Development/Languages
License:	New BSD License. 
URL:		http://www.clearsilver.net/	
Source0:	php_clearsilver-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	php56w-devel clearsilver-devel re2c
Requires:	php56w-common

%description
ClearSilver is a fast, powerful, and language-neutral HTML template
system.  In both static content sites and dynamic HTML applications,
it provides a separation between presentation code and application
logic which makes working with your project easier.  The design of
ClearSilver began in 1999, and evolved during its use at onelist.com,
egroups.com, and Yahoo! Groups.  Today many other projects and
websites are using it. This is the PHP module for it

%prep
%setup -q
cat > %{ini_name} << 'EOF'
; Enable ClearSilver extension module
extension = clearsilver.so
EOF


%build
%{_bindir}/phpize
%configure --with-clearsilver --with-php-config=%{_bindir}/php-config
make %{?_smp_mflags}


%install
rm -rf %{buildroot}
make install INSTALL_ROOT=%{buildroot} DESTDIR=%{buildroot}
install -Dpm 644 %{ini_name} %{buildroot}%{php_inidir}/%{ini_name}


%clean
rm -rf %{buildroot}


%files
%{php_extdir}/clearsilver.so
%config(noreplace) %{php_inidir}/%{ini_name}
%defattr(-,root,root,-)
%doc



%changelog

