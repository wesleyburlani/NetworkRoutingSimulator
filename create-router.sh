## Creates a router with specified argument id

if [ $# -eq 0 ]; then
    echo "You need to specify one argument to be the router id."
    exit 1
fi

docker-compose build --no-cache
gnome-terminal -- docker-compose run program ./bin/program $@