#!/bin/sh

#�]�w�����ܼ� PID ���ӵ{���� pid�A�קK�h�H�ϥήɧ��Ƽg�J�P�@��
PID=`echo $$`

#�� HTTP Server �ǹL�Ӫ������ܼ� QUERY_STRING ���Φ� $FORM_[�ܼƦW]
#���h�������ܼơC���{���� CERN HTTP Server ���ѡA�������ϥΫ��O�A��
#�Ѧ� cgiparse 
/home3/www/cgi-bin/cgiparse -form

#�ŧi�᭱��Ƭ��@���r�ɡA���e��HTML�榡�A����᭱�n�[�@��ť�
#���MServer�ǥX�h����Ʒ|�O�ť�
echo "Content-type: text/html"
echo

#��HTML�y�k�������ܼ�$FORM_username(�ϥΪ̿�J���W�r)�s�J�Ȧs�� 
#/tmp/personal.PID
#�ѩ󤣦P�ɭ԰���{���� pid ���@�ˡA�b���i�H�קK�h�H�ϥήɭ��Ƽg
#�J�����D
echo "<title>$FORM_username���i��</title>" > /tmp/personal.PID

#��r�� <center> ���[�b /tmp/personal.PID �᭱
echo "<center>" >> /tmp/personal.PID
 

#�H�U�B�z�覡�ۦP
echo "<img src=/~jhlien/image/person3.gif>" >> /tmp/personal.PID

echo "<table border=4 cellspacing=3 cellpadding=3 width=100%>" >> 
/tmp/personal.PID

echo "<tr><th>�m �W</th><td>$FORM_username</td>">> 
/tmp/personal.PID

echo "<th>�� �O</th><td>$FORM_sex</td>" >> /tmp/personal.PID

echo "<th rowspan=6><img src=$FORM_picture></th></tr>" >> 
/tmp/personal.PID
echo "<tr><th>�� ��</th><td>$FORM_birthday</td><th>�B ��
</th><td>$FORM_marry</td></tr>" >> /tmp/personal.PID

echo "<tr><th>�y �e</th><td>$FORM_birthplace</td><th>�� ��
</th><td>$FORM_blood</td></tr>" >> /tmp/personal.PID

echo "<tr><th>�� ��</th><td>$FORM_tall CM</td><th>�� ��
</th><td>$FORM_weight kg</td></tr>" >> /tmp/personal.PID

echo "<tr><th>�L�ЧO</th><td>$FORM_military</td><th>�a�x���p
</th><td>$FORM_family</td></tr>" >> /tmp/personal.PID

echo "<tr><th>���骬�p</th><td>$FORM_health</td><th>�v�ЫH��
</th><td>$FORM_religion</td></tr>" >> /tmp/personal.PID

echo "<tr><th>�q�T�a�}</th><td colspan=2>$FORM_address1</td><th>�q
��</th><td>$FORM_telephone1</td></tr>" >> /tmp/personal.PID

echo "<tr><th>���y�a�}</th><td colspan=2>$FORM_address2</td><th>�q
��</th><td>$FORM_telephone2</td></tr>" >> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID
echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" >> 
/tmp/personal.PID

echo "<tr><th rowspan=4>��<br>�@<br>�@<br>��</th><th>���@�@�O
</th><th>�ǡ@�@��</th><th>�t �� �O</th><th>���~�~��</th></tr>" >> 
/tmp/personal.PID
 

echo "<tr><td>$FORM_school_g1</td><td>$FORM_school_n1</td><td> 
$FORM_school_t1</td><td>$FORM_school_d1</td></tr>">> 
/tmp/personal.PID

echo "<tr><td>$FORM_school_g2</td><td>$FORM_school_n2�@
</td><td>$FORM_school_t2�@</td><td>$FORM_school_d2</td></tr>" >> 
/tmp/personal.PID

echo "<tr><td>$FORM_school_g3</td>" >> /tmp/personal.PID

<td>$FORM_school_n3�@</td><td>$FORM_school_t3�@
</td><td>$FORM_school_d3�@</td></tr>" >> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID

echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%> " >> 
/tmp/personal.PID

echo "<tr><th width=15%>�M�~�M��</th><td>$FORM_profess1</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>�S��M��</th><td>$FORM_profess2</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>�q���n��</th><td>$FORM_profess3</td></tr>" 
>> /tmp/personal.PID

echo "<tr><th width=15%>�ӤH����</th><td>$FORM_interest</td></tr>" 
>> /tmp/personal.PID

echo "</table>" >> /tmp/personal.PID

echo "<p>" >> /tmp/personal.PID

echo "<table border=3 cellspacing=3 cellpadding=3 width=100%> " >> 
/tmp/personal.PID

echo "<tr><th width=15%>�y���O</th>" >> /tmp/personal.PID
 

#�P�_�ϥΦb�x�y�y���O�O�_�����ť�B���BŪ�B�g���@�ءA�p�G���h��J
#�ҿ諸��ơA�_�h��J�@�ӥ������ťղŸ�
if [ "$FORM_langauep1" ]; then
 echo "<td width=25%>�x�y�w$FORM_langauep1</td>">>/tmp/personal.PID
else
 echo "<td width=25%>�@</td>"  >> /tmp/personal.PID
fi

#���U�G�ӧP�_���B�z�覡�P�W�C�ۦP
if [ "$FORM_langauep2" ]; then
 echo "<td width=25%>�^�y�w$FORM_langauep2</td>">> tmp/personal.PID
else
 echo "<td width=25%>�@</td>"  >> /tmp/personal.PID
fi

if [ "$FORM_langauep3" ]; then
  echo "<td width=25%>$FORM_oterlang�w$FORM_langauep4</td>" >> 
/tmp/personal.PID
else
  echo "<td width=25%>�@</td>"  >> /tmp/personal.PID
fi

echo "</tr></table>" >> /tmp/personal.PID
echo "<p>" >> /tmp/personal.PID

#�P�_�ӤH�g���O�_�ݭn�⵲�G�ɤJ /tmp/personal.PID
#�p�G�ϥΪ̦b����ɡA�S���� include����, �h$FORM_include1 ���Ȭ� 
#null(�Ÿ��)
if [ $FORM_include1 ] ; then
  echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" 
>> /tmp/personal.PID

  echo "<tr><th><font size=+3>�� �g�@�@�@�@�@�� ��</font></th></tr>" 
>> /tmp/personal.PID

  echo "<tr><td><pre>$FORM_execp</pre>�@<p>�@<p></td></tr>" >> 
/tmp/personal.PID

  echo "</table>" >> /tmp/personal.PID
fi
 

#�P�_���x¾�ȬO�_�ݭn�⵲�G�ɤJ /tmp/personal.PID
#�B�z�覡�P�W�C�ۦP
if [ $FORM_include2 ] ; then
  echo "<p>" >> /tmp/personal.PID

  echo "<table border=3 cellspacing=3 cellpadding=3 width=100%>" 
>> /tmp/personal.PID

  echo "<tr><th>�@</th><th>���x¾��</th><td>$FORM_job_type</td>" 
>> /tmp/personal.PID

  echo "<th>�Ʊ�ݹJ</th><td>$FORM_money</td></tr>" >> 
/tmp/personal.PID

 echo "<tr><th>��<br>¾<br>��<br>��<br>�{<br>��<br>�P<br>��<br>�\
</th>" >> /tmp/personal.PID

  echo "<td colspan=4><pre>$FORM_understand</pre></td></tr>" >> 
/tmp/personal.PID

  echo "</table>" >> /tmp/personal.PID

fi 

echo "<p>" >> /tmp/personal.PID

echo "</center>" >> /tmp/personal.PID

#��̫᪺���G�A�v�@�ˬd��ϥΪ̨S����J��ƳB <td></td> ���� <td>
#�@</td>�A�H�Χ�<td colspan=2></td> ���� <td colspan=2>�@</td>�A�Y�b
#�G�̤����[�J�@�ӥ����ťղŸ��A�o�˦b�s�����W��ܮɡA������[�C
cat /tmp/personal.PID | awk '{print $0}' | sed 
s/'<td><\/td>'/'<td>�@<\/td>'/g|sed s/'<td colspan=2><\/td>'/'<td 
colspan=2>�@<\/td>'/g 

#��Ȧs�� /tmp/personal.PID �R��
\rm /tmp/personal.PID

