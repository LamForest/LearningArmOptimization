set DEPLOY_DIR=/data/local/tmp/ldp
ECHO %DEPLOY_DIR%
adb shell "mkdir -p /data/local/tmp/ldp/lib"
adb push .\build\unit_test\runUnitTests /data/local/tmp/ldp
adb push .\build\src\libboxfilter.so /data/local/tmp/ldp/lib

@REM adb shell "su; cd /data/local/tmp/ldp; chmod a+x runUnitTests; export LD_LIBRARY_PATH=./lib ; ./runUnitTests"