#!/bin/bash

HOST=localhost
PORT=8080
PATH=/person

for i in "$@"; do
  case $i in
    --host=*)
      HOST="${i#*=}"
      shift
      ;;
    --port=*)
      PORT="${i#*=}"
      shift
      ;;
    --path=*)
      PATH="${i#*=}"
      shift
      ;;
    --*)
      echo "Unknown option $i"
      exit 1
      ;;
    *)
      ;;
  esac
done

TESTED_URL_REQ=http://$HOST:$PORT$PATH

function assert {
    
    local RED="\e[31m"
    local GREEN="\e[32m"
    local ENDCOLOR="\e[0m"

    local OUTPUT=$1
    local SUB_STR=$2

    if [[ "$OUTPUT" =~ .*"$SUB_STR".* ]]; then
        echo -e " ${GREEN}OK${ENDCOLOR}"
    else
        echo -e " ${RED}Fail${ENDCOLOR}"
    fi

}

function GetLoginById {

    local EXPECTED_LOGIN='"login":'\"$1\"

    local QUERY_STRING=login=$1
    local REQ=${TESTED_URL_REQ}?${QUERY_STRING}

    local OUTPUT=$(/usr/bin/curl -s ${REQ} 2>&1)
    [[ -z "$OUTPUT" ]] && { echo "RETURN nothing"; exit 1; }
        
    assert $OUTPUT $EXPECTED_LOGIN
}

function LookingFor {
    
    local FIRST_NAME=$1
    local LAST_NAME=$2

    local EXPECTED_LOGIN='"login":'\"$3\"

    local QUERY_STRING="first_name=${FIRST_NAME}&last_name=${LAST_NAME}"
    local REQ=${TESTED_URL_REQ}?${QUERY_STRING}

    local OUTPUT=$(/usr/bin/curl -s ${REQ} 2>&1)
    [[ -z "$OUTPUT" ]] && { echo "RETURN nothing"; exit 1; }

    assert $OUTPUT $EXPECTED_LOGIN
}

function Insert {

    local OUTPUT=`/usr/bin/curl -s \
      --header "Content-Type: application/json" \
      --request POST --data @test.json \
      $TESTED_URL_REQ`
    [[ -z "$OUTPUT" ]] && { echo "RETURN nothing"; exit 1; }

    assert $OUTPUT 'OK'
}

function HttpGetLogin { GetLoginById biba2007thebest; }
function HttpGetMask { LookingFor Boba Bibovich boba; }
function HttpPost { Insert; }

InitTests=(
  HttpGetLogin
  HttpGetMask
  HttpPost
)

for i in ${!InitTests[@]}; do
  printf "Test name: %s" ${InitTests[$i]}
  ${InitTests[$i]}
done
