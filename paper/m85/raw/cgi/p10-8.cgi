#!/bin/sh
#�ŧi��X�� HTML �榡����r��
echo Content-type: text/plain
echo

#CGI/1.0 ���ճ��i
echo CGI/1.0 test script report:
echo

#�L�X test-cgi �᭱���ѼƭӼƤΩҦ��Ѽ�
echo argc is $#. argv is "$*".
echo
 

#�L�X���A���W�٤Ϊ���
echo SERVER_SOFTWARE = $SERVER_SOFTWARE

#�L�X���A���� hostname
echo SERVER_NAME = $SERVER_NAME

#�L�X CGI ������
echo GATEWAY_INTERFACE = $GATEWAY_INTERFACE

#�L�X���A�����ǿ��w
echo SERVER_PROTOCOL = $SERVER_PROTOCOL

#�L�X���A������}�A�j���O 80
echo SERVER_PORT = $SERVER_PORT

#�L�X Client �n�D�覡�A�� GET, POST, PUT �T��
echo REQUEST_METHOD = $REQUEST_METHOD

#HTTP �i�H�������榡
echo HTTP_ACCEPT = "$HTTP_ACCEPT"

#�L�X���ݨϥΪ��s�����W�٤Ϊ���
echo HTTP_USER_AGENT=$HTTP_USER_AGENT

#�L�X���{�����޿���|
echo SCRIPT_NAME = "$SCRIPT_NAME"

#�L�X QUERY_STRING �����ܼ�
echo QUERY_STRING = "$QUERY_STRING"

#�L�X���ݪ� hostname
echo REMOTE_HOST = $REMOTE_HOST

#�L�X���ݪ� IP address
echo REMOTE_ADDR = $REMOTE_ADDR

#�L�X���ݨϥΪ̪��W�r
echo REMOTE_USER = $REMOTE_USER

#�O�@���T�{�覡�A�p"Basic"
echo AUTH_TYPE = $AUTH_TYPE
 

#�L�X���媺����
echo CONTENT_TYPE = $CONTENT_TYPE

#�L�X���媺����
echo CONTENT_LENGTH = $CONTENT_LENGTH
