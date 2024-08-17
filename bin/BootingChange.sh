#!/bin/bash

# 스크립트 실행 전에 확인할 것
if [[ $EUID -ne 0 ]]; then
   echo "이 스크립트는 root 권한으로 실행해야 합니다. 'sudo'를 사용하여 다시 시도해 주세요." 1>&2
   exit 1
fi

# 새 이미지 파일 경로 설정
new_kali_logo="/home/kali/bob/kali-logo.png"
new_kali_logo_container="/home/kali/bob/kali-logo-container.png"
new_kali_logo_container_small="/home/kali/bob/kali-logo-container-small.png"
new_kali_logo_fade="/home/kali/bob/kali-logo-fade.png"
new_kali_logo_fade_small="/home/kali/bob/kali-logo-fad_small.png"
new_kali_logo_small="/home/kali/bob/kali-logo_small.png"
new_kali_aqua_16x19="/home/kali/bob/lobing.jpg"
new_kali_ferrofluid_16x9="/home/kali/bob/lobing.jpg"
new_outline="/home/kali/bob/outline.png"

# 파일 포맷 확인
if [[ ! "$new_kali_logo" =~ \.png$ ]] || [[ ! "$new_kali_logo_container" =~ \.png$ ]] || [[ ! "$new_kali_logo_container_small" =~ \.png$ ]] || [[ ! "$new_kali_logo_fade" =~ \.png$ ]] || [[ ! "$new_kali_logo_fade_small" =~ \.png$ ]] || [[ ! "$new_kali_logo_small" =~ \.png$ ]] || [[ ! "$new_kali_aqua_16x19" =~ \.jpg$ ]] || [[ ! "$new_kali_ferrofluid_16x9" =~ \.jpg$ ]]; then
    echo "파일은 PNG 또는 JPG 포맷이어야 합니다." 1>&2
    exit 1
fi

# 원본 파일이 있는 디렉토리로 이동
cd /usr/share/plymouth/themes/kali || { echo "디렉토리 이동 실패"; exit 1; }

# 기존 파일 백업
cp kali-logo.png kali-logo.png.bak
cp kali-logo-container.png kali-logo-container.png.bak
cp kali-logo-container_small.png kali-logo-container_small.png.bak
cp kali-logo-fade.png kali-logo-fade.png.bak
cp kali-logo-fade_small.png kali-logo-fade_small.png.bak
cp kali-aqua-16x9.jpg kali-aqua-16x9.jpg.bak
cp kali-ferrofluid-16x9.jpg kali-ferrofluid-16x9.jpg.bak
cp outline.png outline.png.bak
cp outline_small.png outline_small.png.bak

# 새 파일로 교체
cp "$new_kali_logo" kali-logo.png
cp "$new_kali_logo_container" kali-logo.png
cp "$new_kali_logo_container_small" kali-logo.png
cp "$new_kali_logo_fade" kali-logo.png
cp "$new_kali_logo_fade_small" kali-logo.png
cp "$new_kali_logo_small" kali-logo.png
cp "$new_kali_aqua_16x19" lobing.jpg
cp "$new_kali_ferrofluid_16x9" lobing.jpg
cp "$new_outline" outline.png
cp "$new_outline" outline_small.png

cp "$new_kali_logo" kali-logo.png
cp "$new_kali_logo" kali-logo-container.png
cp "$new_kali_logo" kali-logo-container_small.png
cp "$new_kali_logo" kali-logo-fade.png
cp "$new_kali_logo" kali-logo-fade_small.png
cp "$new_kali_logo" kali-logo_small.png
cp "$new_outline" outline.png
cp "$new_outline" outline_small.png
cp "$new_kali_aqua_16x19" kali-aqua-16x9.jpg
cp "$new_kali_ferrofluid_16x9" kali-ferrofluid-16x9.jpg

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

