#!/bin/sh
# This is a simple helper script to setup git user on shared machine

if [ "$1" = "" ];
then
	>&2 echo "usage: ./set_git_user.sh [xlogin]"
	exit 1
fi
git config user.email "$1@student.42.fr" &&
	git config user.name "$1" &&
	>&2 echo "SUCCES" || >&2 echo "FAILURE"
