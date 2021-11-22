numberOfRouters=$(cat inputs/routers.config | grep  -v -e '^$' | wc --l | awk '{print $1}')

for refLine in `seq 1 $numberOfRouters`
do
    routerId=$(cat inputs/routers.config | head -n +$refLine | tail -n 1 | awk '{print $1;}')
    ./create-router.sh $routerId
done