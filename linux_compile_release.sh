
export COMPILATION_TYPE=Release

mkdir build

cmake -DCOMPILATION_TYPE=${COMPILATION_TYPE} . -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -O3" -B"build/${COMPILATION_TYPE}"

cmake --build "build/${COMPILATION_TYPE}" --config ${COMPILATION_TYPE} -- -j$(nproc)

cp build/${COMPILATION_TYPE}/Ray-it ./Ray-it_${COMPILATION_TYPE}

