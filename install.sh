#!/bin/sh
cwd=`dirname "${0}"`
expr "${0}" : "/.*" > /dev/null || cwd=`(cd "${cwd}" && pwd)`
cd $cwd/src/qt

UNAME=$(uname)

if [ "$UNAME" = "Darwin" ] ; then

    ## Qt does not work well with xcode-select. need to install xcode from app store
    # install xcode
    # check=`xcode-\select --install 2>&1 >/dev/null`
    # str="xcode-select: note: install requested for command line developer tools"
    #
    # while [ "$check" = "$str" ];
    # do
    #     check=`xcode-\select --install 2>&1 >/dev/null`
    #     echo "waiting xcode installation..."
    #     sleep 20
    # done

    # prepare permission
    sudo chown -R $USER:admin /usr/local/
    # probably, two lines below are not necessary
    sudo chown -R $USER:admin /usr/local/bin/
    sudo chown -R $USER:admin /usr/local/Cellar/
    export PATH=/user/local/bin/:$PATH

    # install Homebrew
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    # brew tap homebrew/science

    # install qt5
    brew install qt5
    brew link --force --overwrite qt5

    # install R
    # these two lines are for handling bugs in R installation
    brew install gmp
    brew link --force --overwrite gmp
    brew install libpng
    brew link --force --overwrite libpng
    # install R
    brew install R
    brew link --force --overwrite R
    # install required package
    R -e 'if (!require(RInside)) { install.packages("RInside", repos="http://cran.us.r-project.org") }'

    # make
    make clean
    rm vibrain.pro.user
    qmake vibrain.pro -spec macx-clang CONFIG+=x86_64
    qmake
    make qmake_all
    make

    # move and open software
    rm -rf ../../vibrain.app
    mv vibrain.app ../../
    cd ../../

    open vibrain.app

elif [ "$UNAME" = "Linux" ] ; then
    # install qt5
    sudo apt-get update
    sudo apt-get install qt5-default build-essential libgl1-mesa-dev -y
    #sudo apt-get install qtcreator

    # install R
    sudo apt-get install r-base -y
    sudo chmod o+w /usr/local/lib/R/site-library
    sudo R -e 'if (!require(RInside)) { install.packages("RInside", repos="http://cran.us.r-project.org") }'

    # install GLUT
    sudo apt-get install freeglut3 freeglut3-dev -y
    sudo apt-get install binutils-gold -y

    # make
    make clean
    qmake vibrain.pro -r -spec linux-g++-64
    make

    # move and open software
    rm ../../vibrain
    mv vibrain ../../
    cd ../../

    ./vibrain

fi