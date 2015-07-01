#/bin/sh

CURRENT_DIR=$(pwd)
for GITDIR in $(find -name ".git")
do
  GITDIR=$(dirname ${GITDIR})
  cd ${GITDIR}
  echo "----------------------"
  echo "GIT: ${GITDIR}"
  git $@
  cd ${CURRENT_DIR}
done
