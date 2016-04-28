## date: 28/04/2016
##
#!/bin/bash
#############################################
# INSTALL CONTIKI AND THE EXAMPLES MADE BY ME

cwd=$(pwd)
echo $cwd
cd $cwd
echo "Installing Contiki on your system"
##INSTALL CONTIKI
git clone https://github.com/contiki-os/contiki.git conti
cd conti/
git submodule sync && git submodule update --init
git pull
echo "succesful instalation of contiki"
cd examples
mkdir ictp
cd $cwd
echo "installing my examples"
##DOWNLOAD THE EXAMPLES
git clone https://github.com/jluzuria2001/TS-IT.git mqtools
echo "succesful instalation of the examples"
cd mqtools/ictp
mv * $cwd"/conti/examples/ictp"
cd ..
rm -r ictp
echo "moving finished"
