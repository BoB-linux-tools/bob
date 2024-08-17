#!/bin/bash

# 스크립트 실행 전에 확인할 것
if [[ $EUID -ne 0 ]]; then
   echo "이 스크립트는 root 권한으로 실행해야 합니다. 'sudo'를 사용하여 다시 시도해 주세요." 1>&2
   exit 1
fi

# 이미지 파일 목록 가져오기
background_files=(../resource/background*.jpg)
logo_files=(../resource/logo*.png)
outline_files=(../resource/outline*.png)

# 파일 개수 확인
background_count=${#background_files[@]}
logo_count=${#logo_files[@]}
outline_count=${#outline_files[@]}

# 랜덤 숫자 생성
random_background=$((RANDOM % background_count + 1))
random_logo=$((RANDOM % logo_count + 1))
random_outline=$((RANDOM % outline_count + 1))

# 선택된 파일 설정
new_kali_aqua_16x19="../resource/background${random_background}.jpg"
new_kali_logo="../resource/logo${random_logo}.png"
new_outline="../resource/outline${random_outline}.png"

# 파일 포맷 확인
if [[ ! "$new_kali_logo" =~ \.png$ ]] || [[ ! "$new_kali_aqua_16x19" =~ \.jpg$ ]]; then
    echo "파일은 PNG 또는 JPG 포맷이어야 합니다." 1>&2
    exit 1
fi

# 원본 파일이 있는 디렉토리로 이동
cd /usr/share/plymouth/themes/kali || { echo "디렉토리 이동 실패"; exit 1; }

# 파일 복사
cp "$new_kali_logo" kali-logo.png
cp "$new_kali_logo" kali-logo-container.png
cp "$new_kali_logo" kali-logo-container_small.png
cp "$new_kali_logo" kali-logo-fade.png
cp "$new_kali_logo" kali-logo-fade_small.png
cp "$new_kali_logo" kali-logo_small.png
cp "$new_outline" outline.png
cp "$new_outline" outline_small.png
cp "$new_kali_aqua_16x19" kali-aqua-16x9.jpg
cp "$new_kali_aqua_16x19" kali-ferrofluid-16x9.jpg

# 권한 설정
chown root:root kali-logo.png
chown root:root kali-logo-container.png
chown root:root kali-logo-container_small.png
chown root:root kali-logo-fade.png
chown root:root kali-logo-fade_small.png
chown root:root kali-logo_small.png
chown root:root kali-aqua-16x9.jpg
chown root:root kali-ferrofluid-16x9.jpg
chown root:root outline.png
chown root:root outline_small.png

# plymouth 재설정
update-initramfs -u

# 완료 메시지
echo "로고 변경이 완료되었습니다. 시스템을 재부팅하여 변경 사항을 확인해 주세요."
