#!/bin/sh

#���o�{������ɪ�id
PID=`echo $$`
 

#���� QUERY_STRING �����ܼ�
eval `/usr/local/etc/httpd/cgi-bin/cgiparse -form`

#���w�t�� mail ��������|
mailprog=/usr/ucb/mail

#�ŧi������ɬ� HTML �榡����r��
echo "Content-type: text/html"
echo ""

#�N���G���s�� /tmp/letter.$PID �ɮ׸�
echo "�q�Ҫ̡G"   > /tmp/letter.$PID
echo ""  >> /tmp/letter.$PID

#���o�X�ȩҶ񪺦W�r�A�αq���ݩҶǨӪ��ϥΪ̦W�r(����T�������o)
echo "�ڪ��W�r�O[$FORM_name]($REMOTE_IDENT)" >> /tmp/letter.$PID

#�N¾�~���O���[��/tmp/letter.$PID
echo "�ڪ�¾�~�O[$FORM_job_type|$FORM_job]" >> /tmp/letter.$PID

#�N�q�l�H�c���[��/tmp/letter.$PID
echo "�q�l�H�c�O[$FORM_email]" >> /tmp/letter.$PID

#���o���ݨϥΪ̨ϥΪ��s������T�A�ê��[��/tmp/letter.$PID
echo "�ڨϥΪ�client�O--$HTTP_USER_AGENT" >> /tmp/letter.$PID

#���o���ݪ� hostname �� IP ��}�A���[��/tmp/letter.$PID
echo "�ڬO�Ӧ۩�:$REMOTE_HOST($REMOTE_ADDR)" >> /tmp/letter.$PID

#�N�ϥΪ̷N�����[��/tmp/letter.$PID
echo "��ı�o��j�ն�ͬ���T�����w[$FORM_judge]" >> /tmp/letter.$PID
echo "�ڪ��@�ǫب��G" >> /tmp/letter.$PID

#�N�ϥΪ̷N�����[��/tmp/letter.$PID
/www/cgi-bin/cgiparse -value comment >> /tmp/letter.$PID

#�N /tmp/letter.$PID �����e�H���޲z�� shock
$mailprog -s "From $FORM_name" shock < /tmp/letter.$PID
 

#�R�� /tmp/letter.$PID
rm -f /tmp/letter.$PID

#�N���U��T�ǵ����A��
echo "<title>Thank You !</title>"
echo "<center><br><br>"
echo "<h1>�n�� netscape 1.1n �~�i�H�ݨ�p�s��</h1>"
echo "<table border=3 celladding=5 cellpassing=5>"
echo "<td>Thank you for your suggestion<p>Have a nice 
day !</td>"
echo "<td><TABLE BORDER=5 CELLSPACING=5 CELLPADDING=5>"
echo "<TR><TD ALIGN=center>"
#�p�s���R�O�Ѻ����s�����Ҵ��Ѫ��u�W�ʵe�\��һs�@
echo "<IMG SRC=/cgi-bin/doit.cgi?/home/users/shock/image>"
echo "</TD></TR>"
echo "</TABLE></td><tr></table>"
echo "</CENTER>"
