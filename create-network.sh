number_of_routers=$(cat inputs/routers.config | grep  -v -e '^$' | wc --l | awk '{print $1}')

for current_line in `seq 1 $number_of_routers`
do
    router_id=$(cat inputs/routers.config | head -n +$current_line | tail -n 1 | awk '{print $1;}')
    ./create-router.sh $router_id
done