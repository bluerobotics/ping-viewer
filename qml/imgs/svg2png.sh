for file in *.svg; do
    convert -background none $file "${file%%.*}.png"
done

