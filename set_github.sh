#/bin/sh

CURRENT_DIR=$(pwd)
for GITDIR in $(find -name ".git")
do
  GITDIR=$(dirname ${GITDIR})
  cd ${GITDIR}
  echo "----------------------"
  echo "GIT: ${GITDIR}"
  NAME=$(git remote -v | head -n 1 | sed "s/.*\/\(.*\)\.git .*/\1/g")
  echo ${NAME}
  git remote set-url origin git@github.com:cocobot/${NAME}.git
  cd ${CURRENT_DIR}
done
