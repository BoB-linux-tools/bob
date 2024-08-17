#!/bin/bash

# 설정 파일 생성 위치
SETTINGS_FILE="./.bob_settings"

# curl, jq 및 Python 설치 확인 및 필요시 설치 안내
check_and_install_dependencies() {
    if ! command -v curl &> /dev/null; then
        echo "curl is not installed. Installing curl..."
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            sudo apt-get update
            sudo apt-get install -y curl
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            brew install curl
        else
            echo "Unsupported OS. Please install curl manually."
            exit 1
        fi
    fi

    if ! command -v jq &> /dev/null; then
        echo "jq is not installed. Installing jq..."
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            sudo apt-get update
            sudo apt-get install -y jq
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            brew install jq
        else
            echo "Unsupported OS. Please install jq manually."
            exit 1
        fi
    fi

    if ! command -v python3 &> /dev/null; then
        echo "Python3 is not installed. Installing Python3..."
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            sudo apt-get update
            sudo apt-get install -y python3
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            brew install python3
        else
            echo "Unsupported OS. Please install Python3 manually."
            exit 1
        fi
    fi
}

# 설정 파일 로드 또는 생성
load_settings() {
    if [ -f "$SETTINGS_FILE" ]; then
        source "$SETTINGS_FILE"
    fi
}

# API 키 설정
set_api_key() {
    local api_key="$1"
    echo "API_KEY=\"$api_key\"" > "$SETTINGS_FILE"
    echo "API key has been set successfully."
}

# API 키 가져오기
get_api_key() {
    if [ -z "$API_KEY" ]; then
        echo "Error: API key not found. Please set your API key using './bob --codecheck --setting \"your_api_key\"'."
        exit 1
    else
        echo "$API_KEY"
    fi
}

# 코드 검사를 수행하는 함수
codecheck() {
    local api_key=$(get_api_key)

    # 파일 경로 입력 확인
    if [ -z "$1" ]; then
        echo "Error: Please provide the path to the source code file."
        exit 1
    fi

    # 소스 코드 파일 읽기
    local filepath="$1"
    if [ ! -f "$filepath" ]; then
        echo "Error: File '$filepath' not found."
        exit 1
    fi

    # Python을 사용하여 파일 내용을 JSON 문자열로 변환
    local escaped_source_code=$(python3 -c "
import json, sys
import codecs

# 파일을 바이너리 모드로 읽기
with open('$filepath', 'rb') as file:
    input_data = file.read()

try:
    # UTF-8로 디코딩 시도
    decoded_data = input_data.decode('utf-8')
except UnicodeDecodeError:
    # UTF-8 디코딩이 실패할 경우, ISO-8859-1로 디코딩 시도
    decoded_data = input_data.decode('ISO-8859-1')

# JSON 문자열로 변환하여 출력
print(json.dumps(decoded_data, ensure_ascii=False))
")


    # GPT에 보낼 프롬프트 설정
    local json_data=$(cat <<EOF
{
    "model": "gpt-3.5-turbo",
    "messages": [
        {
            "role": "system",
            "content": "당신은 이경문이라는 이름의 한국의 소프트웨어 개발자입니다. 당신은 BoB(Best of Best)라는 한국 최고의 보안 교육 프로그램의 보안제품개발트랙의 멘토입니다. 당신은 잘생겼고 개발을 매우 잘합니다. 그리고 현재 교육생을 가르치고 있습니다. 교육생들이 소스코드를 당신에게 보내줄 것인데, 다음과 같은 기준으로 평가해주세요:\n1. 코드 작동 여부: 10점 만점에 몇점\n2. 가독성: 10점 만점에 몇점\n3. 주석(설명) 존재 여부: 10점 만점에 몇점\n4. 코드 구조화: 10점 만점에 몇점\n5. 코딩 스타일: 10점 만점에 몇점\n6. 오류 처리: 10점 만점에 몇점\n7. 효율성: 10점 만점에 몇점\n8. 확장성: 10점 만점에 몇점\n9. 보안: 10점 만점에 몇점\n10. 테스트: 10점 만점에 몇점\n총 평가 점수: 10개 항목 더한 점수\n를 출력하고 종합 평가를 출력합니다. 종합 평가: 평가 항목에 대한 평가 이유를 간단하게 출력\n 또한 한칸 아래에 한줄평을 출력하고 그곳에는 높은순으로 잘했어요, 아쉬워요, 때찌!!!, 이노무씨끼!!!! 중 하나를 무조건 출력해주세요. 무조건해야해요. 종합 평가와 한줄평은 서로 별개이며 둘다 출력해야하며 종합 평가는 점수에 대한 이유와 향후 고치면 좋은 점을 출력하여주고 그 이후 최종 한문장으로 한줄평을 출력합니다."
        },
        {
            "role": "user",
            "content": $escaped_source_code
        }
    ]
}
EOF
)

    # 디버깅: JSON 데이터를 출력 (주석 처리됨)
    #echo "Sending JSON data to API..."
    #echo "$json_data"

    # API 요청
    local response=$(curl -s -w "\n%{http_code}" -X POST https://api.openai.com/v1/chat/completions \
        -H "Content-Type: application/json" \
        -H "Authorization: Bearer $api_key" \
        -d "$json_data")

    # HTTP 상태 코드와 응답 분리
    local http_code=$(echo "$response" | tail -n1)
    local response_body=$(echo "$response" | sed '$d')

    if [ "$http_code" == "200" ]; then
        # content 필드만 추출하여 출력
        local content=$(echo "$response_body" | jq -r '.choices[0].message.content')
        echo "$content"
    else
        echo "Error: Received HTTP status code $http_code"
        echo "Full Response: $response_body"
    fi
}

# 스크립트 메인 함수
main() {
    # curl, jq, Python 설치 확인 및 설치
    check_and_install_dependencies

    # 설정 파일 로드
    load_settings

    # API 키가 있는지 먼저 확인
    if [ -z "$API_KEY" ]; then
        if [ "$1" == "--codecheck" ]; then
            if [ "$2" == "--setting" ] && [ -n "$3" ]; then
                set_api_key "$3"
            else
                echo "Usage: bob --codecheck --setting \"gpt-api-key\""
            fi
        else
            echo "Invalid command. Usage: bob --codecheck --setting \"gpt-api-key\""
        fi
    else
        if [ "$1" == "--codecheck" ]; then
            if [ -n "$2" ]; then
                codecheck "$2"
            else
                echo "Usage: bob --codecheck <source_file>"
            fi
        else
            echo "Invalid command. Usage: bob --codecheck <source_file>"
        fi
    fi
}

main "$@"
