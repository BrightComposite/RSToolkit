#!/bin/bash

function incorrect_usage {
	echo Incorrect usage!
	usage
}

function usage {
	echo "Usage:"
	echo "* make - asd make <target> [targets...] - make project files for specified targets"
	echo "* add - asd add module|test|project <name>"
	exit
}

#----------------------------------------

if [[ $# -lt 1 ]] ; then
	incorrect_usage
	exit
fi

command=$1
shift

case ${command} in
    make*)
		old=${IFS}
		IFS=";"
		args="$*"
		IFS=${old}
		exec cmake -D BUILD_TARGET=${args} -P ./tools/asd-make.cmake
    ;;
    add*)
		if [[ $# -lt 2 ]] ; then
			incorrect_usage
		fi

		type=$1
		name=$2

		case ${type} in
			module*)
				append_string="add_module(${name} modules)"
			;;
			test*)
				append_string="add_module(${name} tests)"
			;;
			project*)
				append_string="add_module(${name} projects)"
			;;
			vendor*)
				mkdir ./vendor/${name}
				echo "" > ./vendor/${name}/setup.cmake
				echo "Vendor library ${name} successfully added"
				exit
			;;
			*)
				incorrect_usage
			;;
		esac

		echo "${append_string}" >> CMakeLists.txt
    ;;
    *)
		incorrect_usage
    ;;
esac