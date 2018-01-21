with import <nixpkgs> {};
stdenv.mkDerivation {
  hardeningDisable = ["fortify"];
  name = "gdbast";
  buildInputs = [
    libdwarf
    libelf
    boost
    perlPackages.TestLeakTrace
    valgrind
    ncurses
    llvmPackages.llvm
    llvmPackages.clang-unwrapped
    cmake
  ];
  LDFLAGS="-ldwarf -Wl,-rpath=${libdwarf}/lib -lelf -Wl,-rpath=${libelf}/lib -lboost_filesystem -lboost_system -Wl,-rpath=${llvmPackages.clang-unwrapped}/lib -L${llvmPackages.clang-unwrapped}/lib -Wl,-rpath=${llvmPackages.llvm}/lib -L${llvmPackages.llvm}/lib";
  nativeBuildInputs = [
    pkgconfig
    gcc7
    #clang_5
  ];
}