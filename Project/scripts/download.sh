#!/bin/bash
# Downloads the .dly file for every station in a station list.
# Usage: scripts/download.sh stations_test.txt
#
# Each file is first saved as data/<id>.dly.part and only renamed to
# data/<id>.dly when wget finishes successfully. So if the download is
# stopped half way, we never end up with a cut-off .dly file that looks
# complete (a failed or partial .part file is deleted).
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
    if wget -q -O "data/$id.dly.part" "$URL/$id.dly"; then
        mv "data/$id.dly.part" "data/$id.dly"     # complete -> real name
    else
        rm -f "data/$id.dly.part"                  # remove the broken piece
        echo "  FAILED: $id"
    fi
    sleep 0.2      # small pause to be polite to NOAA's server
done < "$LIST"
