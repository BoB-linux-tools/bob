#!/bin/bash

# 설치할 디렉토리
INSTALL_DIR="/usr/local"
# 아스키 아트 출력
echo "██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗    ████████╗ ██████╗     ██████╗  ██████╗ ██████╗ "
echo "██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝    ╚══██╔══╝██╔═══██╗    ██╔══██╗██╔═══██╗██╔══██╗"
echo "██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗         ██║   ██║   ██║    ██████╔╝██║   ██║██████╔╝"
echo "██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝         ██║   ██║   ██║    ██╔══██╗██║   ██║██╔══██╗"
echo "╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗       ██║   ╚██████╔╝    ██████╔╝╚██████╔╝██████╔╝"
echo " ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝       ╚═╝    ╚═════╝     ╚═════╝  ╚═════╝ ╚═════╝      "

# ncurses 라이브러리 확인 함수
check_ncurses() {
    if ! ldconfig -p | grep -q 'libncurses'; then
        echo "ncurses library is not installed. Please install it using:"
        echo "  sudo apt-get install libncurses5"
        exit 1
    fi
}

# ncurses 라이브러리 확인
check_ncurses

# 파일 복사
echo "Installing executables..."
sudo cp -r bin/* $INSTALL_DIR/bin

echo "Installing library files..."
sudo cp -r lib/* $INSTALL_DIR/lib

echo "Installing documentation and manuals..."
sudo cp -r share/* $INSTALL_DIR/share

echo "Installing configuration files..."
sudo cp -r etc/* /etc/

# 라이브러리 캐시 업데이트
echo "Updating library cache..."
sudo ldconfig

echo "Installation complete. You can now use the 'bob' command."

