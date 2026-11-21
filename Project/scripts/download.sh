#!/bin/bash
# Downloads the .dly file for every station in a station list.
# Usage: scripts/download.sh stations_test.txt
LIST=$1
URL=https://www.ncei.noaa.gov/pub/data/ghcn/daily/all
mkdir -p data
total=$(wc -l < "$LIST")
n=0
while read -r id region; do
    n=$((n + 1))
    if [ -f "data/$id.dly" ]; then
        echo "[$n/$total] $id already downloaded"
        continue
    fi
    echo "[$n/$total] downloading $id ($region)"
    wget -q -O "data/$id.dly" "$URL/$id.dly" || echo "  FAILED: $id"
    sleep 0.2      # small pause to be polite to NOAA's server
done < "$LIST"
