#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
QT_VERSION="5.15.15"
QT_SRC_DIR="$SCRIPT_DIR/qt5-static-src"
QT_INSTALL_DIR="$SCRIPT_DIR/qt5-static"
JOBS="$(nproc)"

# Build dependencies (Debian/Ubuntu)
install_deps() {
    echo "==> Installing build dependencies..."
    sudo apt-get update
    sudo apt-get install -y build-essential perl python3 git \
        '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev \
        libxi-dev libxkbcommon-dev libxkbcommon-x11-dev \
        libfontconfig1-dev libfreetype-dev \
        libgl1-mesa-dev libglib2.0-dev \
        libharfbuzz-dev libicu-dev libpng-dev zlib1g-dev \
        libpcre2-dev libdouble-conversion-dev libzstd-dev
}

# Download Qt source (just qtbase, which contains Core/Gui/Widgets)
download_qt() {
    if [ -d "$QT_SRC_DIR" ]; then
        echo "==> Qt source already exists at $QT_SRC_DIR, skipping download"
        return
    fi
    echo "==> Downloading Qt $QT_VERSION source (qtbase only)..."
    mkdir -p "$QT_SRC_DIR"
    curl -L "https://download.qt.io/archive/qt/5.15/$QT_VERSION/submodules/qtbase-everywhere-opensource-src-$QT_VERSION.tar.xz" \
        | tar -xJ -C "$QT_SRC_DIR" --strip-components=1
}

# Build static Qt
build_qt() {
    if [ -f "$QT_INSTALL_DIR/bin/qmake" ]; then
        echo "==> Static Qt already built at $QT_INSTALL_DIR, skipping"
        return
    fi
    echo "==> Configuring static Qt $QT_VERSION..."
    cd "$QT_SRC_DIR"

    ./configure \
        -static \
        -release \
        -prefix "$QT_INSTALL_DIR" \
        -opensource -confirm-license \
        -nomake examples \
        -nomake tests \
        -no-dbus \
        -no-sql-sqlite \
        -no-openssl \
        -fontconfig \
        -system-freetype \
        -system-harfbuzz \
        -system-zlib \
        -system-libpng \
        -xcb \
        -bundled-xcb-xinput

    echo "==> Building static Qt (using $JOBS cores)..."
    make -j"$JOBS"

    echo "==> Installing static Qt to $QT_INSTALL_DIR..."
    make install

    cd "$SCRIPT_DIR"
}

# Build qtcode against static Qt
build_app() {
    echo "==> Building qtcode with static Qt..."
    cd "$SCRIPT_DIR"

    # Clean previous build artifacts
    rm -f *.o moc_*.cpp moc_predefs.h qtcode Makefile .qmake.stash

    "$QT_INSTALL_DIR/bin/qmake" qtcode.pro CONFIG+=static
    make -j"$JOBS"

    echo ""
    echo "==> Build complete: $SCRIPT_DIR/qtcode"
    echo "==> Binary size: $(du -h qtcode | cut -f1)"
    echo "==> Dynamic dependencies:"
    ldd qtcode 2>/dev/null || echo "    (fully static binary)"
}

case "${1:-all}" in
    deps)
        install_deps
        ;;
    download)
        download_qt
        ;;
    qt)
        download_qt
        build_qt
        ;;
    app)
        build_app
        ;;
    all)
        install_deps
        download_qt
        build_qt
        build_app
        ;;
    clean)
        echo "==> Cleaning build artifacts..."
        cd "$SCRIPT_DIR"
        rm -f *.o moc_*.cpp moc_predefs.h qtcode Makefile .qmake.stash
        ;;
    distclean)
        echo "==> Removing static Qt build and source..."
        rm -rf "$QT_SRC_DIR" "$QT_INSTALL_DIR"
        cd "$SCRIPT_DIR"
        rm -f *.o moc_*.cpp moc_predefs.h qtcode Makefile .qmake.stash
        ;;
    *)
        echo "Usage: $0 {deps|download|qt|app|all|clean|distclean}"
        echo ""
        echo "  deps       - Install build dependencies"
        echo "  download   - Download Qt $QT_VERSION source"
        echo "  qt         - Download + build static Qt"
        echo "  app        - Build qtcode (requires static Qt already built)"
        echo "  all        - Full build: deps + Qt + app (default)"
        echo "  clean      - Remove build artifacts"
        echo "  distclean  - Remove everything including static Qt"
        exit 1
        ;;
esac
