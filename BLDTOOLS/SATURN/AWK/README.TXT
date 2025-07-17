
  gasm.bat は、HITACHI アセンブラ用のソースファイルを GNU アセンブラ用のソー
  スファイルに変換してアセンブルするユーティリティです。
  
  gasm.bat を使うためには以下のような準備が必要です。
  ・gasm.bat, gawk.exe, as.exe, gasp.exe をパスの通ったディレクトリに置く。
  ・環境変数 SEGALIBAWK に awk のスクリプトファイルのあるディレクトリを指定
    する。
  ・awk のスクリプトファイル（comment.awk, gasp.awk, keyword.awk, pool.awk,
    section.awk）を SEGALIBAWK に指定したディレクトリに置く。

  なお、ご使用のＰＣ環境によっては、GAWK.EXE が動作しない場合が考えられます。
  この場合は、GASM_EMX.BAT を使用して下さい。(GAWK_EMX.EXE を使用します)