with import <nixpkgs> {};
stdenv.mkDerivation rec {
  hardeningDisable = ["fortify"];
  name = "gdbast";
  buildInputs = [
    libdwarf
    libelf
    boost
    perlPackages.TestLeakTrace
    valgrind
    ncurses
    #llvmPackages.llvm
    #llvmPackages.clang-unwrapped
    cmake
    meson
    ninja
  ];

  CLANG_LDFLAGS=''
  -lclangFrontendTool 
  -lclangFrontend 
  -lclangDriver 
  -lclangSerialization 
  -lclangCodeGen 
  -lclangParse 
  -lclangSema 
  -lclangEdit 
  -lclangAnalysis 
  -lclangIndex 
  -lclangRewrite 
  -lclangAST 
  -lclangLex 
  -lclangBasic 
  -lclang 
  -lLLVMLTO -lLLVMPasses -lLLVMObjCARCOpts -lLLVMSymbolize -lLLVMDebugInfoPDB -lLLVMDebugInfoDWARF 
  -lLLVMMIRParser -lLLVMCoverage -lLLVMTableGen -lLLVMDlltoolDriver -lLLVMOrcJIT -lLLVMXCoreDisassembler 
  -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo -lLLVMXCoreAsmPrinter -lLLVMSystemZDisassembler 
  -lLLVMSystemZCodeGen -lLLVMSystemZAsmParser -lLLVMSystemZDesc -lLLVMSystemZInfo -lLLVMSystemZAsmPrinter 
  -lLLVMSparcDisassembler -lLLVMSparcCodeGen -lLLVMSparcAsmParser -lLLVMSparcDesc -lLLVMSparcInfo 
  -lLLVMSparcAsmPrinter -lLLVMPowerPCDisassembler -lLLVMPowerPCCodeGen -lLLVMPowerPCAsmParser 
  -lLLVMPowerPCDesc -lLLVMPowerPCInfo -lLLVMPowerPCAsmPrinter -lLLVMNVPTXCodeGen -lLLVMNVPTXDesc 
  -lLLVMNVPTXInfo -lLLVMNVPTXAsmPrinter -lLLVMMSP430CodeGen -lLLVMMSP430Desc -lLLVMMSP430Info 
  -lLLVMMSP430AsmPrinter -lLLVMMipsDisassembler -lLLVMMipsCodeGen -lLLVMMipsAsmParser 
  -lLLVMMipsDesc -lLLVMMipsInfo -lLLVMMipsAsmPrinter -lLLVMLanaiDisassembler -lLLVMLanaiCodeGen 
  -lLLVMLanaiAsmParser -lLLVMLanaiDesc -lLLVMLanaiAsmPrinter -lLLVMLanaiInfo -lLLVMHexagonDisassembler 
  -lLLVMHexagonCodeGen -lLLVMHexagonAsmParser -lLLVMHexagonDesc -lLLVMHexagonInfo -lLLVMBPFDisassembler 
  -lLLVMBPFCodeGen -lLLVMBPFDesc -lLLVMBPFInfo -lLLVMBPFAsmPrinter -lLLVMARMDisassembler -lLLVMARMCodeGen 
  -lLLVMARMAsmParser -lLLVMARMDesc -lLLVMARMInfo -lLLVMARMAsmPrinter -lLLVMAMDGPUDisassembler 
  -lLLVMAMDGPUCodeGen -lLLVMAMDGPUAsmParser -lLLVMAMDGPUDesc -lLLVMAMDGPUInfo -lLLVMAMDGPUAsmPrinter 
  -lLLVMAMDGPUUtils -lLLVMAArch64Disassembler -lLLVMAArch64CodeGen -lLLVMAArch64AsmParser 
  -lLLVMAArch64Desc -lLLVMAArch64Info -lLLVMAArch64AsmPrinter -lLLVMAArch64Utils -lLLVMObjectYAML 
  -lLLVMLibDriver -lLLVMOption -lLLVMX86Disassembler -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMGlobalISel 
  -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMDebugInfoCodeView -lLLVMDebugInfoMSF -lLLVMX86Desc 
  -lLLVMMCDisassembler -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMMCJIT -lLLVMLineEditor 
  -lLLVMInterpreter -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMCodeGen -lLLVMTarget -lLLVMCoroutines 
  -lLLVMipo -lLLVMInstrumentation -lLLVMVectorize -lLLVMScalarOpts -lLLVMLinker -lLLVMIRReader -lLLVMAsmParser 
  -lLLVMInstCombine -lLLVMTransformUtils -lLLVMBitWriter -lLLVMAnalysis -lLLVMProfileData -lLLVMObject
  -lLLVMMCParser -lLLVMMC -lLLVMBitReader -lLLVMCore -lLLVMBinaryFormat -lLLVMSupport -lLLVMDemangle
  '';

  TMP_LLVM_CFLAGS='' \
    -I/home/dza/source/astgdb/tmp/built/var/empty/local/include \
    -fPIC -Werror=date-time -Wall -W -Wno-unused-parameter \
    -Wwrite-strings -Wno-missing-field-initializers -pedantic \
    -Wno-long-long -Wno-comment -g -DLLVM_BUILD_GLOBAL_ISEL \
    -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS \
    -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
  '';

  LLVM_LDFLAGS="-Wl,-rpath=${llvmPackages.clang-unwrapped}/lib -L${llvmPackages.clang-unwrapped}/lib -Wl,-rpath=${llvmPackages.llvm}/lib -L${llvmPackages.llvm}/lib";

  LDFLAGS="-ldwarf -Wl,-rpath=${libdwarf}/lib -lelf -Wl,-rpath=${libelf}/lib -lboost_filesystem -lboost_system ";
  #CXXFLAGS=TMP_LLVM_CFLAGS;
  #CFLAGS=CXXFLAGS;
  nativeBuildInputs = [
    pkgconfig
    gcc7
    #clang_5
  ];
  shellHook = ''
    export PATH=/home/dza/source/astgdb/tmp/built/var/empty/local/bin:$PATH
    export CFLAGS="$(llvm-config --cflags) $CFLAGS"
    export CXXFLAGS="$(llvm-config --cflags) $XXCFLAGS"
    export LDFLAGS="$(llvm-config --libs --ldflags) $LDFLAGS"
  '';
}