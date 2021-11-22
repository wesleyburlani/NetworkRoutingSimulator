rm -Rf improvc
mkdir improvc

rm -Rf bin 
mkdir bin

sudo apt install -y gnome-terminal

wget https://github.com/wesleyburlani/improvc/archive/master.zip
unzip master.zip
mv improvc-master/* improvc
rm -R improvc-master
rm master.zip