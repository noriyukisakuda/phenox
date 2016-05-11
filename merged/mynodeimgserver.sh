kill %1
sleep 1
source /root/nodejs/nvm/nvm.sh/nvm.sh
nvm use v0.11.14
sleep 1
rm -f ~/nodejs/projects/imgserver/mysocket
node ~/nodejs/projects/imgserver/server.js &
sleep 3
mount /dev/mmcblk0p1 /mnt
~/phenox/work/control_git/main
#~/phenox/work/may/main
#~/phenox/work/nodeimgserver/main


