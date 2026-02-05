#!/usr/bin/env sh

set -e

BLUE='\033[0;34m'
RESET='\033[0m'

PATH_PROJECT=$(dirname "${0}")
cd "${PATH_PROJECT}"

PRESET="debug"
DEBUGGER=false
while [ ${#} -gt 0 ]; do
    arg="${1}"
    shift
    case "${arg}" in
        "-h" | "--help")
            echo "Usage"
            echo "    ${0} [-h|--help] [-p|--preset <PRESET>] [-d|--debugger] [-- <ARGS TO APP>]"
            echo
            echo "Arguments"
            echo "    -h|--help             Print this menu."
            echo "    -p|--preset <PRESET>  Defaults to \`debug\`."
            echo "                          Use a specific CMake preset named <PRESET> when building the application."
            echo "    -d|--debugger         Attach a debugger."
            echo "    -- <ARGS TO APP>      Defaults to empty."
            echo "                          Pass arguments to the executable."
            exit 0
            ;;
        "-p" | "--preset")
            if [ -z "${1}" ]; then
                >&2 echo "${arg} must be followed by a preset name."
                exit 1
            fi
            PRESET="${1}"
            shift
            ;;
        "-d" | "--debugger")
            DEBUGGER=true
            ;;
        "--")
            break
            ;;
        *)
            >&2 echo "${arg} is not recognized. See \`--help\`."
            exit 2
            ;;
    esac
done

printf "${BLUE}# Build${RESET}\n"
cmake --preset "${PRESET}"
cmake --build --preset "${PRESET}"

printf "${BLUE}# Run${RESET}\n"
if [ "${DEBUGGER}" = false ]; then
    "./build/${PRESET}/assignment-3" ${@}
elif command -v gdb; then
    unset DEBUGINFOD_URLS
    gdb -ex run --args "./build/${PRESET}/assignment-3" ${@}
else
    >&2 echo "No debugger preset."
    exit -1
fi

