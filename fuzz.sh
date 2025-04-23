for i in $(seq 1 1000000);
do
cat /dev/urandom | head -n500 | netcat localhost 12345 &
echo 'done'
done
