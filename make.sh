cd Starry
mkdir build
cd build
echo "--------- CMAKE STARRY ---------"
cmake ..
echo "--------- MAKE STARRY ---------"
make
echo "--------- INSTALL STARRY ---------"
cmake --install .
cd ../..
cd Editor
mkdir build
cd build
echo "--------- CMAKE EDITOR ---------"
cmake ..
echo "--------- MAKE EDITOR ---------"
make
cd ../..