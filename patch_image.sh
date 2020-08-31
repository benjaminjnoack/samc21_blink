#!/bin/bash
set -e

if [ -z $1 ]; then
  echo missing input file && exit 1
fi

INPUT_FILE=$1
OUTPUT_DIR=$(dirname $INPUT_FILE)
# the output file is the final build artifact
OUTPUT_FILE="$OUTPUT_DIR/$(basename $INPUT_FILE .bin).patch.bin"
cp $INPUT_FILE $OUTPUT_FILE
# a temp file is used because the CRC cannot be calculated on a file which contains the CRC word!
TEMP_FILE="$OUTPUT_DIR/temp.bin"
tail -c +5 $INPUT_FILE > $TEMP_FILE
# the size of the image is equal to the number of bytes the CRC was calculated on
SIZE=$(wc -c $TEMP_FILE | cut -d ' ' -f 1 | xargs -L1 printf '%x')
echo -e "size:\t0x$SIZE"
# the size word must be written to the temp artifact to ensure the CRC is the same the bootloader will calculate
echo "00000000: $(echo -n $SIZE | tac -rs ..)" | xxd -r - $TEMP_FILE
CRC32=$(crc32 $TEMP_FILE)
echo -e "crc32:\t0x$CRC32"
rm $TEMP_FILE
# write the CRC and size into the final build artifact
echo "00000000: $(echo -n $CRC32 | tac -rs ..)" | xxd -r - $OUTPUT_FILE
echo "00000004: $(echo -n $SIZE | tac -rs ..)" | xxd -r - $OUTPUT_FILE

exit 0
