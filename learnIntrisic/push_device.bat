set DEPLOY_DIR=/data/local/tmp/ldp
ECHO %DEPLOY_DIR%
adb shell "mkdir -p /data/local/tmp/host"
@REM adb push .\build\unit_test\runUnitTests /data/local/tmp/host
adb push .\build\rgb\rgb /data/local/tmp/host/rgb

@REM adb shell "su; cd /data/local/tmp/ldp; chmod a+x rgb; export LD_LIBRARY_PATH=./lib ; ./rgb"