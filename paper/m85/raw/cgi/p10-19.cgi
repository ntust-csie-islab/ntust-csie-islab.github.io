#!/bin/sh

#�_�l�� QUERY_STRING
eval `/www/cgi-bin/cgiparse -init`

#QUERY_STRING�ܼƤ���
eval `/www/cgi-bin/cgiparse -form -prefix ""`

#���������H�W�r�r��A"(" ���j�Ÿ��H�᪺��ƧR��
actor=`echo $actor | awk -F\( '{print $1}'`

#�����H�W�r�r�ꤤ���ťղŸ��h���A�æb�᭱�[�W".html"�r��
artist=`echo $actor | awk '{print $1$2$3".html"}'`

#�ŧi��X�榡�� HTML �榡����r��
echo Content-type: text/html
echo

#�P�_�p�G username="movie" �ӥB password="world"�A�h�����H���
#�ɰe�ܼзǿ�X(stdout)
if [ $username = "movie" -a $password = "world" ]; then
  cd /home1/staff/kclau/public_html/entertain/movie/artists
  #�p�G�����H����ɦs�b�A�h���ɮפ��e�C�X��
  if [ -f $artist ]; then
    cat $artist
  else

  #�_�h�X�{�䤣���ɮת����~�T��
    echo "<br><br><br><br>"
    echo "<center><hr><h1>"
    echo "Sorry<br>! Artist Not Found !"
    echo "</h1><hr></center>"
  fi

#�p�GID �αK�X���~�A�h�X�{���U�T��
else
  echo "<br><br><br><br>"
  echo "<center><hr><h1>"
 echo "Sorry<br>! ID or password error !"
  echo "</h1><hr></center>"
fi
