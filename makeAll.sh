make clean
make -j12 Z3D=OOT3DE
echo Moving to ./Patch Files/EUR/3DS/code.ips
mv code.ips "./Patch Files/EUR/3DS/code.ips"

make clean
make -j12 Z3D=OOT3DE citra=1
echo Moving to ./Patch Files/EUR/Citra/code.ips
mv code.ips "./Patch Files/EUR/Citra/code.ips"

make clean
make -j12 Z3D=OOT3DJ
echo Moving to ./Patch Files/JP/3DS/code.ips
mv code.ips "./Patch Files/JP/3DS/code.ips"

make clean
make -j12 Z3D=OOT3DJ citra=1
echo Moving to ./Patch Files/JP/Citra/code.ips
mv code.ips "./Patch Files/JP/Citra/code.ips"

make clean
make -j12 Z3D=OOT3DU citra=1
echo Moving to ./Patch Files/USA/Citra/code.ips
mv code.ips "./Patch Files/USA/Citra/code.ips"

make clean
make -j12 Z3D=OOT3DU
echo Moving to ./Patch Files/USA/3DS/code.ips
mv code.ips "./Patch Files/USA/3DS/code.ips"