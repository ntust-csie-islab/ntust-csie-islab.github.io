#!/bin/sh

#起始化 QUERY_STRING
eval `/www/cgi-bin/cgiparse -init`

#QUERY_STRING變數切割
eval `/www/cgi-bin/cgiparse -form -prefix ""`

#把選取的藝人名字字串，"(" 分隔符號以後的資料刪除
actor=`echo $actor | awk -F\( '{print $1}'`

#把藝人名字字串中的空白符號去除，並在後面加上".html"字串
artist=`echo $actor | awk '{print $1$2$3".html"}'`

#宣告輸出格式為 HTML 格式的文字檔
echo Content-type: text/html
echo

#判斷如果 username="movie" 而且 password="world"，則把藝人資料
#檔送至標準輸出(stdout)
if [ $username = "movie" -a $password = "world" ]; then
  cd /home1/staff/kclau/public_html/entertain/movie/artists
  #如果該藝人資料檔存在，則把檔案內容列出來
  if [ -f $artist ]; then
    cat $artist
  else

  #否則出現找不到檔案的錯誤訊息
    echo "<br><br><br><br>"
    echo "<center><hr><h1>"
    echo "Sorry<br>! Artist Not Found !"
    echo "</h1><hr></center>"
  fi

#如果ID 或密碼錯誤，則出現底下訊息
else
  echo "<br><br><br><br>"
  echo "<center><hr><h1>"
 echo "Sorry<br>! ID or password error !"
  echo "</h1><hr></center>"
fi
