#!/bin/bash

# Set script and project paths to absolute paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$(dirname "${SCRIPT_DIR}")" && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${BLUE}$1${NC}"
}

print_success() {
    echo -e "${GREEN}$1${NC}"
}

print_warning() {
    echo -e "${YELLOW}$1${NC}"
}

print_error() {
    echo -e "${RED}$1${NC}" >&2
}

# Check if clang-tidy is installed
if ! command -v clang-tidy &> /dev/null; then
    print_error "Error: clang-tidy is not installed. Please install it first."
    exit 1
fi

# Define source directories with absolute paths
SOURCE_DIRS=("${PROJECT_ROOT}/include/ctti")
FILE_EXTENSIONS=("cpp" "h" "hpp" "inl")

# Parse command line arguments
FIX=0
BUILD_DIR="build"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --fix)
            FIX=1
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Usage: $0 [--fix] [--build-dir BUILD_DIR]"
            exit 1
            ;;
    esac
done

# Convert BUILD_DIR to absolute path if it's not already
if [[ "$BUILD_DIR" != /* ]]; then
    # If BUILD_DIR is a relative path, make it absolute from the current directory
    BUILD_DIR="$(pwd)/$BUILD_DIR"
fi

# Check if compile_commands.json exists
if [[ ! -f "${BUILD_DIR}/compile_commands.json" ]]; then
    print_error "Error: compile_commands.json not found in ${BUILD_DIR}."
    print_error "Please build the project first with CMAKE_EXPORT_COMPILE_COMMANDS=ON."
    exit 1
fi

# Find all source files
find_command="find ${SOURCE_DIRS[*]} -type f ! -path \"${PROJECT_ROOT}/third-party/*\""

# Add extensions to the find command
extension_pattern=""
for ext in "${FILE_EXTENSIONS[@]}"; do
    if [[ -z "$extension_pattern" ]]; then
        extension_pattern="-name \"*.$ext\""
    else
        extension_pattern="$extension_pattern -o -name \"*.$ext\""
    fi
done

find_command="$find_command \( $extension_pattern \)"

# Execute the find command to get all source files
mapfile -t SOURCE_FILES < <(eval "$find_command")

if [[ ${#SOURCE_FILES[@]} -eq 0 ]]; then
    print_warning "No source files found. Checked directories: ${SOURCE_DIRS[*]}"
    exit 0
fi

print_info "Found ${#SOURCE_FILES[@]} source files to process."

# Process files
TIDY_OPTIONS="-p=${BUILD_DIR}"
if [[ $FIX -eq 1 ]]; then
    TIDY_OPTIONS="${TIDY_OPTIONS} --fix"
fi

HAS_ERRORS=0
for file in "${SOURCE_FILES[@]}"; do
    print_info "Linting: $file"
    if ! clang-tidy $TIDY_OPTIONS "$file"; then
        print_error "Linting failed for $file"
        HAS_ERRORS=1
    fi
done

if [[ $HAS_ERRORS -eq 0 ]]; then
    print_success "All files passed linting successfully."
    exit 0
else
    print_error "Linting failed for some files."
    exit 1
fi
