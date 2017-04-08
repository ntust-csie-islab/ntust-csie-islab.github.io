#!/bin/sh
#宣告輸出為 HTML 格式的文字檔
echo Content-type: text/plain
echo

#CGI/1.0 測試報告
echo CGI/1.0 test script report:
echo

#印出 test-cgi 後面的參數個數及所有參數
echo argc is $#. argv is "$*".
echo
 

#印出伺服器名稱及版本
echo SERVER_SOFTWARE = $SERVER_SOFTWARE

#印出伺服器的 hostname
echo SERVER_NAME = $SERVER_NAME

#印出 CGI 的版本
echo GATEWAY_INTERFACE = $GATEWAY_INTERFACE

#印出伺服器的傳輸協定
echo SERVER_PROTOCOL = $SERVER_PROTOCOL

#印出伺服器的埠址，大都是 80
echo SERVER_PORT = $SERVER_PORT

#印出 Client 要求方式，有 GET, POST, PUT 三種
echo REQUEST_METHOD = $REQUEST_METHOD

#HTTP 可以接受的格式
echo HTTP_ACCEPT = "$HTTP_ACCEPT"

#印出遠端使用的瀏覽器名稱及版本
echo HTTP_USER_AGENT=$HTTP_USER_AGENT

#印出本程式的邏輯路徑
echo SCRIPT_NAME = "$SCRIPT_NAME"

#印出 QUERY_STRING 環境變數
echo QUERY_STRING = "$QUERY_STRING"

#印出遠端的 hostname
echo REMOTE_HOST = $REMOTE_HOST

#印出遠端的 IP address
echo REMOTE_ADDR = $REMOTE_ADDR

#印出遠端使用者的名字
echo REMOTE_USER = $REMOTE_USER

#保護的確認方式，如"Basic"
echo AUTH_TYPE = $AUTH_TYPE
 

#印出本文的型式
echo CONTENT_TYPE = $CONTENT_TYPE

#印出本文的長度
echo CONTENT_LENGTH = $CONTENT_LENGTH
