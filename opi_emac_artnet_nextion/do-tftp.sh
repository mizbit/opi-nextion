
echo '!tftp#1' | nc -u -p 10501 -w 1 192.168.2.250 10501
echo '?tftp#' | nc -u -p 10501 -w 1 192.168.2.250 10501

tftp 192.168.2.250 << -EOF
binary
put orangepi_zero.uImage
quit
-EOF

echo '!tftp#0' | nc -u -p 10501 -w 1 192.168.2.250 10501
echo '?tftp#' | nc -u -p 10501 -w 1 192.168.2.250 10501

echo '?reboot##' | nc -u -p 10501 -w 1 192.168.2.250 10501
