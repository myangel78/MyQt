#!/bin/bash


start(){
  echo "start"
  log_file=$(pwd)/deploy_autonanopore.log
  echo "run bash xxxx.sh"
  bash ./self_check.sh  1  >>  $log_file
}


stop(){
  echo "stop"
  ps auxww | grep monitor.py | awk '{print $2}' | xargs kill -9
  ps auxww | grep pop_msgpack_data_from_redis.py | awk '{print $2}' | xargs kill -9
  ps auxww | grep 'python37_env/bin/celery' | awk '{print $2}' | xargs kill -9
  ps auxww | grep 'python37_env/bin/supervisor' | awk '{print $2}' | xargs kill -9
  rediscli=$(which redis-cli)
  $rediscli -p 9099 flushall
  $rediscli -p 9099 shutdown
}


if [ ! -n "$1" ] ;then
  echo "Not find parameters: start or stop"
  exit
else

  if [ "$1" == "start" ] || [ "$1" == "stop" ]; then
    cmd=$1
    if [ "$cmd" == "start" ]; then
      sudo mkdir /home/root
      rm -f status_code.info
      start
    else
      stop
    fi
  else
    echo "Parameters must be start or stop"
    exit
  fi

fi

