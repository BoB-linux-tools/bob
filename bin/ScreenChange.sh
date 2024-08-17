#!/bin/bash

# 스크립트 실행 전에 root 권한 확인
if [[ $EUID -ne 0 ]]; then
    echo "이 스크립트는 root 권한으로 실행해야 합니다. 'sudo'를 사용하여 다시 시도해 주세요." 1>&2
    exit 1
fi

# /etc/os-release 파일에서 배포판 이름 읽기
os_name=$(grep "^ID=" /etc/os-release | cut -d'=' -f2 | tr -d '"')

# 현재 디렉토리의 절대경로 가져오기
script_dir=$(pwd)

# 배포판 판별 및 실행
if [ "$os_name" == "kali" ]; then
    echo "This is Kali Linux."

    # 이미지 파일 목록 가져오기 (Kali)
    wallpaper_files=("${script_dir}/../resource/screan"*.png)
    
    # 파일 개수 확인
    wallpaper_count=${#wallpaper_files[@]}
    
    # 랜덤 숫자 생성
    random_wallpaper=$((RANDOM % wallpaper_count))

    # 선택된 파일 설정
    WALLPAPER_PATH="${wallpaper_files[$random_wallpaper]}"

    # 모든 모니터와 워크스페이스에 대해 배경화면 변경
    MONITORS=$(xfconf-query -c xfce4-desktop -p /backdrop -lv | grep last-image | awk -F '/' '{print $4}' | uniq)
    
    for MONITOR in $MONITORS; do
        xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/last-image --create -t string -s "$WALLPAPER_PATH"
        xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/image-style --create -t int -s 4
    done

    echo "Kali 배경화면이 변경되었습니다."

elif [ "$os_name" == "ubuntu" ]; then
    echo "This is Ubuntu."

    # 이미지 파일 목록 가져오기 (Ubuntu)
    screen_files=("${script_dir}/../resource/screen"*.png)
    
    # 파일 개수 확인
    screen_count=${#screen_files[@]}
    
    # 랜덤 숫자 생성
    random_screen=$((RANDOM % wallpaper_count))

    # 선택된 파일 설정
    WALLPAPER_PATH="${screen_files[$random_screen]}"

    # GNOME 설정을 사용하여 배경화면 변경
    gsettings set org.gnome.desktop.background picture-uri "file://$WALLPAPER_PATH"
    
    echo "Ubuntu 배경화면이 변경되었습니다."

else
    echo "This script is only designed for Kali Linux and Ubuntu. Detected OS: $os_name"
    exit 1
fi
