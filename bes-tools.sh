build() {
    exec zig c++ src/main.cpp  -target x86_64-windows -o build/bes.exe
}

"$@"