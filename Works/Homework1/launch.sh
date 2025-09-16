#!/usr/bin/env bash

get_script_dir()
{
    local SOURCE_PATH="${BASH_SOURCE[0]}"
    local SYMLINK_DIR
    local SCRIPT_DIR
    # Resolve symlinks recursively
    while [ -L "$SOURCE_PATH" ]; do
        # Get symlink directory
        SYMLINK_DIR="$( cd -P "$( dirname "$SOURCE_PATH" )" >/dev/null 2>&1 && pwd )"
        # Resolve symlink target (relative or absolute)
        SOURCE_PATH="$(readlink "$SOURCE_PATH")"
        # Check if candidate path is relative or absolute
        if [[ $SOURCE_PATH != /* ]]; then
            # Candidate path is relative, resolve to full path
            SOURCE_PATH=$SYMLINK_DIR/$SOURCE_PATH
        fi
    done
    # Get final script directory path from fully resolved source path
    SCRIPT_DIR="$(cd -P "$( dirname "$SOURCE_PATH" )" >/dev/null 2>&1 && pwd)"
    echo "$SCRIPT_DIR"
}

if [[ $# -lt 1 ]]; then
	echo "Usage: $0 <loaders count>"
	exit 0
fi

bgIds=$(ps -aux | grep -E "Homework1.*\.py" | head -n -1 | awk '{print $2}')
delay=3

if [[ ! -z "$bgIds" ]]; then
	echo "Closing $(echo "$bgIds" | wc -l) jobs in backgound and sleep $delay secons"
	for job in $bgIds; do
		kill $job
		echo "kill $job"
	done
	sleep $delay
	echo "Lasted jobs: $(ps -aux | grep -E "Homework1.*\.py" | head -n -1 | awk '{print $2}')"
fi


loadersCount=$1
script_dir=$(get_script_dir)

echo "Create logs dir if not exists"
mkdir -p ./logs

echo "Starting proxies..."
unbuffer $script_dir/venv/bin/python $script_dir/proxyAck.py > ./logs/proxyAck &
unbuffer $script_dir/venv/bin/python $script_dir/proxyUrl.py > ./logs/proxyUrl &

echo "Proxy balancers is launched, wait $delay seconds"
sleep $delay

echo "Starting loaders..."

for i in $(seq 1 $loadersCount); do
	unbuffer $script_dir/venv/bin/python $script_dir/loader.py $loadersCount > ./logs/loader$i &
done

echo "Parsers launched"
