cd Starry/build
echo CMAKE
cmake ..
echo MAKE
make
echo INSTALL
cmake --install .
cd ../..
cd Editor/build
echo CMAKE
cmake ..
echo MAKE
make
cd ../..