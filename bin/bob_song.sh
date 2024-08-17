#!/bin/bash

# 필요한 도구 설치 확인 및 필요시 설치 안내
check_and_install_dependencies() {
    if ! command -v sox &> /dev/null; then
        echo "sox is not installed. Installing sox..."
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            sudo apt-get update
            sudo apt-get install -y sox
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            brew install sox
        else
            echo "Unsupported OS. Please install sox manually."
            exit 1
        fi
    fi
}

# WAV 파일을 생성하는 함수
create_tone_wav() {
    freq=$1
    duration=$2
    filename=$3

    # 지속 시간을 초 단위로 변환하여 synth에 전달
    duration_in_seconds=$(echo "scale=2; $duration / 1000" | bc)

    # 간단한 sine wave 톤 생성 (주파수에 맞춘 데이터 생성)
    sox -n -r 44100 -c 1 "${filename}" synth "${duration_in_seconds}" sine "${freq}"
}

# 멜로디를 생성하는 함수
create_melody_wav() {
    echo "Creating melody..."
    
    # "Happy Birthday" 노래의 멜로디에 맞는 notes와 durations 배열 정의
    notes=(
        264 264 297 264 352 330 # Happy Birthday to You
        264 264 297 264 396 352 # Happy Birthday to You
        264 264 528 440 352 330 297 # Happy Birthday dear [Name]
        466 466 440 352 396 352 # Happy Birthday to You
    )
    durations=(
        500 500 1000 1000 1000 2000 # 첫 줄
        500 500 1000 1000 1000 2000 # 둘째 줄
        500 500 1000 1000 1000 1000 2000 # 셋째 줄
        500 500 1000 1000 1000 2000 # 마지막 줄
    )
    
    for i in "${!notes[@]}"; do
        create_tone_wav "${notes[$i]}" "${durations[$i]}" "note_${i}.wav"
    done

    # 멜로디를 하나의 파일로 결합
    sox $(for i in "${!notes[@]}"; do echo "note_${i}.wav"; done) melody.wav
}

# 비프음을 재생하는 함수
play_melody() {
    echo "Playing melody..."
    sox melody.wav -d
}

# 정리 함수
cleanup() {
    echo "Cleaning up..."
    rm -f note_*.wav melody.wav
}

# 스크립트 실행
check_and_install_dependencies
create_melody_wav
play_melody
cleanup

