#!/bin/bash

# 설정할 배경화면 이미지의 절대 경로
WALLPAPER_PATH="/home/kali/Downloads/aa.png"

# 모든 모니터와 워크스페이스에 대해 배경화면 변경
MONITORS=$(xfconf-query -c xfce4-desktop -p /backdrop -lv | grep last-image | awk -F '/' '{print $4}' | uniq)

for MONITOR in $MONITORS; do
    xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/last-image --create -t string -s "$WALLPAPER_PATH"
    xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/image-style --create -t int -s 4
done
#!/bin/bash

# 설정할 배경화면 이미지의 절대 경로
WALLPAPER_PATH="/home/kali/Downloads/aa.png"

# 모든 모니터와 워크스페이스에 대해 배경화면 변경
MONITORS=$(xfconf-query -c xfce4-desktop -p /backdrop -lv | grep last-image | awk -F '/' '{print $4}' | uniq)

for MONITOR in $MONITORS; do
    xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/last-image --create -t string -s "$WALLPAPER_PATH"
    xfconf-query -c xfce4-desktop -p /backdrop/$MONITOR/workspace0/image-style --create -t int -s 4
done

