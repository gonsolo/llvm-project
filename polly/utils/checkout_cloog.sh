#!/bin/sh

CLOOG_HASH="2d8b7c6b43ee46fee978a57fa6877de49675f357"
ISL_HASH="1b3ba3b72c0482fd36bf0b4a1186a259f7bafeed"

PWD=`pwd`

check_command_line() {
  if [ $# -eq 1 ]
  then
    CLOOG_DIR="${1}"
  else
      echo "Usage: " ${0} '<Directory to checkout CLooG>'
      exit 1
  fi
}

check_cloog_directory() {
  if ! [ -e ${CLOOG_DIR} ]
  then
    echo :: Directory "'${CLOOG_DIR}'" does not exists. Trying to create it.
    if ! mkdir -p "${CLOOG_DIR}"
    then exit 1
    fi
  fi

  if ! [ -d ${CLOOG_DIR} ]
  then
    echo "'${CLOOG_DIR}'" is not a directory
    exit 1
  fi

  # Make it absolute
  cd ${CLOOG_DIR}
  CLOOG_DIR=`pwd`

  if ! [ -e "${CLOOG_DIR}/.git" ]
  then
    echo ":: No git checkout found"
    IS_GIT=0
  else
    echo ":: Existing git repo found"
    IS_GIT=1
  fi
}

complain() {
  echo "$@"
  exit 1
}

run() {
  $cmdPre $*
  if [ $? != 0 ]
    then
    complain $* failed
  fi
}

check_command_line $@
check_cloog_directory

ISL_DIR=${CLOOG_DIR}/isl

if [ ${IS_GIT} -eq 0 ]
then
  echo :: Performing initial checkout
  # Remove the existing CLooG and ISL dirs to avoid crashing older git versions.
  cd ${CLOOG_DIR}/..
  run rmdir "${CLOOG_DIR}"
  run git clone http://repo.or.cz/r/cloog.git ${CLOOG_DIR}
  run rmdir "${ISL_DIR}"
  run git clone http://repo.or.cz/r/isl.git ${ISL_DIR}
fi

echo :: Fetch versions required by Polly
run cd ${CLOOG_DIR}
run git remote update
run cd isl
run git remote update

echo :: Setting CLooG version
run cd ${CLOOG_DIR}
run git reset --hard "${CLOOG_HASH}"

echo :: Setting isl version
run cd ${ISL_DIR}
run git reset --hard "${ISL_HASH}"

echo :: Generating configure
run cd ${CLOOG_DIR}
run ./autogen.sh

echo :: If you install cloog/isl the first time run "'./configure'" followed by
echo :: "'make'" and "'make install'", otherwise, just call "'make'" and
echo :: "'make'" install.
