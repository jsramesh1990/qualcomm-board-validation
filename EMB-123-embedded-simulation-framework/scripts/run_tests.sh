#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Embedded Simulation Test Suite${NC}"
echo -e "${BLUE}  Branch: feature/EMB-123-embedded-simulation-framework${NC}"
echo -e "${BLUE}========================================${NC}"

# Create build directory
mkdir -p build_tests
cd build_tests

# Configure with tests enabled
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON

# Build
echo -e "${YELLOW}Building tests...${NC}"
make -j$(nproc)

# Run all tests
echo -e "${YELLOW}Running test suite...${NC}"
./embedded_sim --test

# Run with different configurations
echo -e "${YELLOW}Running stress tests...${NC}"
for i in {1..100}; do
    if [ $((i % 10)) -eq 0 ]; then
        echo "  Iteration $i/100"
    fi
    ./embedded_sim --quick-test > /dev/null 2>&1
done

# Generate coverage report
echo -e "${YELLOW}Generating coverage report...${NC}"
gcovr --html --html-details -o coverage.html
gcovr --xml -o coverage.xml

echo -e "${GREEN}✓ All tests passed!${NC}"
echo -e "${GREEN}✓ Coverage report: build_tests/coverage.html${NC}"

cd ..

# Run simulation test
echo -e "${YELLOW}Running simulation test...${NC}"
./build_tests/embedded_sim --mode=simulation --dashboard &
SIM_PID=$!
sleep 5
kill $SIM_PID

echo -e "${GREEN}✓ Simulation test completed${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}All tests and simulations successful!${NC}"
