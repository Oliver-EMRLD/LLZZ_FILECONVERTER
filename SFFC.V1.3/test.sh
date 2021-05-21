#!/bin/zsh
export DD_FILEINP="/home/oliver/Desktop/x"
export DD_FILEOUT="/home/oliver/Desktop/out"
./a.out CVT2NTHE FIXED NO NO NO YES 4

export DD_FILEINP="/home/oliver/tst/inputfile"
export DD_FILEOUT="/home/oliver/tst/out"
./a.out CVT2NTHE LLZZ NO NO NO NO


export DD_FILEINP="/home/oliver/tst/inputfile"
export DD_FILEOUT="/home/oliver/tst/hex"
./a.out CVT2NTHE LLZZ NO NO YES NO


export DD_FILEINP="/home/oliver/tst/out"
export DD_FILEOUT="/home/oliver/tst/outbak"
./a.out CVT4NTHE FIXED NO NO NO NO


export DD_FILEINP="/home/oliver/tst/hex"
export DD_FILEOUT="/home/oliver/tst/hexbak"
./a.out CVT4NTHE FIXED NO NO YES NO
