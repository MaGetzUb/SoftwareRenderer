#!/bin/bash

os=$(uname -s)
arch=$(uname -m)

directory=$(ls | grep premake/)

# Linux system
if [ "$os" == "Linux" ]; then
	os="linux"
	echo "WARNING: Platform implementation for window & canvas class not implemented yet."
# Max OS X system
elif [ "$os" == "Darwin" ]; then
	os="macosx"
	echo "WARNING: Platform implementation for window & canvas class not implemented yet."
# Otherwise, assume Windows
else
	os="windows"
fi


# Windows setup
if [ "$os" == "windows" ]; then
	# Download premake executable
	$(curl -L -o "premake/premake5.zip" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha12/premake-5.0.0-alpha12-windows.zip")
	$(unzip -u -q "premake/premake5.zip" -d "premake/")
	$(rm -f "premake/premake5.zip")

# Linux setup
elif [ "$os" == "linux" ]; then
	# Determine whether we need to build from source or not
	if [ "$arch" == "x86_64" ]; then
		# Download premake executable
		$(curl -L -o "premake/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha12/premake-5.0.0-alpha12-linux.tar.gz")
		$(tar -xvzf "premake/premake5.tar.gz" -C "premake/")
		$(rm -f "premake/premake5.tar.gz")
	else
		# Download premake source package
		$(curl -L -o "premake/premake5-src.zip" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha12/premake-5.0.0-alpha12-src.zip")
		$(unzip -o "premake/premake5-src.zip" -d "premake/")

		# Build premake
		echo "Building premake from source.."
		$(make -C "premake/premake-5.0.0-alpha12/build/gmake.unix/")
		$(cp "premake/premake-5.0.0-alpha12/bin/release/premake5" "premake/")
		$(rm -rf "premake/premake-5.0.0-alpha12/")
	fi

	
	
# Mac OS X setup
elif [ "$os" == "osx" ]; then
	# Download premake executable
	$(curl -L -o "premake/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha12/premake-5.0.0-alpha12-macosx.tar.gz")
	$(tar -xvzf "premake/premake5.tar.gz" -C "premake/")
	$(rm -f "premake/premake5.tar.gz")
fi