#!/bin/bash
set -e

if [ -z $1 ]; then
  echo missing input file && exit 1
fi

INPUT_FILE=$1
OUTPUT_DIR=$(dirname $INPUT_FILE)

OUTPUT_FILE="$OUTPUT_DIR/$(basename $INPUT_FILE .bin).patch.bin"
cp $INPUT_FILE $OUTPUT_FILE

TEMP_FILE="$OUTPUT_DIR/temp.bin"
tail -c +5 $INPUT_FILE > $TEMP_FILE

CRC32=$(crc32 $TEMP_FILE)
echo "CRC-32: 0x$CRC32"
SIZE=$(wc -c $TEMP_FILE | cut -d ' ' -f 1 | xargs -L1 printf '%x')
echo "size 0x$SIZE"

#rm $TEMP_FILE

echo "00000000: $(echo -n $CRC32 | tac -rs ..)" | xxd -r - $OUTPUT_FILE
echo "00000004: $(echo -n $SIZE | tac -rs ..)" | xxd -r - $OUTPUT_FILE

exit 0
