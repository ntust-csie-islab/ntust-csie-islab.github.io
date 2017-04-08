#!/bin/sh

#取得程式執行時的id
PID=`echo $$`
 

#切割 QUERY_STRING 環境變數
eval `/usr/local/etc/httpd/cgi-bin/cgiparse -form`

#指定系統 mail 的完整路徑
mailprog=/usr/ucb/mail

#宣告本資料檔為 HTML 格式的文字檔
echo "Content-type: text/html"
echo ""

#將結果先存至 /tmp/letter.$PID 檔案裡
echo "敬啟者："   > /tmp/letter.$PID
echo ""  >> /tmp/letter.$PID

#取得訪客所填的名字，及從遠端所傳來的使用者名字(此資訊不易取得)
echo "我的名字是[$FORM_name]($REMOTE_IDENT)" >> /tmp/letter.$PID

#將職業類別附加至/tmp/letter.$PID
echo "我的職業是[$FORM_job_type|$FORM_job]" >> /tmp/letter.$PID

#將電子信箱附加至/tmp/letter.$PID
echo "電子信箱是[$FORM_email]" >> /tmp/letter.$PID

#取得遠端使用者使用的瀏覽器資訊，並附加至/tmp/letter.$PID
echo "我使用的client是--$HTTP_USER_AGENT" >> /tmp/letter.$PID

#取得遠端的 hostname 及 IP 位址，附加至/tmp/letter.$PID
echo "我是來自於:$REMOTE_HOST($REMOTE_ADDR)" >> /tmp/letter.$PID

#將使用者意見附加至/tmp/letter.$PID
echo "我覺得交大校園生活資訊做的─[$FORM_judge]" >> /tmp/letter.$PID
echo "我的一些建言：" >> /tmp/letter.$PID

#將使用者意見附加至/tmp/letter.$PID
/www/cgi-bin/cgiparse -value comment >> /tmp/letter.$PID

#將 /tmp/letter.$PID 的內容寄給管理者 shock
$mailprog -s "From $FORM_name" shock < /tmp/letter.$PID
 

#刪除 /tmp/letter.$PID
rm -f /tmp/letter.$PID

#將底下資訊傳給伺服器
echo "<title>Thank You !</title>"
echo "<center><br><br>"
echo "<h1>要用 netscape 1.1n 才可以看到小新喔</h1>"
echo "<table border=3 celladding=5 cellpassing=5>"
echo "<td>Thank you for your suggestion<p>Have a nice 
day !</td>"
echo "<td><TABLE BORDER=5 CELLSPACING=5 CELLPADDING=5>"
echo "<TR><TD ALIGN=center>"
#小新跳舞是由網景瀏覽器所提供的線上動畫功能所製作
echo "<IMG SRC=/cgi-bin/doit.cgi?/home/users/shock/image>"
echo "</TD></TR>"
echo "</TABLE></td><tr></table>"
echo "</CENTER>"
