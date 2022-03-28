#!/bin/bash


user=$USER
deploy_dir=/home/$user/deploy_autonanopore
proj_dir=$deploy_dir/autonanopore

deploy=$1

status_code_file=$(pwd)/status_code.info
py_env_log=$(pwd)/py_env.log


url_pip=http://172.16.18.2:9087/simple/
url_git=http://172.16.18.2:9103
url_version=http://172.16.18.2:9101/version/autonanopore
repo_autonanopore=http://172.16.18.2:9103/autonanopore.git


check_backend(){
  ver_file=$proj_dir/deploy/supervisor/supervisord.conf
  if [ -f "$ver_file" ]; then
    echo 1
  else
    echo 0
  fi
}
update_backend(){
  echo "Update backend ..."
  if [ "$version_info" == "git pull" ]; then
    echo "Run git pull"
    if [ $deploy == 1 ]; then
      cd /home/$user/deploy_autonanopore/autonanopore
#      git checkout /home/$user/deploy_autonanopore/autonanopore/deploy/supervisor/supervisord.conf  && git pull
      git checkout *.conf && git pull
      # update supervisord.conf
      cd /home/$user/deploy_autonanopore/autonanopore/deploy/supervisor
      bash delete_row.sh  supervisord.conf    pop_data.conf
    fi
  else
    echo "Run git checkout $version_online"
    if [ $deploy == 1 ]; then
      cd /home/$user/deploy_autonanopore/autonanopore
      git checkout $version_online
    fi
  fi
}
auto_install_backend(){
  mkdir  /home/$user/deploy_autonanopore
  cd /home/$user/deploy_autonanopore
  echo "Run git clone $repo_autonanopore"
  if [ "$deploy" == 1 ]; then
    git clone $repo_autonanopore
  fi

  # build env
  cd /home/$user/deploy_autonanopore/autonanopore/env
  echo "Start build env ..."
  if [ $deploy == 1 ]; then
    bash build_py37_env.sh  >  $py_env_log
  fi
}


check_pip(){
  status_code=$(curl -m 5 -s -o /dev/null -w %{http_code} $url_pip)
  if [ $status_code == "200" ]; then
    echo 1
  else
    echo 0
  fi
}
check_git(){
  status_code=$(curl -m 5 -s -o /dev/null -w %{http_code} $url_git)
  if [ $status_code == "200" ]; then
    echo 1
  else
    echo 0
  fi
}
check_version(){
  status_code=$(curl -m 5 -s -o /dev/null -w %{http_code} $url_version)
  if [ $status_code == "200" ]; then
    version_info=$(curl  -s  $url_version)
    echo 1 "$version_info"
  else
    echo 0 $status_code
  fi
}
service_check(){
  res_version=$(check_version)
  version_info=$(echo  "$res_version" | awk  '{ string=substr($0,3); print string; }')
  is_find_version=$(echo  "$res_version" | awk  '{ string=substr($0,1,1); print string; }')
  if [ $(check_pip) -eq 1 ] && [ $(check_git) -eq 1 ] && [ $is_find_version -eq 1 ]; then
    echo 1
  else
    echo 0
  fi
}


run_backend(){
  if [ "$deploy" == 1 ]; then
    echo "Delete unnecessary worker ..."
    cd /home/$user/deploy_autonanopore/autonanopore/deploy/supervisor
    bash delete_row.sh  supervisord.conf    pop_data.conf
    echo "Setting maxmemory for redis ..."
    cd /home/$user/deploy_autonanopore/autonanopore/deploy/redis
    sed -i 's/#maxmemory-setting/maxmemory 7000000000/g'  redis.conf

    echo "Execute run.sh ..."
    cd /home/$user/deploy_autonanopore/autonanopore
    bash run.sh  "online"
  fi
}
check_run_backend(){
  flag_file=$proj_dir/deploy/backend_started.info
  if [ -f "$flag_file" ]; then
    echo 1
  else
    echo 0
  fi
}
repeat_run_backend(){
  echo "Repeat run backend ..."
  num=1
  while [ $num -le 2 ]
  do
    run_backend
    res=$(check_run_backend)
    if [ $res -eq 1 ]; then
      echo "Start successfully! try $num time"
      cd $proj_dir/deploy
      flag_file=$proj_dir/deploy/backend_started.info
      rm -f "$flag_file"
      echo 200  > $status_code_file
      break
    else
      echo "Start failure! try $num time."
      if [ $num -eq 2 ]; then
        # if after two times try still failure, then echo 500
        echo 500 > $status_code_file
      fi
    fi
    let num++
  done
}



main(){
  time=$(date "+%Y-%m-%d %H:%M:%S")
  echo -e "\n==============" "$time" "==============="
  echo "Pip status: $(check_pip)"
  echo "Git status: $(check_git)"
  echo "Version status: $(check_version)"
  res=$(service_check)
  if [ $res -eq 1 ]; then
    echo "Services ready"
    echo "Start check if old backend exists or not ..."
    if [ $(check_backend) -eq 1 ]; then
      echo "Find out old backend, start version checking ... "
    else
      echo "Did not find out old backend, start auto installation ... "
      auto_install_backend
    fi

    res_version=$(check_version)
    version_info=$(echo  "$res_version" | awk  '{ string=substr($0,3); print string; }')
    is_find_version=$(echo  "$res_version" | awk  '{ string=substr($0,1,1); print string; }')
    echo "Get res_version info is $is_find_version----$version_info"
    echo "Start updating backend ..."
    update_backend

    echo "Start run backend ..."
    repeat_run_backend

  else
    echo "Services not ready!!"
    echo "Start check if old backend exists or not ..."
    if [ $(check_backend) -eq 1 ]; then
      echo "Find out old backend, start run old backend"
      repeat_run_backend
    else
      echo 500 > $status_code_file
    fi

  fi
}


main