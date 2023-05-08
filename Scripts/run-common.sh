
run_player() {
    bin=$1
    shift
    if ! test -e Scripts/logs; then
        mkdir Scripts/logs
    fi
    params="-f 1 $*"
    rem=$(($players - 2))
    for i in $(seq 0 $rem); do
      echo "trying with player $i"
      >&2 echo Running ../$bin $i $params
      ./$bin $i $params 2>&1 | tee -a Scripts/logs/$i  &
      #valgrind --tool=memcheck --leak-check=yes ./$bin $i $params 2>&1 | tee -a Scripts/logs/$i  &
    done
    last_player=$(($players - 1))
    >&2 echo Running ../$bin $last_player $params
    ./$bin $last_player $params >> Scripts/logs/$last_player 2>&1 || return 1
}

killall player || echo "no previous processes running"
killall PlayerBinary.x || echo "no previous processes running"
sleep 0.5

declare -i players=$(sed -n 2p Data/NetworkData.txt)

#. Scripts/setup.sh
