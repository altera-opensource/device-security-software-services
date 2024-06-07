@echo off
setlocal
setlocal EnableDelayedExpansion

set openssl_exists=0
set libspdm_exists=0
set libcurl_exists=0
set boost_exists=0
set gtest_exists=0
set current_dir=%cd%
set dependencies_dir=%current_dir%\dependencies
set spdm_wrapper_dir=%current_dir%\spdm_wrapper
set bkpprogrammer_dir=%current_dir%\bkpprogrammer
set TRUSTSTORE_PATH=/tmp/bkps-nonprod.p12
set JAVA_HOME=%JAVA_HOME:"=%
set KEYTOOL_EXE="%JAVA_HOME%/bin/keytool.exe"
for /f "delims=" %%x in (config.txt) do (
	set "%%x"
)

@echo on
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsamd64_x86.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %dependencies_dir%
cd %dependencies_dir%
set boost_version_string=%boost.version:.=_%
:: ====== openssl ======
If Defined openssl.version (
	If exist openssl_%openssl.version%_windows_x64 (
		If Defined always_build (
			set openssl_exists=0
		) else (
			set openssl_exists=1
		)
	)
	If !openssl_exists!==0 (
		:build_openssl
		echo building openssl...
		@RD /S /Q openssl
		@RD /S /Q openssl_%openssl.version%_windows_x64
		git clone https://github.com/openssl/openssl.git
		cd openssl
		git pull
		git fetch --all --tags
		git checkout tags/openssl-%openssl.version%
		call C:\Strawberry\perl\bin\perl.exe configure VC-WIN64A no-asm
		call nmake
		cd %dependencies_dir%
		xcopy openssl\include openssl_%openssl.version%_windows_x64\include /E /Y /I
		xcopy openssl\libcrypto-1_1-x64.dll openssl_%openssl.version%_windows_x64\lib\ /E /Y /I
		xcopy openssl\libcrypto.lib openssl_%openssl.version%_windows_x64\lib\ /E /Y /I
		xcopy openssl\libssl-1_1-x64.dll openssl_%openssl.version%_windows_x64\lib\  /E /Y /I
		xcopy openssl\libssl.lib openssl_%openssl.version%_windows_x64\lib\ /E /Y /I
		xcopy openssl\libcrypto_static.lib openssl_%openssl.version%_windows_x64\lib\ /E /Y /I
		xcopy openssl\libssl_static.lib openssl_%openssl.version%_windows_x64\lib\ /E /Y /I
		powershell Compress-Archive -Path openssl_%openssl.version%_windows_x64\* -DestinationPath openssl-%openssl.version%-windows-x64.zip
		set openssl_exists=1
	)
)

:: ====== libspdm ======
If Defined libspdm.version (
	If exist libspdm_%libspdm.version%_windows_x64 (
		If Defined always_build (
			set libspdm_exists=0
		) else (
			set libspdm_exists=1
		)
	)
	If !libspdm_exists!==0 (
		echo building libspdm...
		@RD /S /Q libspdm
		@RD /S /Q libspdm_%libspdm.version%_windows_x64
		If not exist "openssl" goto build_openssl
		git clone https://github.com/DMTF/libspdm.git
		cd libspdm
		git pull
		git fetch --all --tags
		git checkout tags/%libspdm.version%
		git submodule update --init unit_test/cmockalib/cmocka
		mkdir build
		cd build
		set "custom_defines=-DLIBSPDM_MAX_MESSAGE_BUFFER_SIZE=20000 -DLIBSPDM_MAX_CERT_CHAIN_BLOCK_LEN=15000 -DLIBSPDM_MAX_CERT_CHAIN_SIZE=18000 -DLIBSPDM_MAX_MEASUREMENT_RECORD_SIZE=15000"
		set "algorithms_enabled=-DLIBSPDM_RECORD_TRANSCRIPT_DATA_SUPPORT=1"
        set "algorithms_disabled=-DLIBSPDM_ENABLE_CAPABILITY_CHUNK_CAP=0 -DLIBSPDM_ENABLE_CAPABILITY_CSR_CAP=0 -DLIBSPDM_ENABLE_CAPABILITY_HBEAT_CAP=0 -DLIBSPDM_ENABLE_CAPABILITY_PSK_CAP=0 -DLIBSPDM_ENABLE_CAPABILITY_CHAL_CAP=0"
		set openssl_include_dir=%dependencies_dir%\openssl_%openssl.version%_windows_x64\include
		cmake -G"NMake Makefiles" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DARCH=x64 -DTOOLCHAIN=VS2015 -DTARGET=Release -DDISABLE_TESTS=1 -DCRYPTO=openssl -DCMAKE_C_FLAGS="!custom_defines! !algorithms_enabled! !algorithms_disabled! -I!openssl_include_dir!" -DENABLE_BINARY_BUILD=1 -DCOMPILED_LIBCRYPTO_PATH=%current_dir%\openssl_%openssl.version%_windows_x64\lib_static\libcrypto_static.lib -DCOMPILED_LIBSSL_PATH=%current_dir%\openssl_%openssl.version%_windows_x64\lib_static\libssl_static.lib ..
		nmake
		cd %dependencies_dir%
		xcopy libspdm\include libspdm_%libspdm.version%_windows_x64\include /E /Y /I
		xcopy libspdm\build\lib libspdm_%libspdm.version%_windows_x64\lib /E /Y /I
		for %%f in (debuglib_null.lib, debuglib.lib, malloclib.lib, memlib.lib, platform_lib_null.lib, platform_lib.lib, rnglib.lib, spdm_common_lib.lib, spdm_crypt_lib.lib, spdm_requester_lib.lib, spdm_secured_message_lib.lib, spdm_transport_mctp_lib.lib, cryptlib_openssl.lib, spdm_device_secret_lib_null.lib) do xcopy libspdm\build\lib\%%f libspdm_%libspdm.version%_windows_x64\lib_static\ /E /Y /I
		cd %dependencies_dir%
		powershell Compress-Archive -Path libspdm_%libspdm.version%_windows_x64\* -DestinationPath libspdm-%libspdm.version%-windows-x64.zip
		set libspdm_exists=1
	)
)

:: ====== libcurl ======
If Defined libcurl.version (
	If exist libcurl_%libcurl.version%_windows_x64 (
		If Defined always_build (
			set libcurl_exists=0
		) else (
			set libcurl_exists=1
		)
	)
	If !libcurl_exists!==0 (
		echo building libcurl...
		@RD /S /Q curl-%libcurl.version%
		@RD /S /Q libcurl_%libcurl.version%_windows_x64
		If not exist "openssl" goto build_openssl
		curl https://curl.se/download/curl-%libcurl.version%.tar.gz --output curl-%libcurl.version%.tar.gz
		tar -xzf curl-%libcurl.version%.tar.gz
		cd curl-%libcurl.version%\winbuild
		nmake /f Makefile.vc mode=dll WITH_SSL=dll ENABLE_SCHANNEL=no DEBUG=no GEN_PDB=no MACHINE=x64 WITH_DEVEL=../../openssl_%openssl.version%_windows_x64
		cd %dependencies_dir%
		xcopy curl-%libcurl.version%\builds\libcurl-vc-x64-release-dll-ssl-dll-ipv6-sspi libcurl_%libcurl.version%_windows_x64\ /E /Y /I
		powershell Compress-Archive -Path libcurl_%libcurl.version%_windows_x64\* -DestinationPath libcurl-%libcurl.version%-windows-x64.zip
		set libcurl_exists=1
	)
)

:: ====== boost ======
If Defined boost.version (
    If exist boost_%boost.version%_windows_x64 (
		If Defined always_build (
			set boost_exists=0
		) else (
			set boost_exists=1
		)
	)
	If !boost_exists!==0 (
		echo building boost...
		@RD /S /Q boost-%boost.version%
		@RD /S /Q boost_%boost.version%_windows_x64
		curl -L https://github.com/boostorg/boost/releases/download/boost-%boost.version%/boost-%boost.version%.tar.gz --output boost_%boost.version%.tar.gz
		tar -xzf boost_%boost.version%.tar.gz
		cd boost-%boost.version%
		cmd /c .\bootstrap.bat vc141
		cmd /c .\b2.exe --toolset=msvc-14.1 architecture=x86 address-model=64 runtime-link=shared link=static variant=release threading=multi install --prefix=output/x64 --layout=system -j4
		cd %dependencies_dir%
		xcopy boost-%boost.version%\output\x64\include boost_%boost.version%_windows_x64\include /E /Y /I
		xcopy boost-%boost.version%\output\x64\lib\libboost_container.lib boost_%boost.version%_windows_x64\lib\libboost_container-vc141-mt-x64-%boost_version_string:~0,-2%.lib*
		xcopy boost-%boost.version%\output\x64\lib\libboost_json.lib boost_%boost.version%_windows_x64\lib\libboost_json-vc141-mt-x64-%boost_version_string:~0,-2%.lib*
		powershell Compress-Archive -Path boost_%boost.version%_windows_x64\* -DestinationPath boost-%boost.version%-windows-x64.zip
		set boost_exists=1
	)
)

If Defined gtest.version (
    If exist googletest_%gtest.version%_windows_x64 (
		If Defined always_build (
			set gtest_exists=0
		) else (
			set gtest_exists=1
		)
	)
	If !gtest_exists!==0 (
		echo building gtest...
		@RD /S /Q googletest
		@RD /S /Q googletest_%gtest.version%_windows_x64
		git clone https://github.com/google/googletest.git
		cd googletest
		git pull
		git fetch --all --tags
		git checkout tags/v%gtest.version%
		cmake -G"NMake Makefiles" -DBUILD_GMOCK=ON
		call nmake
		cd %dependencies_dir%
		xcopy googletest\googletest\include googletest_%gtest.version%_windows_x64\include /E /Y /I
		xcopy googletest\googlemock\include googletest_%gtest.version%_windows_x64\include /E /Y /I
		xcopy googletest\lib googletest_%gtest.version%_windows_x64\lib /E /Y /I
		powershell Compress-Archive -Path googletest_%gtest.version%_windows_x64\* -DestinationPath googletest-%gtest.version%-windows-x64.zip
		set gtest_exists=1
	)
)

::build bkpprogrammer
echo buiding bkpprogrammer...
cd %bkpprogrammer_dir%
@RD /S /Q dependencies
@RD /S /Q build
xcopy %dependencies_dir%\openssl_%openssl.version%_windows_x64 dependencies\openssl /E /Y /I
xcopy %dependencies_dir%\boost_%boost.version%_windows_x64 dependencies\boost /E /Y /I
xcopy %dependencies_dir%\libcurl_%libcurl.version%_windows_x64 dependencies\libcurl /E /Y /I
xcopy %dependencies_dir%\googletest_%gtest.version%_windows_x64 dependencies\gtest /E /Y /I
mkdir build
cd build
cmake -D DISABLE_TESTS:BOOL=ON -D DISABLE_BKP_APP:BOOL=ON -DCMAKE_RULE_MESSAGES:BOOL=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -D CMAKE_BUILD_TYPE:STRING=Release -G "Visual Studio 15 2017 Win64" -T "v141" ..
cmake --build . --config Release

::build spdm_wrapper
echo buiding spdm_wrapper...
cd %spdm_wrapper_dir%
@RD /S /Q dependencies
@RD /S /Q build
xcopy %dependencies_dir%\openssl_%openssl.version%_windows_x64 dependencies\openssl /E /Y /I
xcopy %dependencies_dir%\libspdm_%libspdm.version%_windows_x64 dependencies\libspdm /E /Y /I
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release

::Java Gradle build
cd %current_dir%
If exist %TRUSTSTORE_PATH% (
	echo List dummy keys...
	@echo on
	%KEYTOOL_EXE% -list -keystore %TRUSTSTORE_PATH% -storepass donotchange -alias dummy
)

If not exist %TRUSTSTORE_PATH% (
	echo Generate dummy key...
	%KEYTOOL_EXE% -genkey -keyalg RSA -keystore %TRUSTSTORE_PATH% -keysize 2048 -keypass donotchange -storepass donotchange -dname "CN=Developer, OU=Department, O=Company, L=City, ST=State, C=CA" -alias dummy
)
set KEYSTORE_DUMMY_ALIAS=dummy
call gradlew.bat clean build

::Copy result files
xcopy bkpprogrammer\build\Release out_windows\bkpprogrammer /E /Y /I
xcopy spdm_wrapper\build\wrapper\Release out_windows\spdm_wrapper /E /Y /I
xcopy workload\build\libs out_windows\workload /E /Y /I
xcopy Verifier\build\libs out_windows\Verifier /E /Y /I
xcopy Verifier\src\main\resources\config.properties out_windows\Verifier /E /Y /I
xcopy bkps\build\libs out_windows\bkps /E /Y /I

endlocal
EXIT /B %ERRORLEVEL%
