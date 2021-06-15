
export COMPILATION_TYPE=Debug

mkdir build

cmake -DCOMPILATION_TYPE=${COMPILATION_TYPE} . -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -g -D_GLIBCXX_DEBUG" -B"build/${COMPILATION_TYPE}"

cmake --build "build/${COMPILATION_TYPE}" --config ${COMPILATION_TYPE} -- -j$(nproc)

cp build/${COMPILATION_TYPE}/Ray-it ./Ray-it_${COMPILATION_TYPE}

