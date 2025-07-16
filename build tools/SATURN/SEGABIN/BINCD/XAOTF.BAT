rem ##### オンザフライ書き込み実行バッチファイル #####
xbld -otf %1
if "%1"=="" goto end
addcdw -otf=%1 -t
:end
