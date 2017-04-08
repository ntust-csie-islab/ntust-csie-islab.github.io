#!/bin/sh

#設定環境變數 PID 為該程式的 pid，避免多人使用時把資料寫入同一檔
PID=`echo $$`

#把 HTTP Server 傳過來的環境變數 QUERY_STRING 切割成 $FORM_[變數名]
#的多個環境變數。本程式由 CERN HTTP Server 提供，相關的使用指令，請
#參考 cgiparse 
/home3/www/cgi-bin/cgiparse -form

#宣告後面資料為一般文字檔，內容為HTML格式，此行後面要加一行空白
#不然Server傳出去的資料會是空白
echo "Content-type: text/html"
echo

#把HTML語法及環境變數$FORM_username(使用者輸入的名字)存入暫存檔 
#/tmp/personal.PID
#由於不同時候執行程式的 pid 不一樣，在此可以避免多人使用時重複寫
#入的問題
echo "<title>$FORM_username的履歷</title>" > /tmp/personal.PID

#把字串 <center> 附加在 /tmp/personal.PID 後面
echo "<center>" >> /tmp/personal.PID
 

#以下處理方式相同
echo "<img src=/~jhlien/image/person3.gif>" >> /tmp/personal.PID

echo "<table border=4 cellspacing=3 cellpadding=3 width=100%>" >> 
/tmp/personal.PID

echo "<tr><th>姓 名</th><td>$FORM_username</td>">> 
/tmp/personal.PID

echo "<th>性 別</th><td>$FORM_sex</td>" >> /tmp/personal.PID

echo "<th rowspan=6><img src=$FORM_picture></th></tr>" >> 
/tmp/personal.PID
echo "<tr><th>生 日</th><td>$FORM_birthday</td><th>婚 姻
</th><td>$FORM_marry</td></tr>" >> /tmp/personal.PID

echo "<tr><th>籍 貫</th><td>$FORM_birthplace</td><th>血 型
</th><td>$FORM_blood</td></tr>" >> /tmp/personal.PID

echo "<tr><th>身 高</th><td>$FORM_tall CM</td><th>體 重
</th><td>$FORM_weight kg</td></tr>" >> /tmp/personal.PID

echo "<tr><th>兵役別</th><td>$FORM_military</td><th>家庭狀況
</th><td>$FORM_family</td></tr>" >> /tmp/personal.PID

echo "<tr><th>身體狀況</th><td>$FORM_health</td><th>宗教信仰
</th><td>$FORM_religion</td></tr>" >> /tmp/personal.PID

echo "<tr><th>通訊地址</th><td colspan=2>$FORM_address1</td><th>電
話</th><td>$FORM_telephone1</td></tr>" >> /tmp/personal.PID

echo "<tr><th>戶籍地址</th><td colspan=2>$FORM_address2</td><th>電
話</th><td>$FORM_telephone2</td></tr>" >> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID
echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" >> 
/tmp/personal.PID

echo "<tr><th rowspan=4>學<br>　<br>　<br>歷</th><th>等　　別
</th><th>學　　校</th><th>系 所 別</th><th>畢業年月</th></tr>" >> 
/tmp/personal.PID
 

echo "<tr><td>$FORM_school_g1</td><td>$FORM_school_n1</td><td> 
$FORM_school_t1</td><td>$FORM_school_d1</td></tr>">> 
/tmp/personal.PID

echo "<tr><td>$FORM_school_g2</td><td>$FORM_school_n2　
</td><td>$FORM_school_t2　</td><td>$FORM_school_d2</td></tr>" >> 
/tmp/personal.PID

echo "<tr><td>$FORM_school_g3</td>" >> /tmp/personal.PID

<td>$FORM_school_n3　</td><td>$FORM_school_t3　
</td><td>$FORM_school_d3　</td></tr>" >> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID

echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%> " >> 
/tmp/personal.PID

echo "<tr><th width=15%>專業專長</th><td>$FORM_profess1</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>特殊專長</th><td>$FORM_profess2</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>電腦軟體</th><td>$FORM_profess3</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>個人興趣</th><td>$FORM_interest</td></tr>" 
>> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID

echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%> " >> 
/tmp/personal.PID

echo "<tr><th width=15%>語文能力</th>" >> /tmp/personal.PID
 

#判斷使用在台語語文能力是否有選擇聽、說、讀、寫任一種，如果有則填入
#所選的資料，否則填入一個全型的空白符號
if [ "$FORM_langauep1" ]; then
 echo "<td width=25%>台語─$FORM_langauep1</td>">>/tmp/personal.PID
else
 echo "<td width=25%>　</td>"  >> /tmp/personal.PID
fi

#底下二個判斷的處理方式與上列相同
if [ "$FORM_langauep2" ]; then
 echo "<td width=25%>英語─$FORM_langauep2</td>">> tmp/personal.PID
else
 echo "<td width=25%>　</td>"  >> /tmp/personal.PID
fi

if [ "$FORM_langauep3" ]; then
  echo "<td width=25%>$FORM_oterlang─$FORM_langauep4</td>" >> 
/tmp/personal.PID
else
  echo "<td width=25%>　</td>"  >> /tmp/personal.PID
fi

echo "</tr></table>" >> /tmp/personal.PID
echo "<p>" >> /tmp/personal.PID

#判斷個人經歷是否需要把結果導入 /tmp/personal.PID
#如果使用者在填表格時，沒有選 include本欄, 則$FORM_include1 的值為 
#null(空資料)
if [ $FORM_include1 ] ; then
  echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" 
>> /tmp/personal.PID

  echo "<tr><th><font size=+3>※ 經　　　　　歷 ※</font></th></tr>" 
>> /tmp/personal.PID

  echo "<tr><td><pre>$FORM_execp</pre>　<p>　<p></td></tr>" >> 
/tmp/personal.PID

  echo "</table>" >> /tmp/personal.PID
fi
 

#判斷應徵職務是否需要把結果導入 /tmp/personal.PID
#處理方式與上列相同
if [ $FORM_include2 ] ; then
  echo "<p>" >> /tmp/personal.PID

  echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" 
>> /tmp/personal.PID

  echo "<tr><th>　</th><th>應徵職務</th><td>$FORM_job_type</td>" 
>> /tmp/personal.PID

  echo "<th>希望待遇</th><td>$FORM_money</td></tr>" >> 
/tmp/personal.PID

 echo "<tr><th>對<br>職<br>務<br>的<br>認<br>知<br>與<br>期<br>許
</th>" >> /tmp/personal.PID

  echo "<td colspan=4><pre>$FORM_understand</pre></td></tr>" >> 
/tmp/personal.PID

  echo "</table>" >> /tmp/personal.PID

fi 

echo "<p>" >> /tmp/personal.PID

echo "</center>" >> /tmp/personal.PID

#把最後的結果，逐一檢查把使用者沒有填入資料處 <td></td> 換成 <td>
#　</td>，以及把<td colspan=2></td> 換成 <td colspan=2>　</td>，即在
#二者中間加入一個全型空白符號，這樣在瀏覽器上顯示時，比較美觀。
cat /tmp/personal.PID | awk '{print $0}' | sed 
s/'<td><\/td>'/'<td>　<\/td>'/g|sed s/'<td colspan=2><\/td>'/'<td 
colspan=2>　<\/td>'/g 

#把暫存檔 /tmp/personal.PID 刪除
\rm /tmp/personal.PID

