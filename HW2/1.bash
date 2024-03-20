#!/bin/bash

# Функция вывода результата
print_result() {
  if [ $1 -eq 0 ]; then
    echo "Успех: процесс с PID $2 работает."
  else
    echo "Ошибка: процесс с PID $2 не найден."
  fi
}

# Функция проверки процесса по PID
check_process() {
  local pid=$1
  while true; do
    if ps -p $pid > /dev/null; then
      print_result 0 $pid
      break
    else
      print_result 1 $pid
      break
    fi
  done
}

# Вызов функции проверки с PID, предоставленным пользователем
check_process $1