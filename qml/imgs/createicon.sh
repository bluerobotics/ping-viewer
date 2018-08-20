if [ -z "$1" ]; then
    echo "No argument supplied: createicon.sh file.svg"
    exit 1
fi

echo "Check for ImageMagick:"
# It's necessary to have librsvg in ImageMagick
# brew install imagemagick --with-librsvg
convert --version
echo "Check for iconutil:"
iconutil --help
echo "Start image processing..."

inputfile=$1
icnstempfolder=/tmp/icon.iconset
# macos files
echo "ICNS for macOS:"
if [ -d $icnstempfolder ]; then
    echo "ICNS temporary folder already exist, folder will be removed."
    rm -rf $icnstempfolder
fi

echo "Create temporary folder."
mkdir $icnstempfolder

echo "Convert files.."
for size in 1024 512 256 64 32; do
    echo "icon_${size}x${size}.png"
    convert -background none -size ${size}x${size} $inputfile "${icnstempfolder}/icon_${size}x${size}.png"
done
echo "Create icns file."
iconutil --convert icns $icnstempfolder

if [ -d /tmp/icon.icns ]; then
    echo "ICNS does not exist!"
    exit 1
fi

echo "Move ICNS file to deploy folder."
mv /tmp/icon.icns ../../deploy