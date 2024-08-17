#!/bin/bash

# 다운로드할 파일의 URL
DOWNLOAD_URL="https://github.com/username/repository/releases/download/v1.0/myproject-package.tar.gz"

# 설치할 디렉토리
INSTALL_DIR="/usr/local"

# 패키지 다운로드
echo "Downloading package..."
wget $DOWNLOAD_URL -O /tmp/myproject-package.tar.gz

# 패키지 압축 해제
echo "Extracting package..."
sudo tar -xzvf /tmp/myproject-package.tar.gz -C $INSTALL_DIR

# Clean up
rm /tmp/myproject-package.tar.gz

echo "Installation complete. You can now use the 'bob' command."

